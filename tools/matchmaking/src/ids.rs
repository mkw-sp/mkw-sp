use netprotocol::matchmaking::{self, LoggedInId};
use std::sync::atomic::{AtomicU32, Ordering};

macro_rules! forward_fmt {
    ($impl_on:ident) => {
        impl std::fmt::Display for $impl_on {
            fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
                self.0.fmt(f)
            }
        }
    };
}

#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash, Debug)]
pub struct RoomId(pub u16);

#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash, Debug)]
pub struct GameserverID(pub u16);

forward_fmt!(RoomId);
forward_fmt!(GameserverID);

#[derive(Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash, Debug)]
pub enum ClientId {
    LoggedIn {
        device: u32,
        licence: u16,
    },
    Guest {
        id: u32,
    },
}

impl ClientId {
    pub fn new(login: Option<(u32, u16)>) -> Self {
        static GUEST_ID: AtomicU32 = AtomicU32::new(0);
        match login {
            Some((device, licence)) => Self::LoggedIn {
                device,
                licence,
            },
            None => Self::Guest {
                id: GUEST_ID.fetch_add(1, Ordering::Relaxed),
            },
        }
    }
}

impl std::fmt::Display for ClientId {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::LoggedIn {
                device,
                licence,
            } => write!(f, "LoggedIn({device}, {licence})"),
            Self::Guest {
                id,
            } => write!(f, "Guest({id})"),
        }
    }
}

impl From<matchmaking::ClientIdOpt> for ClientId {
    fn from(id: matchmaking::ClientIdOpt) -> Self {
        let id = id.inner.expect("Invalid client ID");

        match id {
            matchmaking::ClientId::LoggedIn(LoggedInId {
                device,
                licence,
            }) => Self::LoggedIn {
                device,
                licence: licence as u16,
            },
            matchmaking::ClientId::Guest(id) => Self::Guest {
                id,
            },
        }
    }
}

impl From<ClientId> for matchmaking::ClientId {
    fn from(id: ClientId) -> Self {
        match id {
            ClientId::LoggedIn {
                device,
                licence,
            } => Self::LoggedIn(matchmaking::LoggedInId {
                device,
                licence: licence as u32,
            }),
            ClientId::Guest {
                id,
            } => Self::Guest(id),
        }
    }
}
