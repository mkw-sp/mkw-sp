use anyhow::bail;
use libhydrogen::{kx, secretbox};
use netprotocol::{
    async_stream::AsyncStream,
    matchmaking::{self, CTSMessage, CTSMessageOpt},
};
use tokio::net::ToSocketAddrs;

use crate::{
    cts_message, stc_message, ClientId, Fallible, Result, STCMessage, STCMessageOpt, Server,
};

pub struct ClientForwarder {
    listener: tokio::net::TcpListener,
    db_pool: sqlx::PgPool,
    server: Server,
}

impl ClientForwarder {
    pub async fn new(
        bind: impl ToSocketAddrs,
        server: Server,
        db_pool: sqlx::PgPool,
    ) -> Result<Self> {
        Ok(Self {
            listener: tokio::net::TcpListener::bind(bind).await?,
            db_pool,
            server,
        })
    }

    pub async fn accept_loop(self) {
        let server_keypair = kx::KeyPair::gen();

        loop {
            let Ok((stream, addr)) = self.listener.accept().await else {continue};
            tracing::debug!("Accepted connection from {addr}");

            let server = self.server.clone();
            let db_pool = self.db_pool.clone();
            let server_keypair = server_keypair.clone();

            tokio::spawn(async move {
                let context = secretbox::Context::from(*b"match   ");
                let Ok(stream) = AsyncStream::new(stream, server_keypair, context).await else {
                    tracing::error!("Failed to create tcp stream");
                    return;
                };

                if let Err(err) = Self::handle_connection(stream, db_pool, server).await {
                    tracing::error!("Error in tcp connection handler: {err}")
                };
            });
        }
    }

    async fn handle_connection(
        mut stream: AsyncStream<CTSMessageOpt, STCMessageOpt>,
        db_pool: sqlx::PgPool,
        server: Server,
    ) -> Fallible {
        let client_id = Self::handle_login_flow(&mut stream, &db_pool).await?;
        server.client_listener(stream, client_id).await
    }

    /// Returns Ok(true) if the login was successful, Ok(false) if logging in as guest, Err if login failed
    async fn handle_login_flow(
        stream: &mut AsyncStream<CTSMessageOpt, STCMessageOpt>,
        db_pool: &sqlx::PgPool,
    ) -> Result<ClientId> {
        let Some(initial_message) = stream.read().await? else {bail!("No initial message")};
        let Some(CTSMessage::Login(initial_message)) = initial_message.message else {bail!("Invalid initial message")};

        tracing::debug!("Successfully fetched initial message");

        if initial_message.client_id.is_none()
            || initial_message.client_id.clone().unwrap().device == 0
        {
            tracing::info!("Guest login");
            stream
                .write(STCMessageOpt {
                    message: Some(STCMessage::GuestResponse(stc_message::LoginGuest {})),
                })
                .await?;
            return Ok(ClientId::new(None));
        }

        let client_id = initial_message.client_id.unwrap();
        tracing::info!("{client_id:?}: Verifiying device id");

        let device_id = client_id.device as i32;
        let licence_id = client_id.licence as i16;

        let challenge = Vec::new();
        stream
            .write(STCMessageOpt {
                message: Some(STCMessage::Challenge(stc_message::LoginChallenge {
                    challenge: challenge.clone(),
                })),
            })
            .await?;

        let Some(message) = stream.read().await? else {bail!("No login response")};
        let Some(CTSMessage::LoginChallengeAnswer(cts_message::LoginChallengeAnswer {challenge_signed})) = message.message else {bail!("Invalid login response")};

        if !verify_challenge_response(&challenge, &challenge_signed) {
            bail!("Challenge response verification failed");
        }

        let mut db_connection = db_pool.acquire().await?;
        let rr_record = sqlx::query!(
            "SELECT race_rating FROM users WHERE device_id = $1 AND licence_id = $2",
            device_id,
            licence_id
        )
        .fetch_one(&mut db_connection)
        .await?;

        stream
            .write(STCMessageOpt {
                message: Some(STCMessage::Response(stc_message::LoginResponse {
                    race_rating: rr_record.race_rating,
                    friends: fetch_friend_data(&mut db_connection, device_id, licence_id).await?,
                })),
            })
            .await?;

        Ok(ClientId::new(Some((device_id as u32, licence_id as u16))))
    }
}

fn verify_challenge_response(original: &[u8], signed: &[u8]) -> bool {
    tracing::warn!("Ignoring challenge response verification!");
    true
}

async fn fetch_friend_data(
    db_connection: &mut sqlx::PgConnection,
    device_id: i32,
    licence_id: i16,
) -> Result<Vec<stc_message::login_response::Friend>> {
    let fid_records = sqlx::query!(
        "SELECT friendee_device_id, friendee_licence_id FROM friendships
        WHERE friender_device_id = $1 AND friender_licence_id = $2",
        device_id,
        licence_id
    )
    .fetch_all(&mut *db_connection)
    .await?;

    let mut friends = Vec::new();
    for fid_record in fid_records {
        let friend = sqlx::query!(
            "SELECT device_id, licence_id, friend_suffix, mii, location, latitude, longitude FROM users WHERE device_id = $1 AND licence_id = $2",
            fid_record.friendee_device_id, fid_record.friendee_licence_id as i32
        ).fetch_one(&mut *db_connection).await?;

        friends.push(stc_message::login_response::Friend {
            client_id: matchmaking::LoggedInId {
                device: friend.device_id as u32,
                licence: friend.licence_id as u32,
            },
            friend_suffix: friend.friend_suffix,
            mii: friend.mii,
            location: friend.location,
            latitude: friend.latitude,
            longitude: friend.longitude,
        });
    }

    Ok(friends)
}
