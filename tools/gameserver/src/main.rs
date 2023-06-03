mod matchmaking;
mod room;
mod unreliable_socket;

use std::sync::Arc;

use anyhow::{Context, Result};
use clap::Parser;
use dashmap::mapref::entry::Entry;
use dashmap::DashMap;
use futures_util::{SinkExt, StreamExt};
use libhydrogen::{kx, secretbox};
use prost::Message as _;
use tokio::net::{TcpListener, TcpStream};
use tokio::sync::mpsc;
use tokio_tungstenite::{MaybeTlsStream, WebSocketStream};

use crate::room::Room;
use matchmaking::Message;
use netprotocol::{
    async_stream::AsyncStream,
    matchmaking::{gts_message, stg_message, GTSMessage, GTSMessageOpt, STGMessage, STGMessageOpt},
    room_protocol,
    room_protocol::{room_request, RoomEventOpt, RoomRequest, RoomRequestOpt},
};

type RoomAsyncStream = AsyncStream<RoomRequestOpt, RoomEventOpt, netprotocol::XXNegotiator>;

#[derive(Clone, Debug)]
pub enum ServerConnection {
    Client(mpsc::Sender<(RoomAsyncStream, room_request::Join)>),
    Matchmaking(Arc<DashMap<u16, mpsc::Sender<(RoomAsyncStream, room_request::Join)>>>),
}

#[derive(clap::Parser, Debug)]
#[command(about)]
struct Args {
    #[arg(long)]
    /// Websocket address to central server.
    central_address: url::Url,
    #[arg(long)]
    /// ID for the gameserver, must be unique.
    gameserver_id: u16,
    #[arg(long)]
    /// IP address for clients to connect to reach this gameserver
    gameserver_ip: std::net::Ipv4Addr,
    #[arg(long)]
    /// Maximum number of rooms to host.
    max_rooms: u16,
}

#[tokio::main]
async fn main() -> Result<()> {
    libhydrogen::init()?;
    tracing_subscriber::fmt::init();

    let arg_count = std::env::args().skip(1).len();
    let server_conn;
    if arg_count == 0 {
        tracing::info!("No arguments passed, starting as standalone server.");
        server_conn = ServerConnection::Client(spawn_room(None));
    } else {
        tracing::info!("Arguments passed, starting as part of matchmaking pool.");

        let args = Args::parse();
        let rooms = Arc::new(DashMap::new());
        server_conn = ServerConnection::Matchmaking(rooms.clone());

        let ws = tokio_tungstenite::connect_async(args.central_address).await?.0;
        tokio::spawn(central_listener(
            ws,
            rooms,
            args.gameserver_ip,
            args.gameserver_id,
            args.max_rooms,
        ));
    }

    let server_keypair = kx::KeyPair::gen();
    tracing::debug!("Public key: {:02x?}", server_keypair.public_key.as_ref());

    client_listener(server_keypair, server_conn).await
}

async fn client_listener(server_keypair: kx::KeyPair, server_conn: ServerConnection) -> Result<()> {
    let listener = TcpListener::bind("0.0.0.0:21330").await?;
    tracing::info!("Listening on 21330!");

    loop {
        let (stream, peer_addr) = match listener.accept().await {
            Ok((stream, peer_addr)) => {
                tracing::info!("{peer_addr}: Accepted connection");
                (stream, peer_addr)
            }
            Err(e) => {
                tracing::error!("Failed to accept connection: {e}");
                continue;
            }
        };

        let server_keypair = server_keypair.clone();
        let server_conn = server_conn.clone();

        tokio::spawn(async move {
            match handle_client(stream, server_keypair, server_conn).await {
                Ok(()) => {
                    tracing::info!("{peer_addr}: Successfully initialized client");
                }
                Err(e) => {
                    tracing::error!("{peer_addr}: Failed to initialize client: {e}");
                }
            }
        });
    }
}

async fn central_listener(
    mut ws: WebSocketStream<MaybeTlsStream<TcpStream>>,
    rooms: Arc<DashMap<u16, mpsc::Sender<(RoomAsyncStream, room_request::Join)>>>,
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

    let mut waiting_client: Option<matchmaking::ClientIdOpt> = None;
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
                let (room_id, waiting_client) = match (room_id, waiting_client.clone()) {
                    (Some(room_id), _) => (room_id as u16, None),
                    (None, None) => {
                        // No waiting client, and new room needs making, wait
                        // until another client joins until sending a response.
                        waiting_client = Some(client_id);
                        continue;
                    }
                    (None, Some(_)) => loop {
                        let room_id = rand::random::<u16>();

                        match rooms.entry(room_id) {
                            Entry::Occupied(_) => continue,
                            Entry::Vacant(entry) => {
                                entry.insert(spawn_room(Some(matchmaking::State {
                                    room_id,
                                    ws_conn: ws_send.clone(),
                                })));

                                break (room_id, waiting_client.take());
                            }
                        }
                    },
                };

                let client_ids = Some(client_id).into_iter().chain(waiting_client.into_iter());
                for client_id in client_ids {
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
    }

    Ok(())
}

fn spawn_room(
    match_state: Option<matchmaking::State>,
) -> mpsc::Sender<(RoomAsyncStream, room_request::Join)> {
    let (connect_tx, connect_rx) = mpsc::channel(32);

    tokio::spawn(async move {
        let mut room = Room::new(connect_rx, match_state);
        room.handle().await
    });

    connect_tx
}

async fn handle_client(
    stream: TcpStream,
    server_keypair: kx::KeyPair,
    server_conn: ServerConnection,
) -> Result<()> {
    let context = secretbox::Context::from(*b"room    ");
    let mut stream = RoomAsyncStream::new(stream, server_keypair, context).await?;

    let request = stream.read().await?.context("Connection closed unexpectedly!")?;
    let join = match request.request {
        Some(RoomRequest::Join(join)) => join,
        _ => anyhow::bail!("Unexpected request type!"),
    };

    let connect_tx = match &join.login_info {
        Some(login_info) => {
            let ServerConnection::Matchmaking(rooms) = server_conn else {
                anyhow::bail!("Provided connection token to private room!");
            };

            let room_id = login_info.room_id as u16;
            let conn = rooms.get(&room_id).context("Invalid connection token!")?;

            conn.clone()
        }
        None => match server_conn {
            ServerConnection::Client(connect_tx) => connect_tx,
            ServerConnection::Matchmaking(_) => anyhow::bail!("No connection token provided!"),
        },
    };

    if join.miis.len() != 1 {
        anyhow::bail!("Invalid Mii count!");
    }
    if join.miis.iter().any(|mii| mii.len() != 76) {
        anyhow::bail!("Invalid Mii size!");
    }
    if join.region_line_color >= 6 {
        anyhow::bail!("Invalid region line color!");
    }
    // TODO move settings to the protocol for better checks
    if join.settings.len() != 6 {
        anyhow::bail!("Invalid setting count!");
    }

    connect_tx.send((stream, join)).await?;
    Ok(())
}
