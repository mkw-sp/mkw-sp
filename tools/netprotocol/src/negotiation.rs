use anyhow::Result;
use libhydrogen::kx;
use tokio::{
    io::{AsyncReadExt, AsyncWriteExt},
    net::TcpStream,
};

#[async_trait::async_trait]
pub trait KeyNegotiator {
    // Performs initial negotiation with a client
    async fn negotiate(
        stream: &mut TcpStream,
        server_keypair: kx::KeyPair,
    ) -> Result<kx::SessionKeyPair>;
}

// Requires no previous knowledge, but therefore does not verify the server is trustworthy.
#[derive(Debug, Clone, Copy)]
pub struct XXNegotiator;

#[async_trait::async_trait]
impl KeyNegotiator for XXNegotiator {
    async fn negotiate(
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
}

#[derive(Debug, Clone, Copy)]
pub struct NNegotiator;

#[async_trait::async_trait]
impl KeyNegotiator for NNegotiator {
    async fn negotiate(
        stream: &mut TcpStream,
        server_keypair: kx::KeyPair,
    ) -> Result<kx::SessionKeyPair> {
        let mut n1 = [0u8; kx::NK_PACKET1BYTES];
        stream.read_exact(&mut n1).await?;
        let n1 = kx::NPacket1::from(n1);

        Ok(libhydrogen::kx::n_2(&n1, None, &server_keypair)?)
    }
}
