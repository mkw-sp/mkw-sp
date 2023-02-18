use anyhow::{Context, Result};
use libhydrogen::secretbox;
use rand::Rng;
use slab::Slab;
use tokio::net::UdpSocket;
use tokio::sync::{broadcast, mpsc};
use tokio::task::JoinHandle;

use crate::matchmaking;
use crate::room_protocol::room_event::Properties;
use crate::room_protocol::*;
use crate::unreliable_socket::{Connection, UnreliableSocket};
use crate::RoomAsyncStream;

#[derive(Debug)]
pub struct Room {
    connect_rx: mpsc::Receiver<(RoomAsyncStream, room_request::Join)>,
    disconnect_tx: mpsc::Sender<usize>,
    disconnect_rx: mpsc::Receiver<usize>,
    read_tx: mpsc::Sender<(usize, RoomRequestOpt)>,
    read_rx: mpsc::Receiver<(usize, RoomRequestOpt)>,
    write_tx: broadcast::Sender<RoomEventOpt>,
    clients: Slab<Client>,
    players: Vec<Player>,
    settings: Option<Vec<u32>>,
    matchmaking_state: Option<matchmaking::State>,
}

impl Room {
    const MAX_CLIENT_COUNT: usize = 32;
    const MAX_PLAYER_COUNT: usize = 12;

    pub fn new(
        connect_rx: mpsc::Receiver<(RoomAsyncStream, room_request::Join)>,
        matchmaking_state: Option<matchmaking::State>,
    ) -> Room {
        let (disconnect_tx, disconnect_rx) = mpsc::channel(32);
        let (read_tx, read_rx) = mpsc::channel(32);
        let (write_tx, _) = broadcast::channel(32);

        Room {
            connect_rx,
            disconnect_tx,
            disconnect_rx,
            read_tx,
            read_rx,
            write_tx,
            clients: Slab::new(),
            players: vec![],
            settings: None,
            matchmaking_state,
        }
    }

    fn client_players(&self, client_key: usize) -> impl Iterator<Item = (usize, &Player)> {
        self.players.iter().enumerate().filter(move |(_, player)| player.client_key == client_key)
    }

    fn client_players_mut(
        &mut self,
        client_key: usize,
    ) -> impl Iterator<Item = (usize, &mut Player)> {
        self.players
            .iter_mut()
            .enumerate()
            .filter(move |(_, player)| player.client_key == client_key)
    }

    fn client_player(
        &self,
        client_key: usize,
        client_player_id: usize,
    ) -> Option<(usize, &Player)> {
        self.client_players(client_key).skip(client_player_id).next()
    }

    fn start_lobby(&mut self, gamemode: u32) {
        let event = room_event::Start {
            gamemode,
        };
        let event = room_event::Event::Start(event);
        let event = RoomEventOpt {
            event: Some(event),
        };

        let _ = self.write_tx.send(event);
    }

    pub async fn handle(&mut self) -> Result<()> {
        let Some(gamemode) = self.handle_lobby().await? else { return Ok(()) };

        let Some(course_id) = self.handle_select(gamemode).await? else { return Ok(()) };

        self.handle_race().await?;

        Ok(())
    }

    async fn handle_lobby(&mut self) -> Result<Option<u32>> {
        loop {
            tokio::select! {
                Some((stream, join)) = self.connect_rx.recv() => {
                    match self.handle_lobby_connect(stream, join) {
                        Ok(Some(gamemode)) => return Ok(Some(gamemode)),
                        Err(e) => tracing::error!("Failed to join: {e}"),
                        Ok(None) => (),
                    }
                },
                Some(client_key) = self.disconnect_rx.recv() => {
                    self.handle_lobby_disconnect(client_key);
                },
                Some((client_key, request)) = self.read_rx.recv() => {
                    if self.clients.get(client_key).is_none() {
                        continue;
                    }
                    match self.handle_lobby_request(client_key, request) {
                        Ok(Some(gamemode)) => return Ok(Some(gamemode)),
                        Err(e) => {
                            tracing::error!("Failed to handle lobby request: {e}");
                            self.handle_lobby_disconnect(client_key);
                        },
                        _ => (),
                    }
                },
                else => return Ok(None),
            }
        }
    }

