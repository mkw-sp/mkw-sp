use anyhow::{anyhow, Result};
use libhydrogen::secretbox;
use prost::Message;
use std::net::SocketAddr;
use tokio::net::UdpSocket;

#[derive(Debug)]
pub struct UnreliableSocket {
    socket: UdpSocket,
    context: secretbox::Context,
    connections: Vec<Connection>,
}

impl UnreliableSocket {
    pub fn new(
        socket: UdpSocket,
        context: secretbox::Context,
        connections: Vec<Connection>,
    ) -> UnreliableSocket {
        UnreliableSocket {
            socket,
            context,
            connections,
        }
    }

    pub fn connection_addr(&self, index: usize) -> Option<SocketAddr> {
        self.connections[index].addr
    }

    pub async fn read<M>(&mut self) -> Result<(usize, M)>
    where
        M: Message + Default,
    {
        let mut tmp = [0u8; 1024]; // TODO make that configurable
        loop {
            let (size, addr) = self.socket.recv_from(&mut tmp).await?;
            for (index, connection) in self.connections.iter_mut().enumerate() {
                let tmp = secretbox::decrypt(&tmp[0..size], 0, &self.context, &connection.read_key);
                let Ok(tmp) = tmp else {continue};
                connection.addr = Some(addr);
                let Ok(message) = M::decode(&*tmp) else {break};
                return Ok((index, message));
            }
        }
    }

    pub async fn write<M: Message>(&mut self, index: usize, message: &M) -> Result<()>
    where
        M: Message,
    {
        let connection = &self.connections[index];
        let addr = connection.addr.ok_or(anyhow!("Unknown connection address!"))?;
        let tmp = message.encode_to_vec();
        let tmp = secretbox::encrypt(&tmp, 0, &self.context, &connection.write_key);
        self.socket.send_to(&tmp, addr).await?;
        Ok(())
    }
}

#[derive(Debug)]
pub struct Connection {
    read_key: secretbox::Key,
    write_key: secretbox::Key,
    addr: Option<SocketAddr>,
}

impl Connection {
    pub fn new(read_key: secretbox::Key, write_key: secretbox::Key) -> Connection {
        Connection {
            read_key,
            write_key,
            addr: None,
        }
    }
}
