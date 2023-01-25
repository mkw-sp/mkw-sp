pub mod async_stream;

mod inner {
    include!(concat!(env!("OUT_DIR"), "/_.rs"));
}

pub mod room_protocol {
    pub use super::inner::{
        LoginInfo, ClientId as ClientIdOpt, client_id::Inner as ClientId,
        room_event, room_event::Event as RoomEvent, room_request,
        room_request::Request as RoomRequest, RoomEvent as RoomEventOpt,
        RoomRequest as RoomRequestOpt,
    };
}

pub mod matchmaking {
    pub use super::inner::{
        LoginInfo,
        LoggedInId,
        ClientId as ClientIdOpt,
        client_id::Inner as ClientId,

        gts_message,
        GtsMessage as GTSMessageOpt,
        gts_message::Message as GTSMessage,
        stg_message,
        StgMessage as STGMessageOpt,
        stg_message::Message as STGMessage,
        cts_message,
        CtsMessage as CTSMessageOpt,
        cts_message::Message as CTSMessage,
        stc_message,
        StcMessage as STCMessageOpt,
        stc_message::Message as STCMessage,
    };
}
