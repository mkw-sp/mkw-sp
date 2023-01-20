use std::error::Error;

use libhydrogen::errors::anyhow;
use libhydrogen::{kx, secretbox};
use tokio::net::TcpStream;
use tokio::sync::{broadcast, mpsc, oneshot};

use crate::async_stream::AsyncStream;
use crate::event::Event;
use crate::request::{JoinResponse, Request};
use crate::room_protocol::{room_request, RoomRequest};
use crate::server::ClientKey;

#[derive(Debug)]
pub struct Client {
    stream: AsyncStream,
    tx: mpsc::Sender<Request>,
    rx: broadcast::Receiver<Event>,
    _client_key: ClientKey,
}

impl Client {
    pub async fn new(
        stream: TcpStream,
        server_keypair: kx::KeyPair,
        tx: mpsc::Sender<Request>,
    ) -> Result<Client, Box<dyn Error + Send + Sync>> {
        let context = secretbox::Context::from(*b"room    ");
        let mut stream = AsyncStream::new(stream, server_keypair, context).await?;

        let request: RoomRequest = stream.read().await?;
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
            _client_key: client_key,
        })
    }

    pub async fn handle(&mut self) -> Result<(), Box<dyn Error + Send + Sync>> {
        loop {
            tokio::select! {
                request = self.stream.read() => self.handle_request(request?),
                event = self.rx.recv() => self.handle_event(event?).await?,
            }
        }

        Ok(())
    }

    async fn handle_event(&mut self, event: Event) -> Result<(), Box<dyn Error + Send + Sync>> {
        match event {
            Event::Forward { inner } => {
                self.stream.write(inner).await?;
            },
        }

        Ok(())
    }

    fn handle_request(&mut self, request: RoomRequest) {

    }
}
