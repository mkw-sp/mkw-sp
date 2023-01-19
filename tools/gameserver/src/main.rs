mod async_stream;
mod client;
mod event;
mod request;
mod server;

use std::error::Error;

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

fn main() -> Result<(), Box<dyn Error + Send + Sync>> {
    libhydrogen::init()?;

    let server_keypair = kx::KeyPair::gen();
    eprintln!("Public key: {:02x?}", server_keypair.public_key.as_ref());

    let runtime = Runtime::new()?;
    runtime.block_on(async {
        let (tx, rx) = mpsc::channel::<Request>(32);

        let server_task = tokio::spawn(async {
            Server::new(rx).handle().await
        });

        let listener_task = tokio::spawn(async move {
            let listener = TcpListener::bind("0.0.0.0:21330").await?;
            loop {
                if let Ok((stream, _)) = listener.accept().await {
                    let server_keypair = server_keypair.clone();
                    let tx = tx.clone();
                    tokio::spawn(async move {
                        if let Ok(mut client) = Client::new(stream, server_keypair, tx).await {
                            let _ = client.handle().await;
                        }
                    });
                }
            }
        });

        tokio::select! {
            r = server_task => r,
            r = listener_task => r,
        }?
    })
}
