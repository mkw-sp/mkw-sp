use std::collections::hash_map::Entry;
use std::collections::HashMap;
use std::env;
use std::io::{ErrorKind, Read, SeekFrom, Write};
use std::path::{Path, PathBuf};

use libhydrogen::errors::anyhow;
use libhydrogen::{kx, secretbox};
use prost::Message;
use tokio::io::{AsyncReadExt, AsyncSeekExt, AsyncWriteExt};
use tokio::net::{TcpListener, TcpStream};
use tokio::runtime::Runtime;
use tokio::sync::{mpsc, oneshot};
use zeroize::Zeroizing;

include!(concat!(env!("OUT_DIR"), "/_.rs"));
use net_storage_response::Response;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        Err(anyhow!("Usage: netstorageserver <root>"))?;
    }
    let root = Path::new(&args[1]).canonicalize()?;

    libhydrogen::init()?;

    let server_keypair = match std::fs::File::open("keypair.bin") {
        Ok(mut file) => {
            let mut pk = Zeroizing::new([0u8; 32]);
            file.read_exact(&mut *pk)?;
            let mut sk = Zeroizing::new([0u8; 32]);
            file.read_exact(&mut *sk)?;
            kx::KeyPair {
                public_key: (*pk).into(),
                secret_key: (*sk).into(),
            }
        }
        Err(e) if e.kind() == ErrorKind::NotFound => {
            let server_keypair = kx::KeyPair::gen();
            let pk = server_keypair.public_key.clone();
            let sk = server_keypair.secret_key.clone();
            let pk: Zeroizing<[u8; 32]> = Zeroizing::new(pk.into());
            let sk: Zeroizing<[u8; 32]> = Zeroizing::new(sk.into());
            let mut file = std::fs::File::create("keypair.bin")?;
            file.write_all(&*pk)?;
            file.write_all(&*sk)?;
            server_keypair
        }
        Err(e) => Err(e)?,
    };
    println!("Public key: {:02x?}", server_keypair.public_key.as_ref());

    let runtime = Runtime::new()?;
    runtime.block_on(async {
        let (tx, mut rx) = mpsc::channel::<ConversionRequest>(16);
        tokio::spawn(async move {
            let mut ids = HashMap::new();
            let mut paths = vec![];
            while let Some(request) = rx.recv().await {
                match request {
                    ConversionRequest::PathToId {
                        path,
                        tx,
                    } => {
                        let id = match ids.entry(path.clone()) {
                            Entry::Occupied(occupied) => *occupied.get(),
                            Entry::Vacant(vacant) => {
                                let id = paths.len();
                                paths.push(path);
                                *vacant.insert(id)
                            }
                        };
                        let _ = tx.send(id as u32);
                    }
                    ConversionRequest::IdToPath {
                        id,
                        tx,
                    } => {
                        let path = paths.get(id as usize).map(|path| path.clone());
                        let _ = tx.send(path);
                    }
                }
            }
        });

        let listener = TcpListener::bind("0.0.0.0:21329").await?;
        loop {
            if let Ok((stream, _)) = listener.accept().await {
                let server_keypair = server_keypair.clone();
                let tx = tx.clone();
                let root = root.clone();
                tokio::spawn(async move {
                    let _ = handle(stream, server_keypair, tx, root).await;
                });
            }
        }
    })
}

async fn handle(
    stream: TcpStream,
    server_keypair: kx::KeyPair,
    tx: mpsc::Sender<ConversionRequest>,
    root: PathBuf,
) -> Result<(), Box<dyn std::error::Error>> {
    let stream = Stream::new(stream, server_keypair, tx, root).await?;
    stream.handle().await
}

struct Stream {
    stream: TcpStream,
    message_id: u64,
    context: secretbox::Context,
    tx_key: secretbox::Key,
    rx_key: secretbox::Key,
    tx: mpsc::Sender<ConversionRequest>,
    root: PathBuf,
    files: [Option<File>; 32],
    dirs: [Option<Dir>; 32],
}

