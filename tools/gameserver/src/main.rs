mod async_stream;
mod client;
mod event;
mod request;
mod server;

use anyhow::Result;
use libhydrogen::kx;
use tokio::net::TcpListener;
use tokio::runtime::Runtime;
use tokio::sync::mpsc;

use crate::client::Client;
use crate::request::Request;
use crate::server::Server;

mod room_protocol {
    include!(concat!(env!("OUT_DIR"), "/_.rs"));
}

fn main() -> Result<()> {
    libhydrogen::init()?;
    tracing_subscriber::fmt::init();

    let server_keypair = kx::KeyPair::gen();
    tracing::debug!("Public key: {:02x?}", server_keypair.public_key.as_ref());

    let runtime = Runtime::new()?;
    runtime.block_on(async {
        let (tx, rx) = mpsc::channel::<Request>(32);

        let server_task = tokio::spawn(async {
            Server::new(rx).handle().await;
            Ok(())
        });

        let listener_task = tokio::spawn(async move {
            let listener = TcpListener::bind("0.0.0.0:21330").await?;

            loop {
                let stream = match listener.accept().await {
                    Ok((stream, _)) => stream,
                    Err(err) => {
                        tracing::error!("Failed to accept connection: {err}");
                        continue;
                    }
                };

                let peer_addr = stream.peer_addr()?;
                tracing::info!("{peer_addr}: Accepted connection");

                let server_keypair = server_keypair.clone();
                let tx = tx.clone();
                tokio::spawn(async move {
                    let mut client = match Client::new(stream, server_keypair, tx).await {
                        Ok(client) => client,
                        Err(err) => return tracing::error!("Failed to create client: {err}"),
                    };

                    tracing::info!("{peer_addr}: Created client");
                    loop {
                        let Err(err) = client.handle().await else {continue};
                        return tracing::error!("Failed to handle client message: {err}");
                    }
                });
            }
        });

        tokio::select! {
            r = server_task => r,
            r = listener_task => r,
        }?
    })
}
