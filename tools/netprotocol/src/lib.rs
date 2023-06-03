pub mod async_stream;

mod negotiation;
pub use negotiation::*;

mod inner {
    include!(concat!(env!("OUT_DIR"), "/_.rs"));
}

pub mod room_protocol {
    pub use super::inner::{
        client_id::Inner as ClientId, room_event, room_event::Event as RoomEvent, room_request,
        room_request::Request as RoomRequest, ClientId as ClientIdOpt, LoginInfo,
        RoomEvent as RoomEventOpt, RoomRequest as RoomRequestOpt,
    };
    pub use super::inner::{RaceClientPing, RaceServerFrame};
}

pub mod matchmaking {
    pub use super::inner::{
        client_id::Inner as ClientId, cts_message, cts_message::Message as CTSMessage, gts_message,
        gts_message::Message as GTSMessage, stc_message, stc_message::Message as STCMessage,
        stg_message, stg_message::Message as STGMessage, ClientId as ClientIdOpt,
        CtsMessage as CTSMessageOpt, GtsMessage as GTSMessageOpt, LoggedInId, LoginInfo,
        StcMessage as STCMessageOpt, StgMessage as STGMessageOpt,
    };
}

pub mod update_server {
    pub use super::inner::{UpdateRequest, UpdateResponse};
}
