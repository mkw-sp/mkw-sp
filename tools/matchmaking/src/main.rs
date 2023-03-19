mod ids;
mod tcp_forward;
mod ws_forward;

use std::{
    collections::{HashMap, HashSet},
    sync::Arc,
};

use anyhow::Result;
use dashmap::{DashMap, DashSet};
use futures_util::{SinkExt, StreamExt};
use ids::*;
use netprotocol::{
    async_stream::AsyncStream,
    matchmaking::{
        cts_message, gts_message, stc_message, stc_message::friend_result,
        stc_message::friend_result::Result as FriendResult, stg_message, CTSMessage, CTSMessageOpt,
        ClientIdOpt, GTSMessage, GTSMessageOpt, LoggedInId, STCMessage, STCMessageOpt, STGMessage,
        STGMessageOpt,
    },
};
use prost::Message;
use tokio::{
    net::TcpStream,
    sync::mpsc::{UnboundedReceiver, UnboundedSender},
};
use tokio_tungstenite::{tungstenite::Message as WebSocketMessage, WebSocketStream};

type Fallible = Result<()>;
type GameserverMessageSender = UnboundedSender<STGMessage>;
type GameserverMessageReceiver = UnboundedReceiver<STGMessage>;

#[allow(non_upper_case_globals)]
const Success: Fallible = Ok(());

enum InternalClientMessage {
    FoundMatch(gts_message::TokenResponse),
    NeedsFriendUpdate,
}

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

#[derive(Clone)]
pub struct Server {
    client_tx: Arc<DashMap<ClientId, UnboundedSender<InternalClientMessage>>>,
    gameservers: Arc<DashMap<GameserverID, Gameserver>>,
    waiting_clients: Arc<DashSet<ClientId>>,
    pool: sqlx::PgPool,
}

impl Server {
    pub fn new(pool: sqlx::PgPool) -> Self {
        Self {
            waiting_clients: Arc::new(DashSet::new()),
            gameservers: Arc::new(DashMap::new()),
            client_tx: Arc::new(DashMap::new()),
            pool,
        }
    }

    async fn update_friend_data(
        &self,
        tcp: &mut AsyncStream<CTSMessageOpt, STCMessageOpt>,
        client_id: LoggedInId,
        conn: &mut sqlx::PgConnection,
    ) -> Fallible {
        let device_id = client_id.device as i32;
        let licence_id = client_id.licence as i16;

        let fid_records = sqlx::query!(
            "SELECT friendee_device_id, friendee_licence_id FROM friendships
            WHERE friender_device_id = $1 AND friender_licence_id = $2",
            device_id,
            licence_id
        )
        .fetch_all(&mut *conn)
        .await?;

        let mut friends = Vec::new();
        for fid_record in fid_records {
            let friend = sqlx::query!(
                "SELECT device_id, licence_id, mii, location, latitude, longitude FROM users WHERE device_id = $1 AND licence_id = $2",
                fid_record.friendee_device_id, fid_record.friendee_licence_id as i32
            ).fetch_one(&mut *conn).await?;

            let is_full = sqlx::query!(
                "SELECT exists(
                    SELECT 1 FROM friendships WHERE
                    friender_device_id = $1 AND friender_licence_id = $2 AND
                    friendee_device_id = $3 AND friendee_licence_id = $4
                ) as exists",
                fid_record.friendee_device_id,
                fid_record.friendee_licence_id,
                device_id,
                licence_id,
            )
            .fetch_one(&mut *conn)
            .await?;

            friends.push(stc_message::update_friends_list::Friend {
                client_id: LoggedInId {
                    device: friend.device_id as u32,
                    licence: friend.licence_id as u32,
                },
                mii: friend.mii,
                location: friend.location,
                latitude: friend.latitude,
                longitude: friend.longitude,
                full_friend: is_full.exists.unwrap_or(false),
            });
        }

        let message = STCMessageOpt {
            message: Some(STCMessage::FriendsList(stc_message::UpdateFriendsList {
                friends,
            })),
        };

