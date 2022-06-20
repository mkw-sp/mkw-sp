use std::env;
use std::fs::{self, File};
use std::io::{self, ErrorKind, Read, Write};

use argon2::{Argon2, Params};
use libhydrogen::errors::anyhow;
use libhydrogen::{random, sign};
use zeroize::Zeroizing;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        Err(anyhow!("Usage: updateserver <input>"))?;
    }

    eprint!("Password: ");
    io::stdout().flush()?;
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
    let keypair = sign::KeyPair::gen_deterministic(&(*seed).into());
    drop(seed);
    println!("Public key: {:02x?}", keypair.public_key.as_ref());

    let contents = fs::read(&args[1])?;
    let context = (*b"update  ").into();
    let signature = sign::create(&contents, &context, &keypair.secret_key)?;
    fs::write(args[1].to_owned() + ".sig", &signature)?;

    Ok(())
}
