use std::marker::PhantomData;
use std::slice::SliceIndex;

use anyhow::Result;
use libhydrogen::{kx, secretbox};
use prost::Message;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::TcpStream;

const MAX_MESSAGE_SIZE: usize = 1024;

#[derive(Debug)]
pub struct AsyncStream<R: Message + Default, W: Message> {
    stream: TcpStream,
    public_key: kx::PublicKey,
    context: secretbox::Context,
    read_key: secretbox::Key,
    read_message_id: u64,
    read_buffer: [u8; MAX_MESSAGE_SIZE],
    read_offset: usize,
    write_key: secretbox::Key,
    write_message_id: u64,
    _marker: PhantomData<(R, W)>,
}

impl<R: Message + Default, W: Message> AsyncStream<R, W> {
    pub async fn new(
        mut stream: TcpStream,
        server_keypair: kx::KeyPair,
        context: secretbox::Context,
    ) -> Result<Self> {
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

        let mut public_key = kx::PublicKey::from([0u8; kx::PUBLICKEYBYTES]);
        let keypair = kx::xx_4(&mut state, Some(&mut public_key), &xx3, None)?;
        let read_key: [u8; 32] = keypair.rx.into();
        let read_key = secretbox::Key::from(read_key);
        let write_key: [u8; 32] = keypair.tx.into();
        let write_key = secretbox::Key::from(write_key);

        Ok(AsyncStream {
            stream,
            public_key,
            context,
            read_key,
            read_message_id: 0,
            read_buffer: [0; MAX_MESSAGE_SIZE],
            read_offset: 0,
            write_key,
            write_message_id: 0,
            _marker: PhantomData,
        })
    }

    pub fn public_key(&self) -> &kx::PublicKey {
        &self.public_key
    }

    pub fn read_key(&self) -> &secretbox::Key {
        &self.read_key
    }

    pub fn write_key(&self) -> &secretbox::Key {
        &self.write_key
    }

    async fn read_internal<I>(&mut self, index: I) -> Result<bool>
    where
        I: SliceIndex<[u8], Output = [u8]>,
    {
        match self.stream.read(&mut self.read_buffer[index]).await {
            Ok(0) => Ok(false),
            Ok(size) => {
                self.read_offset += size;
                Ok(true)
            }
            Err(e) => Err(e.into()),
        }
    }

    pub async fn read(&mut self) -> Result<Option<R>> {
        while self.read_offset < 2 {
            if !self.read_internal(self.read_offset..2).await? {
                anyhow::ensure!(self.read_offset == 0, "Unexpected eof!");
                return Ok(None);
            }
        }
        let size = u16::from_be_bytes(<[u8; 2]>::try_from(&self.read_buffer[..2]).unwrap());
        let size = size as usize + 2;
        anyhow::ensure!(size <= MAX_MESSAGE_SIZE, "Invalid message size!");
        while self.read_offset < size {
            if !self.read_internal(2..size).await? {
                anyhow::ensure!(self.read_offset == 0, "Unexpected eof!");
            }
        }
        let message = secretbox::decrypt(
            &self.read_buffer[2..size],
            self.read_message_id,
            &self.context,
            &self.read_key,
        )?;
        self.read_message_id += 1;
        let message = R::decode(&*message)?;
        self.read_offset = 0;
        Ok(Some(message))
    }

    pub async fn write(&mut self, message: &W) -> Result<()> {
        let message = message.encode_to_vec();
        let message =
            secretbox::encrypt(&message, self.write_message_id, &self.context, &self.write_key);
        self.write_message_id += 1;
        let size = message.len();
        assert!(size <= u16::MAX as usize);
        let size = (size as u16).to_be_bytes();
        self.stream.write_all(&size).await?;
        self.stream.write_all(&message).await?;
        Ok(())
    }
}