        tcp.write(&message).await?;
        Success
    }

    /// Requests for a room to be opened on either:
    /// - The room with the most members
    /// - The gameserver with the least rooms
    #[async_recursion::async_recursion]
    async fn request_room(&self, client_id: ClientId, gamemode: u32) -> Result<()> {
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
            self.waiting_clients.insert(client_id);
            gameserver.sender.send(message).unwrap();

            Success
        } else {
            tracing::warn!(
                "Gameserver {gameserver_id} deleted before token request could be sent, retrying"
            );
            self.request_room(client_id, gamemode).await
        }
    }

    pub async fn client_listener(
        &self,
        mut tcp: AsyncStream<CTSMessageOpt, STCMessageOpt>,
        client_id: ClientId,
    ) -> Fallible {
        let (tx, mut rx) = tokio::sync::mpsc::unbounded_channel();
        let _ = tx.send(InternalClientMessage::NeedsFriendUpdate);
        self.client_tx.insert(client_id, tx);

        loop {
            let msg = tokio::select! {
                msg = tcp.read() => {
                    if let Some(msg) = msg? {
                        if let Some(msg) = msg.message {
                            msg
                        } else {
                            anyhow::bail!("Failed to parse Client message")
                        }
                    } else {
                        break;
                    }
                },
                msg = rx.recv() => {
                    match msg {
                        Some(InternalClientMessage::FoundMatch(found)) => {
                            let response = stc_message::FoundMatch {
                                login_info: found.login_info,
                                room_ip: found.room_ip
                            };

                            tcp.write(&STCMessageOpt {
                                message: Some(STCMessage::FoundMatch(response))
                            }).await?;
                        },
                        Some(InternalClientMessage::NeedsFriendUpdate) => {
                            let logged_in = match client_id {
                                ClientId::LoggedIn { device, licence } => LoggedInId { device, licence: licence as u32 },
                                _ => {return Ok(())},
                            };

                            let mut conn = self.pool.acquire().await?;
                            self.update_friend_data(&mut tcp, logged_in, &mut conn).await?;
                        }
                        None => {return Ok(())},
                    }

                    continue;
                }
            };

            match msg {
                CTSMessage::StartMatchmaking(cts_message::StartMatchmaking {
                    gamemode,
                    trackpack,
                }) => self.request_room(client_id, gamemode).await?,
                CTSMessage::FriendRequest(cts_message::FriendRequest {
                    device_id: friend_device_id,
                    licence_id: friend_licence_id,
                    is_delete,
                }) => {
                    let ClientId::LoggedIn { device, licence } = client_id else {
                        anyhow::bail!("Friend request from non-logged in client");
                    };

                    // TODO: Ratelimit this
                    let mut conn = self.pool.acquire().await?;
                    if is_delete {
                        sqlx::query!(
                            "DELETE FROM friendships WHERE
                            friender_device_id = $1 AND friender_licence_id = $2 AND
                            friendee_device_id = $3 AND friendee_licence_id = $4",
                            device as i32,
                            licence as i32,
                            friend_device_id as i32,
                            friend_licence_id as i32
                        )
                        .execute(&mut *conn)
                        .await?;

                        let friend_id = ClientId::LoggedIn { device: friend_device_id, licence: friend_licence_id as u16 };

                        let _ = self.client_tx.get(&client_id).unwrap().send(InternalClientMessage::NeedsFriendUpdate);
                        if let Some(friend_tx) = self.client_tx.get(&friend_id) {
                            let _ = friend_tx.send(InternalClientMessage::NeedsFriendUpdate);
                        }

                        continue;
                    }

                    let record = sqlx::query!(
                        "SELECT accepts_friends FROM users WHERE device_id = $1 AND licence_id = $2",
                        friend_device_id as i32, friend_licence_id as i32,
                    )
                    .fetch_optional(&mut *conn)
                    .await?;

                    let response;
                    if let Some(record) = record {
                        if record.accepts_friends {
                            sqlx::query!(
                                "
                                INSERT INTO friendships (
                                    friender_device_id, friender_licence_id,
                                    friendee_device_id, friendee_licence_id
                                ) VALUES ($1, $2, $3, $4)
                            ",
                                device as i32,
                                licence as i32,
                                friend_device_id as i32,
                                friend_licence_id as i32
                            )
                            .execute(&mut *conn)
                            .await?;

                            let friend_id = ClientId::LoggedIn {
                                device: friend_device_id,
                                licence: friend_licence_id as u16,
                            };

                            let _ = self
                                .client_tx
                                .get(&client_id)
                                .unwrap()
                                .send(InternalClientMessage::NeedsFriendUpdate);

                            if let Some(tx) = self.client_tx.get(&friend_id) {
                                let _ = tx.send(InternalClientMessage::NeedsFriendUpdate);
                            }

                            response = FriendResult::Success(friend_result::Success {});
                        } else {
                            let inner = friend_result::NotAcceptingFriends {};
                            response = FriendResult::NotAcceptingFriends(inner);
                        }
                    } else {
                        response = FriendResult::NonExistant(friend_result::NonExistant {});
                    }

                    tokio::time::sleep(std::time::Duration::from_secs(1)).await;
                    tcp.write(&STCMessageOpt {
                        message: Some(STCMessage::FriendResult(stc_message::FriendResult {
                            result: Some(response),
                        })),
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
                    if self.waiting_clients.remove(&client_id).is_some() {
                        // TODO: Handle client disconnection better
                        if let Some(resp_chan) = self.client_tx.get(&client_id) {
                            let _ = resp_chan.send(InternalClientMessage::FoundMatch(inner));
                        }
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

    let server = Server::new(db_pool.clone());

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
