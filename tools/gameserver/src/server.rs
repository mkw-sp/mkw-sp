use std::error::Error;

use tokio::sync::{broadcast, mpsc};

use crate::event::Event;
use crate::request::{JoinResponse, Request};
use crate::room_protocol::{room_event, RoomEvent};

pub struct Server {
    rx: mpsc::Receiver<Request>,
    tx: broadcast::Sender<Event>,
    players: Vec<Player>,
    settings: Option<Vec<u32>>,
}

impl Server {
    pub fn new(rx: mpsc::Receiver<Request>) -> Server {
        let (tx, _) = broadcast::channel(32);

        Server {
            rx,
            tx,
            players: vec![],
            settings: None,
        }
    }

    pub async fn handle(&mut self) -> Result<(), Box<dyn Error + Send + Sync>> {
        while let Some(request) = self.rx.recv().await {
            match request {
                Request::Join { inner, tx: join_tx } => {
                    if self.players.len() + inner.miis.len() > 12 {
                        continue;
                    }

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
                        let event = RoomEvent { event: Some(event) };
                        let event = Event::Forward { inner: event };
                        let _ = self.tx.send(event);
                    }

                    let rx = self.tx.subscribe();
                    let response = JoinResponse { events, rx };
                    let _ = join_tx.send(response);
                },
            }
        }

        Ok(())
    }
}

struct Player {
    mii: Vec<u8>,
    location: u32,
    latitude: u32,
    longitude: u32,
    region_line_color: u32,
}
