use std::collections::HashMap;

pub use netprotocol::matchmaking::*;
use tokio::sync::mpsc;

#[derive(Debug)]
pub enum Message {
    Update {
        room_id: u16,
        client_id: ClientIdOpt,
        is_join: bool,
    },
}

#[derive(Debug)]
pub struct State {
    /// Lookup for Slab ID to Client ID.
    pub client_lookup: HashMap<usize, ClientIdOpt>,
    /// Used to notify matchmaking server that a client has joined or left.
    pub ws_conn: mpsc::UnboundedSender<Message>,
    pub room_id: u16,
}