    async fn handle_select(&mut self, gamemode: u32) -> Result<Option<u32>> {
        loop {
            tokio::select! {
                Some((client_key, request)) = self.read_rx.recv() => {
                    if self.clients.get(client_key).is_none() {
                        continue;
                    }
                    match self.handle_select_request(client_key, request, gamemode) {
                        Ok(Some(gamemode)) => return Ok(Some(gamemode)),
                        Err(err) => tracing::error!("Failed to handle select request: {err}"),
                        Ok(None) => tracing::info!("{client_key} sent a select request"),
                    }
                },
                else => return Ok(None),
            }
        }
    }

    async fn handle_race(&mut self) -> Result<()> {
        let socket = UdpSocket::bind("0.0.0.0:21330").await?;
        let context = secretbox::Context::from(*b"race    ");
        let connections = self
            .clients
            .iter()
            .map(|(_, client)| Connection::new(client.read_key.clone(), client.write_key.clone()))
            .collect();
        let mut unreliable_socket = UnreliableSocket::new(socket, context, connections);

        let mut pending_clients = (0..self.clients.len()).collect::<Vec<_>>();
        while !pending_clients.is_empty() {
            let (index, _) = unreliable_socket.read::<RaceClientPing>().await?;
            pending_clients.retain(|i| *i != index);
        }

        let mut player_frames = vec![None; self.players.len()];
        let mut player_frames = loop {
            let Some((client_key, request)) = self.read_rx.recv().await else {return Ok(())};
            let Some(request) = request.request else { continue }; // TODO handle
            let race = match request {
                RoomRequest::Race(race) => race,
                _ => continue, // TODO handle
            };
            if race.players.len() != self.clients[client_key].player_count {
                continue; // TODO handle
            }
            tracing::debug!("{:?} {:?}", client_key, race);
            for ((player_id, _), player_frame) in
                self.client_players(client_key).zip(race.players.into_iter())
            {
                player_frames[player_id] = Some((race.time, player_frame));
            }

            let player_frames: Option<Vec<_>> = player_frames.iter().cloned().collect();
            if let Some(player_frames) = player_frames {
                break player_frames;
            }
        };

        for time in 0.. {
            let Some((client_key, request)) = self.read_rx.recv().await else {return Ok(())};
            let Some(request) = request.request else { continue }; // TODO handle
            let race = match request {
                RoomRequest::Race(race) => race,
                _ => continue, // TODO handle
            };
            if race.players.len() != self.clients[client_key].player_count {
                continue; // TODO handle
            }
            tracing::debug!("{:?} {:?}", client_key, race);
            for ((player_id, _), player_frame) in
                self.client_players(client_key).zip(race.players.into_iter())
            {
                player_frames[player_id] = (race.time, player_frame);
            }
            let player_times = player_frames.iter().map(|(time, _)| *time).collect();
            let players =
                player_frames.iter().map(|(_, player_frame)| player_frame.clone()).collect();
            let server_frame = RaceServerFrame {
                time,
                player_times,
                players,
            };
            tracing::debug!("{:?}", server_frame);
            for index in 0..self.clients.len() {
                unreliable_socket.write(index, &server_frame).await?;
            }
        }

        Ok(())
    }

