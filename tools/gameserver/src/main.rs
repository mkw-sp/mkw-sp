mod client;
mod event;
mod matchmaking;
mod request;
mod room;

use anyhow::Result;
use dashmap::mapref::entry::Entry;
use dashmap::DashMap;
use futures_util::{SinkExt, StreamExt};
use libhydrogen::kx;
use prost::Message as _;
use std::collections::HashMap;
use std::sync::Arc;
use tokio::net::{TcpListener, TcpStream};
use tokio::sync::mpsc;
use tokio_tungstenite::{MaybeTlsStream, WebSocketStream};

use crate::client::Client;
use crate::request::Request;
use crate::room::Room;
use matchmaking::Message;
use netprotocol::{
    matchmaking::{gts_message, stg_message, GTSMessage, GTSMessageOpt, STGMessage, STGMessageOpt},
    room_protocol,
};

#[derive(Clone, Debug)]
pub enum ServerConnection {
    Client(mpsc::Sender<Request>),
    Matchmaking(Arc<DashMap<u16, mpsc::Sender<Request>>>),
}

#[tokio::main]
async fn main() -> Result<()> {
    libhydrogen::init()?;
    tracing_subscriber::fmt::init();

    let mut args = std::env::args().skip(1);

    let server_conn;
    match (args.next(), args.next(), args.next(), args.next()) {
        (Some(central_address), Some(gameserver_id), Some(gameserver_ip), Some(max_rooms)) => {
            let rooms = Arc::new(DashMap::new());
            server_conn = ServerConnection::Matchmaking(rooms.clone());

            let ws = tokio_tungstenite::connect_async(central_address).await?.0;
            tokio::spawn(central_listener(
                ws,
                rooms,
                gameserver_ip.parse()?,
                gameserver_id.parse()?,
                max_rooms.parse()?,
            ));
        }
        _ => {
            server_conn = ServerConnection::Client(spawn_room(None));
        }
    }

    let server_keypair = kx::KeyPair::gen();
    tracing::debug!("Public key: {:02x?}", server_keypair.public_key.as_ref());

    client_listener(server_keypair, server_conn).await
}

async fn client_listener(server_keypair: kx::KeyPair, server_conn: ServerConnection) -> Result<()> {
    let listener = TcpListener::bind("0.0.0.0:21330").await?;
    tracing::info!("Listening on 21330!");

    loop {
        let stream = match listener.accept().await {
            Ok((stream, _)) => stream,
            Err(err) => {
                tracing::error!("Failed to accept connection: {err}");
                continue;
            }
        };

        let peer_addr = stream.peer_addr()?;
        tracing::info!("{peer_addr}: Accepted connection");

        let server_keypair = server_keypair.clone();
        let server_conn = server_conn.clone();

        tokio::spawn(async move {
            let mut client = match Client::new(stream, server_keypair, server_conn).await {
                Ok(client) => client,
                Err(err) => return tracing::error!("Failed to create client: {err}"),
            };

            tracing::info!("{peer_addr}: Created client");
            match client.handle().await {
                Ok(_) => tracing::info!("{peer_addr}: Client disconnected"),
                Err(err) => tracing::error!("{peer_addr}: Client disconnected: {err}"),
            }
        });
    }
}

async fn central_listener(
    mut ws: WebSocketStream<MaybeTlsStream<TcpStream>>,
    rooms: Arc<DashMap<u16, mpsc::Sender<Request>>>,
    room_ip: std::net::Ipv4Addr,
    gameserver_id: u16,
    max_rooms: u16,
) -> Result<()> {
    let (ws_send, mut ws_recv) = mpsc::unbounded_channel();
    let register = GTSMessage::AddServer(gts_message::AddServer {
        gameserver_id: gameserver_id as u32,
        max_rooms: max_rooms as u32,
    });

    ws.send(
        GTSMessageOpt {
            message: Some(register),
        }
        .encode_to_vec()
        .into(),
    )
    .await?;
    loop {
        let msg = tokio::select! {
            msg = ws.next() => if let Some(msg) = msg {msg} else {break},
            msg = ws_recv.recv() => {
                match msg {
                    Some(Message::Update {room_id, client_id, is_join, is_host}) => {
                        if is_host && !is_join {
                            rooms.remove(&room_id);
                        }

                        ws.send(GTSMessageOpt{message: Some(GTSMessage::ClientUpdate(gts_message::ClientUpdate {
                            is_join,
                            is_host,
                            client_id,
                            room_id: room_id as u32,
                        }))}.encode_to_vec().into()).await?;
                    },
                    None => break
                }
                continue
            }
        };

        let STGMessageOpt {message: Some(msg)} = STGMessageOpt::decode(&*msg?.into_data())? else {
            anyhow::bail!("Failed to decode message!")
        };

        match msg {
            STGMessage::TokenRequest(stg_message::RequestToken {
                client_id,
                room_id,
            }) => {
                let room_id = match room_id {
                    Some(room_id) => room_id as u16,
                    None => loop {
                        let room_id = rand::random::<u16>();

                        match rooms.entry(room_id) {
                            Entry::Occupied(_) => continue,
                            Entry::Vacant(entry) => {
                                entry.insert(spawn_room(Some(matchmaking::State {
                                    room_id,
                                    ws_conn: ws_send.clone(),
                                    client_lookup: HashMap::new(),
                                })));
                                break room_id;
                            }
                        }
                    },
                };

                // TODO: Actually check if the client is allowed to join the room
                let token = 0;
                ws.send(
                    GTSMessageOpt {
                        message: Some(GTSMessage::TokenResponse(gts_message::TokenResponse {
                            room_ip: room_ip.into(),
                            login_info: matchmaking::LoginInfo {
                                token,
                                client_id,
                                room_id: room_id as u32,
                            },
                        })),
                    }
                    .encode_to_vec()
                    .into(),
                )
                .await?;
            }
        }
    }

    Ok(())
}

fn spawn_room(match_state: Option<matchmaking::State>) -> mpsc::Sender<Request> {
    let (tx, rx) = mpsc::channel(32);

    tokio::spawn(async move {
        let mut room = Room::new(rx, match_state);
        room.handle().await
    });

    tx
}
