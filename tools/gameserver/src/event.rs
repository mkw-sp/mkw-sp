use crate::room_protocol::RoomEvent;

/// The core of communication between MKW-SP and this server.
#[derive(Clone, Debug)]
pub enum Event {
    Forward {
        inner: RoomEvent,
    },
}
