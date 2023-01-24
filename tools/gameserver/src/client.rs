use anyhow::{Context, Result};
use libhydrogen::{kx, secretbox};
use tokio::net::TcpStream;
use tokio::sync::broadcast::error::RecvError as BroadcastRecvError;
use tokio::sync::{broadcast, mpsc, oneshot};

use crate::event::Event;
use crate::request::{JoinResponse, Request};
use crate::room::ClientKey;
use crate::{room_protocol::*, ServerConnection};
use netprotocol::async_stream::AsyncStream;

/// Represents a MKW-SP client connected to the server.
#[derive(Debug)]
pub struct Client {
    /// The stream used to communicate with the client.
    stream: AsyncStream<RoomRequestOpt, RoomEventOpt>,
    /// The channel to send [`Request`]s to the server.
    tx: mpsc::Sender<Request>,
    /// The channel to recieve broadcasted [`Event`]s from the server.
    rx: broadcast::Receiver<Event>,
    /// The current gamemode of the room, if Start has been processed.
    gamemode: Option<u8>,

    client_key: ClientKey,
}

impl Client {
    pub async fn new(
        stream: TcpStream,
        server_keypair: kx::KeyPair,
        server_conn: ServerConnection,
    ) -> Result<Client> {
        let context = secretbox::Context::from(*b"room    ");
        let mut stream = AsyncStream::new(stream, server_keypair, context).await?;

        let request: RoomRequestOpt =
            stream.read().await?.context("Connection closed unexpectedly!")?;
        let request = request.request.context("Unknown request type!")?;
        let join = match request {
            room_request::Request::Join(join) => join,
            _ => anyhow::bail!("Unexpected request type!"),
        };

        let tx = match &join.login_info {
            Some(login_info) => {
                let ServerConnection::Matchmaking(rooms) = server_conn else {
                    anyhow::bail!("Provided connection token to private room!");
                };

                let room_id = login_info.room_id as u16;
                let conn = rooms.get(&room_id).context("Invalid connection token!")?;

                conn.clone()
            }
            None => match server_conn {
                ServerConnection::Client(tx) => tx,
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

        let (join_tx, join_rx) = oneshot::channel();
        let request = Request::Join {
            read_key: stream.read_key().clone(),
            write_key: stream.write_key().clone(),
            inner: join,
            tx: join_tx,
        };
        tx.send(request).await?;
        let JoinResponse {
            rx,
            client_key,
            events,
        } = join_rx.await?;

        for event in events {
            stream.write(&event).await?;
        }

        Ok(Client {
            stream,
            tx,
            rx,
            client_key,
            gamemode: None,
        })
    }

    pub async fn handle(&mut self) -> Result<()> {
        loop {
            tokio::select! {
                request = self.stream.read() => {
                    tracing::debug!("Received request: {request:?}");
                    if let Some(request) = request? {
                        self.handle_request(request).await?;
                    } else {
                        return Ok(());
                    }
                },
                event = self.rx.recv() => {
                    let is_ready = match event {
                        Ok(event) => self.handle_event(event).await,
                        Err(BroadcastRecvError::Closed) => return Ok(()),
                        Err(err) => return Err(err.into()),
                    }?;
                    if is_ready {
                        break;
                    }
                },
            }
        }

        loop {
            let Some(client_frame) = self.stream.read_as().await? else {return Ok(())};
            self.tx
                .send(Request::ClientFrame {
                    inner: client_frame,
                    player_id: self.client_key.get() as u32, // FIXME hack
                })
                .await?;
        }
    }

    async fn handle_event(&mut self, event: Event) -> Result<bool> {
        match event {
            Event::Forward {
                inner,
            } => {
                let wrapped_event = RoomEventOpt {
                    event: Some(inner.clone()),
                };
                self.stream.write(&wrapped_event).await?;
                if let room_event::Event::SelectInfo {
                    ..
                } = inner
                {
                    return Ok(true);
                }
            }
            Event::Start {
                gamemode,
            } => {
                self.gamemode = Some(gamemode);
                self.stream
                    .write(&RoomEventOpt {
                        event: Some(RoomEvent::Start(room_event::Start {
                            gamemode: gamemode as u32,
                        })),
                    })
                    .await?;
            }
        }

        Ok(false)
    }

    async fn handle_request(&mut self, request: RoomRequestOpt) -> Result<()> {
        let request = request.request.context("Unknown request type!")?;

        match request {
            RoomRequest::Comment(room_request::Comment {
                message_id,
            }) => {
                anyhow::ensure!(message_id < 96, "Invalid message id!");

                let event = room_event::Comment {
                    player_id: self.client_key.get() as u32,
                    message_id,
                };

                self.tx
                    .send(Request::Comment {
                        inner: event,
                    })
                    .await?;
            }
            RoomRequest::Start(room_request::Start {
                gamemode,
            }) => {
                anyhow::ensure!(gamemode < 4, "Invalid gamemode!");
                anyhow::ensure!(self.client_key.is_host(), "Not the host!");

                self.tx
                    .send(Request::Start {
                        gamemode: gamemode as u8,
                    })
                    .await?;
            }
            RoomRequest::Vote(room_request::Vote {
                course,
                properties,
            }) => {
                let gamemode = self.gamemode.context("Start not processed!")?;

                if gamemode > 0 {
                    anyhow::ensure!(!(0x20..=0x29).contains(&course), "Invalid stage!")
                } else {
                    anyhow::ensure!(course <= 0x1F, "Invalid course!")
                }

                assert_valid_character(properties.character)?;

                let character_weight_class = get_character_weight_class(properties.character);
                let vehicle_weight_class = get_vehicle_weight_class(properties.vehicle);

                anyhow::ensure!(character_weight_class == vehicle_weight_class);
                anyhow::ensure!(character_weight_class.is_some());

                let properties = room_event::Properties {
                    drift_type: properties.drift_type,
                    character: properties.character,
                    vehicle: properties.vehicle,
                    course,
                };

                self.tx
                    .send(Request::Vote {
                        properties,
                        player_id: self.client_key.get() as u32, // FIXME hack
                    })
                    .await?;
            }
            request => anyhow::bail!("Request type not implemented: {request:?}"),
        }

        Ok(())
    }
}

fn assert_valid_character(character: u32) -> Result<()> {
    anyhow::ensure!(character < 0x30);

    let unused_character_ids = [0x1C, 0x1D, 0x22, 0x23, 0x28, 0x29];
    anyhow::ensure!(!unused_character_ids.contains(&character), "Tried using Mii Outfit C");

    Ok(())
}

#[derive(Debug, PartialEq)]
enum WeightClass {
    Light,
    Medium,
    Heavy,
}

fn get_character_weight_class(character: u32) -> Option<WeightClass> {
    let small_characters = [0x6, 0xC, 0x1, 0x4, 0x8, 0xD, 0xE, 0x5, 0x18, 0x19, 0x1A, 0x1B];
    let medium_characters = [0x0, 0x7, 0x10, 0xF, 0xA, 0x11, 0x12, 0x14, 0x1e, 0x1f, 0x20, 0x21];
    let large_characters = [0xB, 0x2, 0x9, 0x3, 0x13, 0x17, 0x16, 0x15, 0x24, 0x25, 0x26, 0x27];

    if small_characters.contains(&character) {
        Some(WeightClass::Light)
    } else if medium_characters.contains(&character) {
        Some(WeightClass::Medium)
    } else if large_characters.contains(&character) {
        Some(WeightClass::Heavy)
    } else {
        None
    }
}

fn get_vehicle_weight_class(vehicles: u32) -> Option<WeightClass> {
    let small_vehicles = [0x0, 0x12, 0x3, 0x15, 0x6, 0x18, 0x9, 0x1B, 0xC, 0x1E, 0xF, 0x21];
    let medium_vehicles = [0x1, 0x13, 0x4, 0x16, 0x7, 0x19, 0xA, 0x1C, 0xD, 0x1F, 0x10, 0x22];
    let large_vehicles = [0x2, 0x14, 0x5, 0x17, 0x8, 0x1A, 0xB, 0x1D, 0xE, 0x20, 0x11, 0x23];

    if small_vehicles.contains(&vehicles) {
        Some(WeightClass::Light)
    } else if medium_vehicles.contains(&vehicles) {
        Some(WeightClass::Medium)
    } else if large_vehicles.contains(&vehicles) {
        Some(WeightClass::Heavy)
    } else {
        None
    }
}
