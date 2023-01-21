use slab::Slab;
use tokio::sync::{broadcast, mpsc};

use crate::event::Event;
use crate::request::{JoinResponse, Request};
use crate::room_protocol::{room_event, RoomEvent};

#[derive(Debug)]
pub struct Server {
    rx: mpsc::Receiver<Request>,
    tx: broadcast::Sender<Event>,
    leave_tx: mpsc::Sender<usize>,
    leave_rx: mpsc::Receiver<usize>,
    clients: Slab<Client>,
    players: Vec<Player>,
    settings: Option<Vec<u32>>,
}

impl Server {
    const MAX_CLIENT_COUNT: usize = 32;
    const MAX_PLAYER_COUNT: usize = 12;

    pub fn new(rx: mpsc::Receiver<Request>) -> Server {
        let (tx, _) = broadcast::channel(32);
        let (leave_tx, leave_rx) = mpsc::channel(Self::MAX_CLIENT_COUNT);

        Server {
            rx,
            tx,
            leave_tx,
            leave_rx,
            clients: Slab::new(),
            players: vec![],
            settings: None,
        }
    }

    pub async fn handle(&mut self) {
        loop {
            tokio::select! {
                Some(request) = self.rx.recv() => self.handle_request(request),
                Some(client_key) = self.leave_rx.recv() => {
                    let client = self.handle_leave(client_key);
                    if client.is_host {
                        return;
                    }
                },
                else => return,
            }
        }
    }

    fn handle_request(&mut self, request: Request) {
        match request {
            Request::Join { inner, tx: join_tx } => {
                if self.clients.len() + 1 > Self::MAX_CLIENT_COUNT {
                    return;
                }
                if self.players.len() + inner.miis.len() > Self::MAX_PLAYER_COUNT {
                    return;
                }

                let client = Client { is_host: self.settings.is_none() };
                let client_key = self.clients.insert(client);
                let client_key = ClientKey {
                    inner: client_key,
                    leave_tx: self.leave_tx.clone(),
                };

                let mut events = vec![];

                for player in &self.players {
                    let event = room_event::Join {
                        mii: player.mii.clone(),
                        location: player.location,
                        latitude: player.latitude,
                        longitude: player.longitude,
                        region_line_color: player.region_line_color,
                    };
                    let event = room_event::Event::Join(event);
                    let event = RoomEvent { event: Some(event) };
                    events.push(event);
                }
                for mii in &inner.miis {
                    let player = Player {
                        client_key: client_key.inner,
                        mii: mii.clone(),
                        location: inner.location,
                        latitude: inner.latitude,
                        longitude: inner.longitude,
                        region_line_color: inner.region_line_color,
                    };
                    self.players.push(player);
                }

                let settings = self.settings.get_or_insert(inner.settings);
                let event = room_event::Settings {
                    settings: settings.clone(),
                };
                let event = room_event::Event::Settings(event);
                let event = RoomEvent { event: Some(event) };
                events.push(event);

                for mii in inner.miis {
                    let event = room_event::Join {
                        mii,
                        location: inner.location,
                        latitude: inner.latitude,
                        longitude: inner.longitude,
                        region_line_color: inner.region_line_color,
                    };
                    let event = room_event::Event::Join(event);
                    let event = Event::Forward { inner: event };
                    let _ = self.tx.send(event);
                }

                let rx = self.tx.subscribe();
                let response = JoinResponse { rx, client_key, events };
                let _ = join_tx.send(response);
            },
            Request::Comment {inner} => {
                self.tx.send(Event::Forward { inner: room_event::Event::Comment(inner) });
            }
        }
    }

    fn handle_leave(&mut self, client_key: usize) -> Client {
        for i in (0..self.players.len()).rev() {
            if self.players[i].client_key != client_key {
                continue;
            }

            self.players.remove(i);
            let event = room_event::Leave {
                player_id: i as u32,
            };
            let event = room_event::Event::Leave(event);
            let event = Event::Forward { inner: event };
            let _ = self.tx.send(event);
        }

        self.clients.remove(client_key)
    }
}

#[derive(Debug)]
pub struct ClientKey {
    inner: usize,
    leave_tx: mpsc::Sender<usize>,
}

impl ClientKey {
    pub fn get(&self) -> usize {
        self.inner
    }
}

impl Drop for ClientKey {
    fn drop(&mut self) {
        let _ = self.leave_tx.try_send(self.inner);
    }
}

#[derive(Debug)]
struct Client {
    is_host: bool,
}

#[derive(Debug)]
struct Player {
    client_key: usize,
    mii: Vec<u8>,
    location: u32,
    latitude: u32,
    longitude: u32,
    region_line_color: u32,
}
