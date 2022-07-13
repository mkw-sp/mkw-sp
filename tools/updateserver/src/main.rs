use std::collections::VecDeque;
use std::fmt;
use std::fs::{self, File};
use std::io::{self, ErrorKind, Read, Write};
use std::net::SocketAddr;
use std::time::{Duration, Instant};

use argon2::{Argon2, Params};
use chrono::Utc;
use libhydrogen::{kx, random, secretbox, sign};
use prost::Message;
use tokio::fs::OpenOptions;
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::{TcpListener, TcpStream};
use tokio::runtime::Runtime;
use tokio::sync::mpsc::{self, Sender};
use zeroize::Zeroizing;

include!(concat!(env!("OUT_DIR"), "/_.rs"));

fn main() -> Result<(), Box<dyn std::error::Error>> {
    eprint!("Password: ");
    io::stderr().flush()?;
    let password = Zeroizing::new(passterm::read_password()?);
    eprintln!("[hidden]");

    libhydrogen::init()?;

    let salt = match File::open("salt.bin") {
        Ok(mut file) => {
            let mut salt = Zeroizing::new([0u8; 32]);
            file.read_exact(&mut *salt)?;
            salt
        }
        Err(e) if e.kind() == ErrorKind::NotFound => {
            eprintln!("No salt found, generating a new one.");
            let mut salt = Zeroizing::new([0u8; 32]);
            random::buf_into(&mut *salt);
            let mut file = File::create("salt.bin")?;
            file.write_all(&*salt)?;
            salt
        }
        Err(e) => Err(e)?,
    };
    let mut seed = Zeroizing::new([0u8; 32]);
    let params = Params::new(131072, 16, 8, None)?;
    let argon2 = Argon2::new(Default::default(), Default::default(), params);
    argon2.hash_password_into(password.as_bytes(), &*salt, &mut *seed)?;
    drop(password);
    drop(salt);
    let server_keypair = kx::KeyPair::gen_deterministic(&(*seed).into());
    drop(seed);
    println!("Public key: {:02x?}", server_keypair.public_key.as_ref());

    let runtime = Runtime::new()?;
    runtime.block_on(async {
        let mut file = OpenOptions::new().append(true).create(true).open("log.txt").await?;
        let (tx, mut rx) = mpsc::channel::<(SocketAddr, UpdateRequest)>(16);
        tokio::spawn(async move {
            let mut queue = VecDeque::new();
            while let Some((address, request)) = rx.recv().await {
                let now = Instant::now();
                while let Some((then, _)) = queue.front() {
                    if now - *then >= Duration::from_secs(24 * 60 * 60) {
                        queue.pop_front();
                    } else {
                        break;
                    }
                }
                if queue.iter().any(|(_, a)| *a == address.ip()) {
                    continue;
                }
                queue.push_back((now, address.ip()));

                let now = Utc::now().format("%F %T");
                let major = request.version_major;
                let minor = request.version_minor;
                let patch = request.version_patch;
                let version = format!("{}.{}.{}", major, minor, patch);
                let gn = request.game_name.escape_debug();
                let hp = request.host_platform.escape_debug();
                let line = format!("{} {} {} {} {}\n", now, address, version, gn, hp);
                let _ = file.write_all(line.as_bytes()).await;
                let _ = file.sync_all().await;
            }
        });

        let listener = TcpListener::bind("0.0.0.0:21328").await?;
        loop {
            if let Ok((stream, address)) = listener.accept().await {
                let server_keypair = server_keypair.clone();
                let tx = tx.clone();
                tokio::spawn(async move {
                    let _ = handle(stream, address, server_keypair, tx).await;
                });
            }
        }
    })
}

async fn handle(
    stream: TcpStream,
    address: SocketAddr,
    server_keypair: kx::KeyPair,
    tx: Sender<(SocketAddr, UpdateRequest)>,
) -> Result<(), Box<dyn std::error::Error>> {
    let stream = Stream::new(stream, address, server_keypair, tx).await?;
    stream.handle().await
}

struct Stream {
    stream: TcpStream,
    address: SocketAddr,
    message_id: u64,
    context: secretbox::Context,
    tx_key: secretbox::Key,
    rx_key: secretbox::Key,
    tx: Sender<(SocketAddr, UpdateRequest)>,
}

