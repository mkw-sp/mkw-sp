use crate::room_protocol::room_event::Event as RoomEvent;

/// The core of communication between MKW-SP and this server.
#[derive(Clone, Debug)]
pub enum Event {
    Forward {
        inner: RoomEvent,
    },
}