impl Stream {
    async fn new(
        mut stream: TcpStream,
        server_keypair: kx::KeyPair,
        tx: mpsc::Sender<ConversionRequest>,
        root: PathBuf,
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
            message_id: 0,
            context: (*b"storage ").into(),
            rx_key,
            tx_key,
            tx,
            root,
            files: Default::default(),
            dirs: Default::default(),
        })
    }

    async fn handle(mut self) -> Result<(), Box<dyn std::error::Error>> {
        use net_storage_request::Request::*;

        loop {
            let request: NetStorageRequest = self.read_message().await?;
            let request = request.request.ok_or(anyhow!("Failed to get request type!"))?;
            match request {
                FastOpen(fast_open) => match self.id_to_path(fast_open.id).await {
                    Some(path) => self.open_file(path, "r").await?,
                    None => self.error().await?,
                },
                Open(open) => match self.convert_path(&open.path) {
                    Some(path) => self.open_file(path, &open.mode).await?,
                    None => self.error().await?,
                },
                Clone(clone) => match self.file(clone.handle).and_then(|file| file.path.as_ref()) {
                    Some(path) => {
                        let path = path.clone();
                        self.open_file(path, "r").await?
                    }
                    None => self.error().await?,
                },
                Close(close) => self.close_file(close.handle).await?,
                Read(read) => self.read_file(read.handle, read.size, read.offset).await?,
                Write(write) => self.write_file(write.handle, write.size, write.offset).await?,
                FastOpenDir(fast_open_dir) => match self.id_to_path(fast_open_dir.id).await {
                    Some(path) => self.open_dir(path).await?,
                    None => self.error().await?,
                },
                OpenDir(open_dir) => match self.convert_path(&open_dir.path) {
                    Some(path) => self.open_dir(path).await?,
                    None => self.error().await?,
                },
                CloneDir(clone_dir) => match self.dir(clone_dir.handle) {
                    Some(dir) => {
                        let path = dir.path.clone();
                        self.open_dir(path).await?
                    }
                    None => self.error().await?,
                },
                CloseDir(close_dir) => self.close_dir(close_dir.handle).await?,
                ReadDir(read_dir) => self.read_dir(read_dir.handle).await?,
                Stat(stat) => match self.convert_path(&stat.path) {
                    Some(path) => self.stat(path).await?,
                    None => self.error().await?,
                },
                StartBenchmark(_) => self.start_benchmark().await?,
            }
        }
    }

    async fn open_file(
        &mut self,
        path: PathBuf,
        mode: &str,
    ) -> Result<(), Box<dyn std::error::Error>> {
        if mode != "r" {
            return self.error().await;
        }
        let handle = match self.files.iter_mut().position(|file| file.is_none()) {
            Some(handle) => handle,
            None => return self.error().await,
        };
        let file = match tokio::fs::File::open(&path).await {
            Ok(file) => file,
            Err(_) => return self.error().await,
        };
        let size = match file.metadata().await {
            Ok(metadata) => metadata.len(),
            Err(_) => return self.error().await,
        };
        let _ = self.files[handle].insert(File {
            file,
            path: Some(path),
        });
        let handle = handle as u32;
        let response = NetStorageResponse {
            response: Some(Response::Open(net_storage_response::Open {
                handle,
                size,
            })),
        };
        self.write_message(response).await
    }

    async fn close_file(&mut self, handle: u32) -> Result<(), Box<dyn std::error::Error>> {
        let file = match self.files.get_mut(handle as usize) {
            Some(file) => file,
            None => return self.error().await,
        };
        match file.take() {
            Some(_) => self.ok().await,
            None => self.error().await,
        }
    }

    async fn read_file(
        &mut self,
        handle: u32,
        size: u32,
        offset: u64,
    ) -> Result<(), Box<dyn std::error::Error>> {
        let file = match self.file_mut(handle) {
            Some(file) => file,
            None => return self.error().await,
        };
        if file.file.seek(SeekFrom::Start(offset)).await.is_err() {
            return self.error().await;
        }
        let mut data = vec![0u8; size as usize];
        match file.file.read_exact(&mut data).await {
            Ok(_) => self.ok().await?,
            Err(_) => return self.error().await,
        }
        for chunk in data.chunks(0x1000) {
            self.write(chunk).await?;
        }
        Ok(())
    }

    async fn write_file(
        &mut self,
        handle: u32,
        mut size: u32,
        offset: u64,
    ) -> Result<(), Box<dyn std::error::Error>> {
        let mut data = vec![];
        while size > 0 {
            let chunk = self.read().await?;
            if chunk.len() != size.min(0x1000) as usize {
                return Err(anyhow!("Wrong chunk size!"))?;
            }
            size -= chunk.len() as u32;
            data.extend(chunk);
        }
        let file = match self.file_mut(handle) {
            Some(file) => file,
            None => return self.error().await,
        };
        if file.file.seek(SeekFrom::Start(offset)).await.is_err() {
            return self.error().await;
        }
        file.file.write_all(&data).await?;
        self.ok().await?;
        Ok(())
    }

    async fn open_dir(&mut self, path: PathBuf) -> Result<(), Box<dyn std::error::Error>> {
        let handle = match self.dirs.iter_mut().position(|dir| dir.is_none()) {
            Some(handle) => handle,
            None => return self.error().await,
        };
        let dir = match tokio::fs::read_dir(&path).await {
            Ok(dir) => dir,
            Err(_) => return self.error().await,
        };
        let _ = self.dirs[handle].insert(Dir {
            dir,
            path,
        });
        let handle = handle as u32;
        let response = NetStorageResponse {
            response: Some(Response::OpenDir(net_storage_response::OpenDir {
                handle,
            })),
        };
        self.write_message(response).await
    }

    async fn close_dir(&mut self, handle: u32) -> Result<(), Box<dyn std::error::Error>> {
        let dir = match self.dirs.get_mut(handle as usize) {
            Some(dir) => dir,
            None => return self.error().await,
        };
        match dir.take() {
            Some(_) => self.ok().await,
            None => self.error().await,
        }
    }

    async fn read_dir(&mut self, handle: u32) -> Result<(), Box<dyn std::error::Error>> {
        let dir = match self.dir_mut(handle) {
            Some(dir) => dir,
            None => return self.error().await,
        };
        let entry = match dir.dir.next_entry().await {
            Ok(Some(entry)) => entry,
            _ => return self.error().await,
        };
        let id = self.path_to_id(entry.path()).await;
        let metadata = entry.metadata().await;
        let name = entry.file_name().into_string();
        let (id, metadata, name) = match (id, metadata, name) {
            (Some(id), Ok(metadata), Ok(name)) => (id, metadata, name),
            _ => return self.error().await,
        };
        let r#type = if metadata.file_type().is_file() {
            net_storage_response::node_info::Type::File
        } else if metadata.file_type().is_dir() {
            net_storage_response::node_info::Type::Dir
        } else {
            return self.error().await;
        } as i32;
        let node_info = net_storage_response::NodeInfo {
            id,
            r#type,
            size: metadata.len(),
            name: name,
        };
        let response = NetStorageResponse {
            response: Some(Response::NodeInfo(node_info)),
        };
        self.write_message(response).await
    }

    async fn stat(&mut self, path: PathBuf) -> Result<(), Box<dyn std::error::Error>> {
        let id = self.path_to_id(path.clone()).await;
        let path = Path::new(&path);
        let metadata = tokio::fs::metadata(path).await;
        let name = path.file_name().and_then(|name| name.to_os_string().into_string().ok());
        let (id, metadata, name) = match (id, metadata, name) {
            (Some(id), Ok(metadata), Some(name)) => (id, metadata, name),
            _ => return self.error().await,
        };
        let r#type = if metadata.file_type().is_file() {
            net_storage_response::node_info::Type::File
        } else if metadata.file_type().is_dir() {
            net_storage_response::node_info::Type::Dir
        } else {
            return self.error().await;
        } as i32;
        let node_info = net_storage_response::NodeInfo {
            id,
            r#type,
            size: metadata.len(),
            name: name,
        };
        let response = NetStorageResponse {
            response: Some(Response::NodeInfo(node_info)),
        };
        self.write_message(response).await
    }

    async fn start_benchmark(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        let handle = match self.files.iter_mut().position(|file| file.is_none()) {
            Some(handle) => handle,
            None => return self.error().await,
        };
        let file = match tempfile::tempfile() {
            Ok(file) => file,
            Err(_) => return self.error().await,
        };
        let file = tokio::fs::File::from_std(file);
        let size = match file.metadata().await {
            Ok(metadata) => metadata.len(),
            Err(_) => return self.error().await,
        };
        let _ = self.files[handle].insert(File {
            file,
            path: None,
        });
        let handle = handle as u32;
        let response = NetStorageResponse {
            response: Some(Response::Open(net_storage_response::Open {
                handle,
                size,
            })),
        };
        self.write_message(response).await
    }

    async fn ok(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        let response = NetStorageResponse {
            response: Some(Response::Ok(net_storage_response::Ok {})),
        };
        self.write_message(response).await
    }

    async fn error(&mut self) -> Result<(), Box<dyn std::error::Error>> {
        let response = NetStorageResponse {
            response: Some(Response::Error(net_storage_response::Error {})),
        };
        self.write_message(response).await
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

    async fn path_to_id(&self, path: PathBuf) -> Option<u32> {
        let (tx, rx) = oneshot::channel();
        if self
            .tx
            .send(ConversionRequest::PathToId {
                path,
                tx,
            })
            .await
            .is_err()
        {
            return None;
        }
        rx.await.ok()
    }

    async fn id_to_path(&self, id: u32) -> Option<PathBuf> {
        let (tx, rx) = oneshot::channel();
        if self
            .tx
            .send(ConversionRequest::IdToPath {
                id,
                tx,
            })
            .await
            .is_err()
        {
            return None;
        }
        rx.await.ok().flatten()
    }

    fn convert_path(&self, path: &str) -> Option<PathBuf> {
        let path = path.strip_prefix("ro:/")?;
        let path = Path::new(&self.root).join(path).canonicalize().ok()?;
        if !path.starts_with(&self.root) {
            None
        } else {
            Some(path)
        }
    }

    fn file(&self, handle: u32) -> Option<&File> {
        self.files.get(handle as usize).map(|file| file.as_ref()).flatten()
    }

    fn file_mut(&mut self, handle: u32) -> Option<&mut File> {
        self.files.get_mut(handle as usize).map(|file| file.as_mut()).flatten()
    }

    fn dir(&self, handle: u32) -> Option<&Dir> {
        self.dirs.get(handle as usize).map(|dir| dir.as_ref()).flatten()
    }

    fn dir_mut(&mut self, handle: u32) -> Option<&mut Dir> {
        self.dirs.get_mut(handle as usize).map(|dir| dir.as_mut()).flatten()
    }
}

enum ConversionRequest {
    PathToId {
        path: PathBuf,
        tx: oneshot::Sender<u32>,
    },
    IdToPath {
        id: u32,
        tx: oneshot::Sender<Option<PathBuf>>,
    },
}

struct File {
    file: tokio::fs::File,
    path: Option<PathBuf>,
}

struct Dir {
    dir: tokio::fs::ReadDir,
    path: PathBuf,
}
