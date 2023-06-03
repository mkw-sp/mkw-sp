mod ids;
mod tcp_forward;
mod ws_forward;

use std::{
    collections::{HashMap, HashSet},
    sync::Arc,
};

use anyhow::Result;
use dashmap::DashMap;
use futures_util::{SinkExt, StreamExt};
use ids::*;
use netprotocol::{
    async_stream::AsyncStream,
    matchmaking::{
        cts_message, gts_message, stc_message, stg_message, CTSMessage, CTSMessageOpt, ClientIdOpt,
        GTSMessage, GTSMessageOpt, STCMessage, STCMessageOpt, STGMessage, STGMessageOpt,
    },
};
use prost::Message;
use tokio::net::TcpStream;
use tokio_tungstenite::{tungstenite::Message as WebSocketMessage, WebSocketStream};

type Fallible = Result<()>;
type GameserverMessageSender = tokio::sync::mpsc::UnboundedSender<STGMessage>;
type OneshotSender = tokio::sync::oneshot::Sender<gts_message::TokenResponse>;
type GameserverMessageReceiver = tokio::sync::mpsc::UnboundedReceiver<STGMessage>;

#[allow(non_upper_case_globals)]
const Success: Fallible = Ok(());

#[derive(Debug, Default)]
struct Room {
    clients: HashSet<ClientId>,
    is_battle: bool,
}

#[derive(Debug)]
struct Gameserver {
    rooms: HashMap<RoomId, Room>,
    sender: GameserverMessageSender,
}

impl Gameserver {
    pub fn new(sender: GameserverMessageSender, max_rooms: usize) -> Self {
        Self {
            rooms: HashMap::with_capacity(max_rooms),
            sender,
        }
    }
}

#[derive(Default, Clone)]
pub struct Server {
    waiting_clients: Arc<DashMap<ClientId, OneshotSender>>,
    gameservers: Arc<DashMap<GameserverID, Gameserver>>,
}

impl Server {
    pub fn new() -> Self {
        Self::default()
    }

    /// Requests for a room to be opened on either:
    /// - The room with the most members
    /// - The gameserver with the least rooms
    #[async_recursion::async_recursion]
    async fn request_room(
        &self,
        client_id: ClientId,
        gamemode: u32,
    ) -> Result<gts_message::TokenResponse> {
        let is_battle = gamemode == 1;
        let mut least_room = None;
        let mut highest_member_count = None;

        for gameserver in self.gameservers.iter() {
            let (gameserver_id, gameserver) = gameserver.pair();

            for (room_id, room) in &gameserver.rooms {
                if room.is_battle == is_battle && room.clients.len() < 12 {
                    if let Some((highest_member_count, _, _)) = highest_member_count {
                        if highest_member_count > room.clients.len() as u8 {
                            continue;
                        }
                    }

                    highest_member_count =
                        Some((room.clients.len() as u8, *gameserver_id, *room_id));
                }
            }

            if highest_member_count.is_none() {
                if let Some((least_room_count, _)) = least_room {
                    if least_room_count > gameserver.rooms.len() as u8 {
                        continue;
                    }
                }

                least_room = Some((gameserver.rooms.len() as u8, *gameserver_id));
            }
        }

        let (gameserver_id, room_id) = match (highest_member_count, least_room) {
            (Some((_, gameserver_id, room_id)), _) => (gameserver_id, Some(room_id)),
            (None, Some((_, gameserver_id))) => (gameserver_id, None),
            (None, None) => anyhow::bail!("No gameservers found!"),
        };

        let message = STGMessage::TokenRequest(stg_message::RequestToken {
            room_id: room_id.map(|r| r.0 as u32),
            client_id: ClientIdOpt {
                inner: Some(client_id.into()),
            },
        });

        if let Some(gameserver) = self.gameservers.get(&gameserver_id) {
            let (resp_tx, resp_rx) = tokio::sync::oneshot::channel();
            self.waiting_clients.insert(client_id, resp_tx);
            gameserver.sender.send(message)?;

            resp_rx.await.map_err(Into::into)
        } else {
            tracing::warn!(
                "Gameserver {gameserver_id} deleted before token request could be sent, retrying"
            );
            self.request_room(client_id, gamemode).await
        }
    }

    pub async fn client_listener(
        &self,
        mut tcp: AsyncStream<CTSMessageOpt, STCMessageOpt, netprotocol::XXNegotiator>,
        client_id: ClientId,
    ) -> Fallible {
        while let Some(msg) = tcp.read().await? {
            let Some(msg) = msg.message else {todo!("Handle client disconnect")};

            match msg {
                CTSMessage::StartMatchmaking(cts_message::StartMatchmaking {
                    gamemode,
                    trackpack,
                }) => {
                    let server_resp = self.request_room(client_id, gamemode).await?;
                    let client_resp = stc_message::FoundMatch {
                        login_info: server_resp.login_info,
                        room_ip: server_resp.room_ip,
                    };

                    tcp.write(&STCMessageOpt {
                        message: Some(STCMessage::FoundMatch(client_resp)),
                    })
                    .await?;
                }
                _ => todo!(),
            }
        }

        tracing::info!("{client_id}: Disconnected");
        Success
    }

    pub async fn gameserver_listener(
        &self,
        mut ws: WebSocketStream<TcpStream>,
        mut reciever: GameserverMessageReceiver,
        gameserver_id: GameserverID,
    ) -> Fallible {
        loop {
            let msg = tokio::select! {
                Some(msg) = reciever.recv() => {
                    let msg = STGMessageOpt {message: Some(msg)};
                    ws.send(WebSocketMessage::Binary(msg.encode_to_vec())).await?;
                    continue
                }
                Some(msg) = ws.next() => msg?,
            };

            let Some(msg) = GTSMessageOpt::decode(&*msg.into_data())?.message else {
                anyhow::bail!("Failed to decode message")
            };

            match msg {
                GTSMessage::TokenResponse(inner) => {
                    let client_id = inner.login_info.client_id.clone().into();
                    if let Some((_, resp_chan)) = self.waiting_clients.remove(&client_id) {
                        let _ = resp_chan.send(inner);
                    };
                }
                GTSMessage::ClientUpdate(gts_message::ClientUpdate {
                    room_id,
                    client_id,
                    is_join,
                    is_host,
                }) => {
                    let room_id = RoomId(room_id as u16);

                    let mut gameserver = self.gameservers.get_mut(&gameserver_id).unwrap();
                    // "Host" left, remove room
                    if is_host && !is_join {
                        gameserver.rooms.remove(&room_id);
                        continue;
                    }

                    let room = gameserver.rooms.entry(room_id).or_default();

                    if is_join {
                        room.clients.insert(client_id.into());
                    } else {
                        room.clients.remove(&client_id.into());
                    }
                }
                _ => todo!(),
            }
        }
    }
}

#[tokio::main]
async fn main() -> Fallible {
    libhydrogen::init()?;
    tracing_subscriber::fmt::init();

    let db_pool = sqlx::PgPool::connect(&std::env::var("DATABASE_URL")?).await?;

    let server = Server::new();

    let client_forwarder =
        tcp_forward::ClientForwarder::new("127.0.0.1:21331", server.clone(), db_pool).await?;
    let gameserver_forwarder =
        ws_forward::WebsocketListener::new("127.0.0.1:21332", server.clone()).await?;

    tokio::select! {
        _ = client_forwarder.accept_loop() => {}
        _ = gameserver_forwarder.accept_loop() => {}
    };

    Success
}
