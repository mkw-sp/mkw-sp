use anyhow::bail;
use futures_util::StreamExt;
use prost::Message;
use tokio::net::{TcpStream, ToSocketAddrs};
use tokio_tungstenite::WebSocketStream;

use crate::{Fallible, GTSMessage, GTSMessageOpt, Gameserver, GameserverID, Result, Server};

pub struct WebsocketListener {
    listener: tokio::net::TcpListener,
    server: Server,
}

impl WebsocketListener {
    pub async fn new(bind: impl ToSocketAddrs, server: Server) -> Result<Self> {
        let listener = tokio::net::TcpListener::bind(bind).await?;
        Ok(Self {
            listener,
            server,
        })
    }

    pub async fn accept_loop(self) {
        loop {
            let Ok((stream, addr)) = self.listener.accept().await else {continue};
            tracing::debug!("Accepted connection from {addr}");

            let server = self.server.clone();

            tokio::spawn(async move {
                let ws_stream = tokio_tungstenite::accept_async(stream).await.unwrap();
                if let Err(err) = Self::handle_connection(ws_stream, server).await {
                    tracing::error!("Error in websocket connection handler: {err}")
                };
            });
        }
    }

    async fn handle_connection(mut ws: WebSocketStream<TcpStream>, server: Server) -> Fallible {
        let Some(initial_message) = ws.next().await.transpose()? else {bail!("No initial message")};
        let initial_message = GTSMessageOpt::decode(&*initial_message.into_data())?;
        let Some(GTSMessage::AddServer (initial_message)) = initial_message.message else {bail!("Wrong initial message sent")};

        let id = GameserverID(initial_message.gameserver_id.try_into()?);
        let (sender, receiver) = tokio::sync::mpsc::unbounded_channel();

        tracing::info!("New gameserver connected: {id}");
        server.gameservers.insert(id, Gameserver::new(sender, initial_message.max_rooms as usize));
        let res = server.gameserver_listener(ws, receiver, id).await;
        server.gameservers.remove(&id);
        res
    }
}