impl Stream {
    async fn new(
        mut stream: TcpStream,
        address: SocketAddr,
        server_keypair: kx::KeyPair,
        tx: Sender<(SocketAddr, UpdateRequest)>,
    ) -> Result<Stream, Box<dyn std::error::Error>> {
        stream.set_nodelay(true)?;

        let mut tmp = Zeroizing::new([0u8; 48]);
        stream.read_exact(&mut *tmp).await?;
        let keypair = libhydrogen::kx::n_2(&(*tmp).into(), None, &server_keypair)?;
        drop(tmp);
        let rx_key: Zeroizing<[u8; 32]> = Zeroizing::new(keypair.rx.clone().into());
        let rx_key = (*rx_key).into();
        let tx_key: Zeroizing<[u8; 32]> = Zeroizing::new(keypair.tx.clone().into());
        let tx_key = (*tx_key).into();

        Ok(Stream {
            stream,
            address,
            message_id: 0,
            context: (*b"update  ").into(),
            rx_key,
            tx_key,
            tx,
        })
    }

    async fn handle(mut self) -> Result<(), Box<dyn std::error::Error>> {
        let request: UpdateRequest = self.read_message().await?;
        let wants_update = request.wants_update;
        let source = Version::parse(&request)?;
        let target = source.find_target();
        if !wants_update && target.is_some() {
            self.tx.send((self.address, request)).await?;
        }
        let mut target = target.unwrap_or(source);
        let contents = format!("updates/{}/contents.arc", target);
        let signature = contents.clone() + ".sig";
        let contents = fs::read(contents).ok();
        let mut size = contents.as_ref().and_then(|contents| contents.len().try_into().ok());
        let signature = fs::read(signature).ok();
        let mut signature = signature.filter(|signature| signature.len() == sign::BYTES);

        if size.is_none() || signature.is_none() {
            target = source;
            size = None;
            signature = None;
        }
        let response = UpdateResponse {
            version_major: target.major as u32,
            version_minor: target.minor as u32,
            version_patch: target.patch as u32,
            size: size.unwrap_or(0),
            signature: signature.unwrap_or(vec![]),
        };
        self.write_message(response).await?;

        let contents = match contents {
            Some(contents) if wants_update && target != source => contents,
            _ => return Ok(()),
        };
        for chunk in contents.chunks(0x1000) {
            self.write(chunk).await?;
        }

        Ok(())
    }

    async fn read(&mut self) -> Result<Vec<u8>, Box<dyn std::error::Error>> {
        let mut size = [0u8; 2];
        self.stream.read_exact(&mut size).await?;
        let size = u16::from_be_bytes(size);
        let mut tmp = vec![0; size as usize];
        self.stream.read_exact(&mut tmp).await?;
        let tmp = secretbox::decrypt(&tmp, self.message_id, &self.context, &self.rx_key)?;
        self.message_id += 1;
        Ok(tmp)
    }

    async fn read_message<M>(&mut self) -> Result<M, Box<dyn std::error::Error>>
    where
        M: Message + Default,
    {
        self.read().await.and_then(|tmp| Ok(M::decode(&*tmp)?))
    }

    async fn write(&mut self, tmp: &[u8]) -> Result<(), Box<dyn std::error::Error>> {
        let tmp = secretbox::encrypt(&tmp, self.message_id, &self.context, &self.tx_key);
        self.message_id += 1;
        let size = tmp.len();
        assert!(size <= u16::MAX as usize);
        let size = (size as u16).to_be_bytes();
        self.stream.write_all(&size).await?;
        self.stream.write_all(&tmp).await?;
        Ok(())
    }

    async fn write_message<M: Message>(
        &mut self,
        message: M,
    ) -> Result<(), Box<dyn std::error::Error>>
    where
        M: Message,
    {
        self.write(&message.encode_to_vec()).await
    }
}

#[derive(Clone, Copy, Debug, PartialEq)]
struct Version {
    major: u16,
    minor: u16,
    patch: u16,
}

impl Version {
    fn parse(request: &UpdateRequest) -> Result<Version, Box<dyn std::error::Error>> {
        Ok(Version {
            major: request.version_major.try_into()?,
            minor: request.version_minor.try_into()?,
            patch: request.version_patch.try_into()?,
        })
    }

    fn find_target(&self) -> Option<Version> {
        let map = fs::read_to_string("map.txt").ok()?;
        for line in map.lines() {
            let (source, target) = line.split_once(' ')?;
            let source: Vec<_> = source
                .splitn(3, '.')
                .map(|part| u16::from_str_radix(part, 10))
                .collect::<Result<_, _>>()
                .ok()?;
            let target: Vec<_> = target
                .splitn(3, '.')
                .map(|part| u16::from_str_radix(part, 10))
                .collect::<Result<_, _>>()
                .ok()?;
            let source = Version {
                major: source[0],
                minor: source[1],
                patch: source[2],
            };
            let target = Version {
                major: target[0],
                minor: target[1],
                patch: target[2],
            };
            if source == *self {
                return Some(target);
            }
        }

        None
    }
}

impl fmt::Display for Version {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}.{}.{}", self.major, self.minor, self.patch)
    }
}