    fn handle_lobby_connect(
        &mut self,
        mut stream: RoomAsyncStream,
        join: room_request::Join,
    ) -> Result<Option<u32>> {
        anyhow::ensure!(
            self.clients.len() + 1 <= Self::MAX_CLIENT_COUNT,
            "Max client count reached!",
        );
        anyhow::ensure!(
            self.players.len() + join.miis.len() <= Self::MAX_PLAYER_COUNT,
            "Max player count reached!",
        );

        let mut to_write = vec![];
        let is_host = self.settings.is_none();

        let client_id = match &mut self.matchmaking_state {
            Some(matchmaking_state) => {
                let Some(login_info) = join.login_info else {
                    anyhow::bail!("No connection token provided!");
                };

                let _ = matchmaking_state.ws_conn.send(matchmaking::Message::Update {
                    room_id: matchmaking_state.room_id,
                    client_id: login_info.client_id.clone(),
                    is_join: true,
                    is_host,
                });

                login_info.client_id
            }
            None => ClientIdOpt {
                inner: None,
            },
        };

        for player in &self.players {
            let event = room_event::Join {
                mii: player.mii.clone(),
                location: player.location,
                latitude: player.latitude,
                longitude: player.longitude,
                region_line_color: player.region_line_color,
            };
            let event = RoomEvent::Join(event);
            let event = RoomEventOpt {
                event: Some(event),
            };
            to_write.push(event);
        }
        let settings = self.settings.clone().unwrap_or(join.settings);
        let event = room_event::Settings {
            settings,
        };
        let event = RoomEvent::Settings(event);
        let event = RoomEventOpt {
            event: Some(event),
        };
        to_write.push(event);

        for mii in &join.miis {
            let event = room_event::Join {
                mii: mii.clone(),
                location: join.location,
                latitude: join.latitude,
                longitude: join.longitude,
                region_line_color: join.region_line_color,
            };
            let event = room_event::Event::Join(event);
            let event = RoomEventOpt {
                event: Some(event),
            };
            let _ = self.write_tx.send(event);
        }

        let read_key = stream.read_key().clone();
        let write_key = stream.write_key().clone();
        let client_entry = self.clients.vacant_entry();
        let client_key = client_entry.key();
        let disconnect_tx = self.disconnect_tx.clone();
        let read_tx = self.read_tx.clone();
        let mut write_rx = self.write_tx.subscribe();
        let task = tokio::spawn(async move {
            let handle_client = async move {
                for message in to_write {
                    stream.write(&message).await?;
                }
                loop {
                    tokio::select! {
                        r = write_rx.recv() => {
                            let message = match r {
                                Ok(message) => message,
                                Err(_) => break,
                            };
                            stream.write(&message).await?;
                        }
                        r = stream.read() => {
                            let message = match r? {
                                Some(message) => message,
                                None => break,
                            };
                            if read_tx.send((client_key, message)).await.is_err() {
                                break;
                            }
                        }
                    }
                }
                Ok(())
            };

            let r: Result<()> = handle_client.await;
            match r {
                Ok(()) => tracing::info!("Disconnected"),
                Err(e) => tracing::error!("Disconnected: {e}"),
            }

            let _ = disconnect_tx.send(client_key).await;
        });
        let client = Client {
            is_host,
            read_key,
            write_key,
            player_count: join.miis.len(),
            task,
            client_id,
        };
        client_entry.insert(client);

        for mii in join.miis {
            let player = Player {
                client_key,
                mii,
                properties: None,
                location: join.location,
                latitude: join.latitude,
                longitude: join.longitude,
                region_line_color: join.region_line_color,
            };
            self.players.push(player);
        }

        tracing::info!(
            "Room {}: Join: Now have {client_key} players",
            self.matchmaking_state.as_ref().map(|m| m.room_id).unwrap_or(0)
        );
        if self.matchmaking_state.is_some() && self.players.len() >= 2 {
            self.start_lobby(0);
            Ok(Some(0))
        } else {
            Ok(None)
        }
    }

    fn handle_lobby_disconnect(&mut self, client_key: usize) {
        for i in (0..self.players.len()).rev() {
            if self.players[i].client_key != client_key {
                continue;
            }

            self.players.remove(i);
            let event = room_event::Leave {
                player_id: i as u32,
            };
            let event = room_event::Event::Leave(event);
            let event = RoomEventOpt {
                event: Some(event),
            };
            let _ = self.write_tx.send(event);
        }

        let client = self.clients.remove(client_key);
        if let Some(matchmaking_state) = &self.matchmaking_state {
            let _ = matchmaking_state.ws_conn.send(matchmaking::Message::Update {
                client_id: client.client_id.clone(),
                room_id: matchmaking_state.room_id,
                is_join: false,
                is_host: client.is_host,
            });
        }
    }

