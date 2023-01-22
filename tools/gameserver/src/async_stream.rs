use anyhow::Result;
use libhydrogen::{kx, secretbox};
use prost::Message;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::TcpStream;

#[derive(Debug)]
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
    ) -> Result<AsyncStream> {
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

    async fn read_exact(&mut self, buf: &mut [u8]) -> Result<bool> {
        match self.stream.read_exact(buf).await {
            Ok(_) => Ok(true),
            Err(e) => {
                if e.kind() == std::io::ErrorKind::UnexpectedEof {
                    Ok(false)
                } else {
                    Err(e.into())
                }
            }
        }
    }

    pub async fn read<M>(&mut self) -> Result<Option<M>>
    where
        M: Message + Default,
    {
        let mut size = [0u8; 2];
        if !self.read_exact(&mut size).await? {
            return Ok(None);
        };

        let size = u16::from_be_bytes(size);
        let mut msg_enc = vec![0; size as usize];
        if !self.read_exact(&mut msg_enc).await? {
            return Ok(None);
        };

        let msg =
            secretbox::decrypt(&msg_enc, self.read_message_id, &self.context, &self.read_key)?;
        self.read_message_id += 1;

        Ok(Some(M::decode(&*msg)?))
    }

    pub async fn write<M: Message>(&mut self, message: M) -> Result<()>
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
