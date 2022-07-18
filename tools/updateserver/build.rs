use std::io::Result;

fn main() -> Result<()> {
    let files = ["../../protobuf/Update.proto"];
    for file in &files {
        println!("cargo:rerun-if-changed={}", file);
    }
    prost_build::compile_protos(&files, &["../../protobuf"])?;
    Ok(())
}