    fn handle_lobby_request(
        &mut self,
        client_key: usize,
        request: RoomRequestOpt,
    ) -> Result<Option<u32>> {
        let request = request.request.context("Invalid request type!")?;

        match request {
            RoomRequest::Comment(room_request::Comment {
                message_id,
            }) => {
                anyhow::ensure!(message_id < 96, "Invalid message id!");

                let (player_id, _) =
                    self.client_player(client_key, 0).context("Is a spectator!")?;

                let event = room_event::Comment {
                    player_id: player_id as u32,
                    message_id,
                };
                let event = room_event::Event::Comment(event);
                let event = RoomEventOpt {
                    event: Some(event),
                };
                let _ = self.write_tx.send(event);

                Ok(None)
            }
            RoomRequest::Start(room_request::Start {
                gamemode,
            }) => {
                let is_host = self.clients[client_key].is_host;
                anyhow::ensure!(is_host, "Not the host!");

                // FIXME
                anyhow::ensure!(gamemode < 4, "Invalid gamemode!");

                self.start_lobby(gamemode);
                Ok(Some(gamemode))
            }
            _ => anyhow::bail!("Invalid request type!"),
        }
    }

    fn handle_select_request(
        &mut self,
        client_key: usize,
        request: RoomRequestOpt,
        gamemode: u32,
    ) -> Result<Option<u32>> {
        let request = request.request.context("Invalid request type!")?;

        match request {
            RoomRequest::Comment(_) => Ok(None),
            RoomRequest::Start(_) => Ok(None),
            RoomRequest::Vote(room_request::Vote {
                properties,
                course,
            }) => {
                for (_, player) in self.client_players(client_key) {
                    anyhow::ensure!(player.properties.is_none(), "Already selected!");
                }

                let character_weight_class = get_character_weight_class(properties.character)
                    .context("Invalid character!")?;
                let vehicle_weight_class =
                    get_vehicle_weight_class(properties.vehicle).context("Invalid vehicle!")?;
                anyhow::ensure!(character_weight_class == vehicle_weight_class);

                if gamemode == 0 {
                    anyhow::ensure!((0x00..=0x1F).contains(&course), "Invalid course!");
                } else {
                    anyhow::ensure!((0x20..=0x29).contains(&course), "Invalid stage!");
                }

                let properties = Properties {
                    character: properties.character,
                    vehicle: properties.vehicle,
                    drift_type: properties.drift_type,
                    course,
                };

                for (_, player) in self.client_players_mut(client_key) {
                    player.properties = Some(properties.clone());
                }

                for (player_id, _) in self.client_players(client_key) {
                    let event = room_event::SelectPulse {
                        player_id: player_id as u32,
                    };
                    let event = room_event::Event::SelectPulse(event);
                    let event = RoomEventOpt {
                        event: Some(event),
                    };
                    let _ = self.write_tx.send(event);
                }

                let Some(player_properties): Option<Vec<_>> = self.players.iter().map(|player| player.properties.clone()).collect() else { return Ok(None) };

                let selected_player = rand::thread_rng().gen_range(0..self.players.len());
                let course = player_properties[selected_player].course;

                let event = room_event::SelectInfo {
                    player_properties,
                    selected_player: selected_player as u32,
                };
                let event = room_event::Event::SelectInfo(event);
                let event = RoomEventOpt {
                    event: Some(event),
                };
                let _ = self.write_tx.send(event);

                Ok(Some(course))
            }
            _ => anyhow::bail!("Invalid request type!"),
        }
    }
}

/// Represents a connection to the room, is either a player (host/client) or a spectator.
#[derive(Debug)]
struct Client {
    task: JoinHandle<()>,
    read_key: secretbox::Key,
    write_key: secretbox::Key,
    player_count: usize,
    is_host: bool,
    client_id: ClientIdOpt,
}

impl Drop for Client {
    fn drop(&mut self) {
        self.task.abort();
    }
}

#[derive(Debug)]
struct Player {
    client_key: usize,
    mii: Vec<u8>,
    location: u32,
    latitude: u32,
    longitude: u32,
    region_line_color: u32,
    properties: Option<Properties>,
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
