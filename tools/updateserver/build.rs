use std::io::Result;

fn main() -> Result<()> {
    let files = ["../../protobuf/UpdateRequest.proto", "../../protobuf/UpdateResponse.proto"];
    for file in &files {
        println!("cargo:rerun-if-changed={}", file);
    }
    prost_build::compile_protos(&files, &["../../protobuf"])?;
    Ok(())
}
