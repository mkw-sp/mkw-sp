use std::fmt;
use std::fs::{self, File};
use std::io::{self, ErrorKind, Read, Write};

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
            file.write(&*salt)?;
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
        let (tx, mut rx) = mpsc::channel::<Version>(16);
        tokio::spawn(async move {
            while let Some(version) = rx.recv().await {
                let now = Utc::now();
                let line = format!("{} {}\n", now.format("%F %T"), version);
                let _ = file.write_all(line.as_bytes()).await;
                let _ = file.sync_all().await;
            }
        });

        let listener = TcpListener::bind("127.0.0.1:21328").await?;
        loop {
            if let Ok((stream, _)) = listener.accept().await {
                let server_keypair = server_keypair.clone();
                let tx = tx.clone();
                tokio::spawn(async {
                    let _ = handle(server_keypair, stream, tx).await;
                });
            }
        }
    })
}

async fn handle(
    server_keypair: kx::KeyPair,
    mut stream: TcpStream,
    tx: Sender<Version>,
) -> Result<(), Box<dyn std::error::Error>> {
    stream.set_nodelay(true)?;

    let mut tmp = Zeroizing::new([0u8; 48]);
    stream.read_exact(&mut *tmp).await?;
    let keypair = libhydrogen::kx::n_2(&(*tmp).into(), None, &server_keypair)?;
    drop(tmp);
    let mut message_id = 0;
    let context = (*b"update  ").into();

    let message: UpdateMessage = read(&mut stream, &mut message_id, &context, &keypair).await?;
    let wants_update = message.wants_update;
    let source = Version::parse(&message)?;
    let target = source.find_target();
    if !wants_update && target.is_some() {
        tx.send(source).await?;
    }
    let mut target = target.unwrap_or(source);
    let contents = format!("updates/{}/contents.arc", target);
    let signature = contents.clone() + ".sig";
    let contents = fs::read(contents).ok();
    let signature = fs::read(signature).ok();

    let mut message = [0u8; 78];
    if let (Some(contents), Some(signature)) = (&contents, &signature) {
        if target != source && contents.len() <= 0x400000 && signature.len() == sign::BYTES {
            write_u32(&mut message, 6, contents.len() as u32);
            message[14..].clone_from_slice(&signature);
        } else {
            target = source;
        }
    } else {
        target = source;
    }
    write_u16(&mut message, 0, target.major);
    write_u16(&mut message, 2, target.minor);
    write_u16(&mut message, 4, target.patch);
    write(&mut stream, &mut message_id, &context, &keypair, &message).await?;

    let contents = match contents {
        Some(contents) if wants_update && target != source => contents,
        _ => return Ok(()),
    };
    for chunk in contents.chunks(0x1000) {
        write(&mut stream, &mut message_id, &context, &keypair, chunk).await?;
    }

    Ok(())
}

async fn read<M: Message + Default>(
    stream: &mut TcpStream,
    message_id: &mut u64,
    context: &secretbox::Context,
    keypair: &kx::SessionKeyPair,
) -> Result<M, Box<dyn std::error::Error>> {
    let mut size = [0u8; 2];
    stream.read_exact(&mut size).await?;
    let size = u16::from_be_bytes(size);
    let mut tmp = Vec::with_capacity(size as usize);
    stream.read_exact(&mut tmp).await?;
    let key: Zeroizing<[u8; 32]> = Zeroizing::new(keypair.rx.clone().into());
    let key = (*key).into();
    let tmp = secretbox::decrypt(&tmp, *message_id, context, &key)?;
    *message_id += 1;
    Ok(M::decode(&*tmp)?)
}

async fn write(
    stream: &mut TcpStream,
    message_id: &mut u64,
    context: &secretbox::Context,
    keypair: &kx::SessionKeyPair,
    message: &[u8],
) -> Result<(), Box<dyn std::error::Error>> {
    let key: Zeroizing<[u8; 32]> = Zeroizing::new(keypair.tx.clone().into());
    let key = (*key).into();
    let tmp = secretbox::encrypt(message, *message_id, context, &key);
    *message_id += 1;
    stream.write_all(&tmp).await?;
    Ok(())
}

/*async fn write<M: Message>(
    stream: &mut TcpStream,
    message_id: &mut u64,
    context: &secretbox::Context,
    keypair: &kx::SessionKeyPair,
    message: M,
) -> Result<(), Box<dyn std::error::Error>> {
    let tmp = message.encode_to_vec();
    let key: Zeroizing<[u8; 32]> = Zeroizing::new(keypair.tx.clone().into());
    let key = (*key).into();
    let tmp = secretbox::encrypt(&tmp, *message_id, context, &key);
    *message_id += 1;
    let size = tmp.len();
    assert!(size <= u16::MAX as usize);
    let size = (size as u16).to_be_bytes();
    stream.write_all(&size).await?;
    stream.write_all(&tmp).await?;
    Ok(())
}*/

fn write_u16(data: &mut [u8], offset: usize, val: u16) {
    let array = val.to_be_bytes();
    data[offset..offset + 2].clone_from_slice(&array);
}

fn write_u32(data: &mut [u8], offset: usize, val: u32) {
    let array = val.to_be_bytes();
    data[offset..offset + 4].clone_from_slice(&array);
}

#[derive(Clone, Copy, Debug, PartialEq)]
struct Version {
    major: u16,
    minor: u16,
    patch: u16,
}

impl Version {
    fn parse(message: &UpdateMessage) -> Result<Version, Box<dyn std::error::Error>> {
        Ok(Version {
            major: message.version_major.try_into()?,
            minor: message.version_minor.try_into()?,
            patch: message.version_patch.try_into()?,
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
