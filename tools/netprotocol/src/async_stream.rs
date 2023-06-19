use std::marker::PhantomData;
use std::slice::SliceIndex;

use crate::negotiation::*;
use anyhow::Result;
use libhydrogen::{kx, secretbox};
use prost::Message;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::TcpStream;

const MAX_MESSAGE_SIZE: usize = 1024;

#[derive(Debug)]
pub struct AsyncStream<R: Message + Default, W: Message, N: KeyNegotiator> {
    stream: TcpStream,
    context: secretbox::Context,
    read_key: secretbox::Key,
    read_buffer: [u8; MAX_MESSAGE_SIZE],
    read_offset: usize,
    write_key: secretbox::Key,
    negotiator: N,
    _marker: PhantomData<(R, W)>,
}

impl<R: Message + Default, W: Message, N: KeyNegotiator> AsyncStream<R, W, N> {
    pub async fn new(
        mut stream: TcpStream,
        server_keypair: kx::KeyPair,
        context: secretbox::Context,
    ) -> Result<Self> {
        let negotiator = N::default();
        let keypair = negotiator.negotiate(&mut stream, server_keypair).await?;

        let read_key: [u8; 32] = keypair.rx.into();
        let read_key = secretbox::Key::from(read_key);
        let write_key: [u8; 32] = keypair.tx.into();
        let write_key = secretbox::Key::from(write_key);

        Ok(AsyncStream {
            stream,
            context,
            read_key,
            read_buffer: [0; MAX_MESSAGE_SIZE],
            read_offset: 0,
            write_key,
            negotiator,
            _marker: PhantomData,
        })
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

        let read_id = self.negotiator.read_message_id();
        let message = secretbox::decrypt(
            &self.read_buffer[2..size],
            *read_id,
            &self.context,
            &self.read_key,
        )?;
        *read_id += 1;
        let message = R::decode(&*message)?;
        self.read_offset = 0;
        Ok(Some(message))
    }

    pub async fn write_raw(&mut self, data: &[u8]) -> Result<()> {
        let write_id = self.negotiator.write_message_id();
        let message = secretbox::encrypt(
            data,
            *write_id,
            &self.context,
            &self.write_key,
        );
        *write_id += 1;
        let size = message.len();
        assert!(size <= u16::MAX as usize);
        let size = (size as u16).to_be_bytes();
        self.stream.write_all(&size).await?;
        self.stream.write_all(&message).await?;
        Ok(())
    }

    pub async fn write(&mut self, message: &W) -> Result<()> {
        self.write_raw(&message.encode_to_vec()).await
    }
}
