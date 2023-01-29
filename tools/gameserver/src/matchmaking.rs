pub use netprotocol::matchmaking::*;
use tokio::sync::mpsc;

#[derive(Debug)]
pub enum Message {
    Update {
        room_id: u16,
        client_id: ClientIdOpt,
        is_join: bool,
        is_host: bool,
    },
}

#[derive(Debug)]
pub struct State {
    /// Used to notify matchmaking server that a client has joined or left.
    pub ws_conn: mpsc::UnboundedSender<Message>,
    pub room_id: u16,
}
