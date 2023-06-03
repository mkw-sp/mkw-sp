use std::collections::VecDeque;
use std::fmt;
use std::fs::{self, File};
use std::io::{self, ErrorKind, Read, Write};
use std::net::SocketAddr;
use std::time::{Duration, Instant};

use anyhow::{Context, Result};
use argon2::{Argon2, Params};
use chrono::Utc;
use libhydrogen::{kx, random, sign};
use netprotocol::update_server::*;
use tokio::fs::OpenOptions;
use tokio::io::AsyncWriteExt;
use tokio::net::{TcpListener, TcpStream};
use tokio::runtime::Runtime;
use tokio::sync::mpsc::{self, Sender};
use zeroize::Zeroizing;

fn main() -> Result<()> {
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
) -> Result<()> {
    type UpdateStream = netprotocol::async_stream::AsyncStream<
        UpdateRequest,
        UpdateResponse,
        netprotocol::NNegotiator,
    >;

    let context = (*b"update  ").into();
    let mut stream = UpdateStream::new(stream, server_keypair, context).await?;

    let request = stream.read().await?.context("Disconnected before first message")?;

    let wants_update = request.wants_update;
    let source = Version::parse(&request)?;
    let target = source.find_target();
    if !wants_update && target.is_some() {
        tx.send((address, request)).await?;
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
    stream.write(&response).await?;

    let contents = match contents {
        Some(contents) if wants_update && target != source => contents,
        _ => return Ok(()),
    };

    for chunk in contents.chunks(0x1000) {
        stream.write_raw(chunk).await?;
    }

    Ok(())
}

#[derive(Clone, Copy, Debug, PartialEq)]
struct Version {
    major: u16,
    minor: u16,
    patch: u16,
}

impl Version {
    fn parse(request: &UpdateRequest) -> Result<Version> {
        Ok(Version {
            major: request.version_major.try_into()?,
            minor: request.version_minor.try_into()?,
            patch: request.version_patch.try_into()?,
        })
    }

    fn find_target(self) -> Option<Version> {
        let map = fs::read_to_string("map.txt").ok()?;
        for line in map.lines() {
            let (source, target) = line.split_once(' ')?;
            let source: Vec<_> =
                source.splitn(3, '.').map(str::parse).collect::<Result<_, _>>().ok()?;
            let target: Vec<_> =
                target.splitn(3, '.').map(str::parse).collect::<Result<_, _>>().ok()?;
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

            if source == self {
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
