use anyhow::{anyhow, Result};
use libhydrogen::{kx, secretbox};
use tokio::net::TcpStream;
use tokio::sync::{broadcast, mpsc, oneshot};
use tokio::sync::broadcast::error::RecvError as BroadcastRecvError;

use crate::async_stream::AsyncStream;
use crate::event::Event;
use crate::request::{JoinResponse, Request};
use crate::room_protocol::*;
use crate::server::ClientKey;

/// Represents a MKW-SP client connected to the server.
#[derive(Debug)]
pub struct Client {
    /// The stream used to communicate with the client.
    stream: AsyncStream,
    /// The channel to send [`Request`]s to the server.
    tx: mpsc::Sender<Request>,
    /// The channel to recieve broadcasted [`Event`]s from the server.
    rx: broadcast::Receiver<Event>,
    client_key: ClientKey,
}

impl Client {
    pub async fn new(
        stream: TcpStream,
        server_keypair: kx::KeyPair,
        tx: mpsc::Sender<Request>,
    ) -> Result<Client> {
        let context = secretbox::Context::from(*b"room    ");
        let mut stream = AsyncStream::new(stream, server_keypair, context).await?;

        let request: RoomRequestOpt = stream.read().await?.ok_or(anyhow!("Connection closed unexpectedly!"))?;
        let request = request.request.ok_or(anyhow!("Unknown request type!"))?;
        let join = match request {
            room_request::Request::Join(join) => join,
            _ => return Err(anyhow!("Unexpected request type!"))?,
        };
        if join.miis.len() != 1 {
            return Err(anyhow!("Invalid Mii count!"))?;
        }
        if join.miis.iter().any(|mii| mii.len() != 76) {
            return Err(anyhow!("Invalid Mii size!"))?;
        }
        if join.region_line_color >= 6 {
            return Err(anyhow!("Invalid region line color!"))?;
        }
        // TODO move settings to the protocol for better checks
        if join.settings.len() != 6 {
            return Err(anyhow!("Invalid setting count!"))?;
        }

        let (join_tx, join_rx) = oneshot::channel();
        let request = Request::Join {
            inner: join,
            tx: join_tx,
        };
        tx.send(request).await?;
        let JoinResponse { rx, client_key, events } = join_rx.await?;

        for event in events {
            stream.write(event).await?;
        }

        Ok(Client {
            stream,
            tx,
            rx,
            client_key,
        })
    }

    pub async fn handle(&mut self) -> Result<()> {
        loop {
            tokio::select! {
                request = self.stream.read() => {
                    tracing::debug!("Received request: {request:?}");
                    if let Some(request) = request? {
                        self.handle_request(request).await
                    } else {
                        break Ok(())
                    }
                },
                event = self.rx.recv() => {
                    match event {
                        Ok(event) => self.handle_event(event).await,
                        Err(BroadcastRecvError::Closed) => break Ok(()),
                        Err(err) => break Err(err.into()),
                    }
                },
            }?
        }
    }

    async fn handle_event(&mut self, event: Event) -> Result<()> {
        match event {
            Event::Forward {inner} => {
                let wrapped_event = RoomEventOpt {event: Some(inner)};
                self.stream.write(wrapped_event).await?;
            },
        }

        Ok(())
    }

    async fn handle_request(&mut self, request: RoomRequestOpt) -> Result<()> {
        let request = request.request.ok_or(anyhow!("Unknown request type!"))?;

        match request {
            RoomRequest::Comment(room_request::Comment {message_id}) => {
                let event = room_event::Comment {
                    player_id: self.client_key.get() as u32,
                    message_id,
                };

                self.tx.send(Request::Comment {inner: event}).await?;
            },
            _ => anyhow::bail!("Request type not implemented!")
        }

        Ok(())
    }
}
