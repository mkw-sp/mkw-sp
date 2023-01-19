use std::error::Error;

use libhydrogen::{kx, secretbox};
use prost::Message;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::TcpStream;

pub struct AsyncStream {
    stream: TcpStream,
    context: secretbox::Context,
    read_key: secretbox::Key,
    write_key: secretbox::Key,
    read_message_id: u64,
    write_message_id: u64,
}

impl AsyncStream {
    pub async fn new(
        mut stream: TcpStream,
        server_keypair: kx::KeyPair,
        context: secretbox::Context,
    ) -> Result<AsyncStream, Box<dyn Error + Send + Sync>> {
        let mut state = kx::State::new();

        let mut xx1 = [0u8; kx::XX_PACKET1BYTES];
        stream.read_exact(&mut xx1).await?;
        let xx1 = kx::XXPacket1::from(xx1);

        let mut xx2 = kx::XXPacket2::new();
        kx::xx_2(&mut state, &mut xx2, &xx1, None, &server_keypair)?;
        stream.write_all(xx2.as_ref()).await?;

        let mut xx3 = [0u8; kx::XX_PACKET3BYTES];
        stream.read_exact(&mut xx3).await?;
        let xx3 = kx::XXPacket3::from(xx3);

        let keypair = kx::xx_4(&mut state, None, &xx3, None)?;
        let read_key: [u8; 32] = keypair.rx.into();
        let read_key = secretbox::Key::from(read_key);
        let write_key: [u8; 32] = keypair.tx.into();
        let write_key = secretbox::Key::from(write_key);

        Ok(AsyncStream {
            stream,
            context,
            read_key,
            write_key,
            read_message_id: 0,
            write_message_id: 0,
        })
    }

    pub async fn read<M>(&mut self) -> Result<M, Box<dyn Error + Send + Sync>>
    where
        M: Message + Default,
    {
        let mut size = [0u8; 2];
        self.stream.read_exact(&mut size).await?;
        let size = u16::from_be_bytes(size);
        let mut tmp = vec![0; size as usize];
        self.stream.read_exact(&mut tmp).await?;
        let tmp = secretbox::decrypt(&tmp, self.read_message_id, &self.context, &self.read_key)?;
        self.read_message_id += 1;
        Ok(M::decode(&*tmp)?)
    }

    pub async fn write<M: Message>(&mut self, message: M) -> Result<(), Box<dyn Error + Send + Sync>>
    where
        M: Message,
    {
        let tmp = message.encode_to_vec();
        let tmp = secretbox::encrypt(&tmp, self.write_message_id, &self.context, &self.write_key);
        self.write_message_id += 1;
        let size = tmp.len();
        assert!(size <= u16::MAX as usize);
        let size = (size as u16).to_be_bytes();
        self.stream.write_all(&size).await?;
        self.stream.write_all(&tmp).await?;
        Ok(())
    }
}
