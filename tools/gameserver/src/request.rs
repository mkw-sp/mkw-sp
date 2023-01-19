use tokio::sync::{broadcast, oneshot};

use crate::event::Event;
use crate::room_protocol::{room_request, RoomEvent};

#[derive(Debug)]
pub enum Request {
    Join {
        inner: room_request::Join,
        tx: oneshot::Sender<JoinResponse>,
    },
}

#[derive(Debug)]
pub struct JoinResponse {
    pub events: Vec<RoomEvent>,
    pub rx: broadcast::Receiver<Event>,
}
