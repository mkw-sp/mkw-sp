use crate::room_protocol::RoomEvent;

#[derive(Clone, Debug)]
pub enum Event {
    Forward {
        inner: RoomEvent,
    },
}
