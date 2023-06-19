use anyhow::Result;
use libhydrogen::kx;
use tokio::{
    io::{AsyncReadExt, AsyncWriteExt},
    net::TcpStream,
};

#[async_trait::async_trait]
pub trait KeyNegotiator: Default {
    // Performs initial negotiation with a client
    async fn negotiate(
        &self,
        stream: &mut TcpStream,
        server_keypair: kx::KeyPair,
    ) -> Result<kx::SessionKeyPair>;

    // Returns the message ID for read operations.
    fn read_message_id(&mut self) -> &mut u64;

    // Returns the message ID for write operations.
    fn write_message_id(&mut self) -> &mut u64;
}

// Requires no previous knowledge, but therefore does not verify the server is trustworthy.
#[derive(Clone, Copy, Debug, Default)]
pub struct XXNegotiator {
    read_message_id: u64,
    write_message_id: u64,
}

#[async_trait::async_trait]
impl KeyNegotiator for XXNegotiator {
    async fn negotiate(
        &self,
        stream: &mut TcpStream,
        server_keypair: kx::KeyPair,
    ) -> Result<kx::SessionKeyPair> {
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

        Ok(kx::xx_4(&mut state, None, &xx3, None)?)
    }

    fn read_message_id(&mut self) -> &mut u64 {
        &mut self.read_message_id
    }

    fn write_message_id(&mut self) -> &mut u64 {
        &mut self.write_message_id
    }
}

#[derive(Clone, Copy, Debug, Default)]
pub struct NNegotiator {
    message_id: u64,
}

#[async_trait::async_trait]
impl KeyNegotiator for NNegotiator {
    async fn negotiate(
        &self,
        stream: &mut TcpStream,
        server_keypair: kx::KeyPair,
    ) -> Result<kx::SessionKeyPair> {
        let mut n1 = [0u8; kx::NK_PACKET1BYTES];
        stream.read_exact(&mut n1).await?;
        let n1 = kx::NPacket1::from(n1);

        Ok(libhydrogen::kx::n_2(&n1, None, &server_keypair)?)
    }

    fn read_message_id(&mut self) -> &mut u64 {
        &mut self.message_id
    }

    fn write_message_id(&mut self) -> &mut u64 {
        &mut self.message_id
    }
}
