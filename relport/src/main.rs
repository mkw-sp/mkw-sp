use std::env;
use std::fs;
use std::io;
use std::num::ParseIntError;

fn main() -> Result<(), Error> {
    let args: Vec<String> = env::args().collect();
    if args.len() != 5 {
        return Err(Error::WrongArgCount);
    }

    let source_rel = fs::read(&args[1])?;
    let target_rel = fs::read(&args[2])?;

    let (source_text_offset, mut source_text) = extract_text(source_rel)?;
    let (target_text_offset, mut target_text) = extract_text(target_rel)?;

    preprocess_text(&mut source_text);
    preprocess_text(&mut target_text);

    let mut min_size = 0x10000;
    let mut matches = vec![];
    while min_size > 0x80 {
        while let Some(best_match) = find_best_match(min_size, &source_text, &target_text, &matches) {
            matches.push(best_match);

            let sum: usize = matches.iter().map(|m| m.size).sum();
            let source_coverage = sum as f32 / source_text.len() as f32;
            let target_coverage = sum as f32 / target_text.len() as f32;
            let coverage = source_coverage.max(target_coverage);

            eprintln!("{:?}", coverage);
        }
        min_size /= 2;
    }

    matches.sort_unstable_by_key(|m| m.source_start);
    for i in 0..matches.len() - 1 {
        let matches = &mut matches[i..i + 2];
        if matches[0].source_end() <= matches[1].source_start {
            continue;
        }
        let overlap_size = matches[0].source_end() - matches[1].source_start;
        for j in 0..overlap_size {
            if source_text[matches[1].source_start + j] == 0x4e800020 {
                matches[0].size -= overlap_size - (j + 0x1);
                matches[1].source_start += j + 0x1;
                matches[1].target_start += j + 0x1;
                matches[1].size -= j + 0x1;
                break;
            }
        }
    }

    matches.sort_unstable_by_key(|m| m.target_start);
    for i in 0..matches.len() - 1 {
        let matches = &mut matches[i..i + 2];
        if matches[0].target_end() <= matches[1].target_start {
            continue;
        }
        let overlap_size = matches[0].target_end() - matches[1].target_start;
        for j in 0..overlap_size {
            if target_text[matches[1].target_start + j] == 0x4e800020 {
                matches[0].size -= overlap_size - (j + 0x1);
                matches[1].source_start += j + 0x1;
                matches[1].target_start += j + 0x1;
                matches[1].size -= j + 0x1;
                break;
            }
        }
    }

    matches.sort_unstable_by_key(|m| m.source_start);
    for m in &matches {
        let source_base = usize::from_str_radix(&args[3], 16)?;
        let target_base = usize::from_str_radix(&args[4], 16)?;

        let source_start = source_base + source_text_offset + m.source_start * 4;
        let target_start = target_base + target_text_offset + m.target_start * 4;
        let source_end = source_start + m.size * 4;
        let target_end = target_start + m.size * 4;
        println!("{:x?} {:x?} => {:x?} {:x?}", source_start, source_end, target_start, target_end);
    }

    println!();

    matches.sort_unstable_by_key(|m| m.target_start);
    for m in &matches {
        let source_base = usize::from_str_radix(&args[3], 16)?;
        let target_base = usize::from_str_radix(&args[4], 16)?;

        let source_start = source_base + source_text_offset + m.source_start * 4;
        let target_start = target_base + target_text_offset + m.target_start * 4;
        let source_end = source_start + m.size * 4;
        let target_end = target_start + m.size * 4;
        println!("{:x?} {:x?} => {:x?} {:x?}", source_start, source_end, target_start, target_end);
    }


    Ok(())
}

fn extract_text(rel: Vec<u8>) -> Result<(usize, Vec<u32>), Error> {
    let section_count = read_u32(&rel, 0xc)? as usize;
    let section_headers_offset = read_u32(&rel, 0x10)? as usize;
    if section_headers_offset % 4 != 0 {
        return Err(Error::Invalid);
    }
    for i in 0..section_count {
        let section_header_offset = section_headers_offset + i * 0x8;
        let val = read_u32(&rel, section_header_offset + 0x0)? as usize;
        if val & 0x1 == 0x1 {
            let offset = val & !0x3;
            let size = read_u32(&rel, section_header_offset + 0x4)? as usize;
            if offset % 4 != 0 || size % 4 != 0 {
                return Err(Error::Invalid);
            }
            let slice = rel.get(offset..offset + size).ok_or(Error::Oob)?;
            let text = slice
                .chunks_exact(4)
                .map(|chunk| {
                    let array = <[u8; 4]>::try_from(chunk).unwrap();
                    u32::from_be_bytes(array)
                })
                .collect();
            return Ok((offset, text));
        }
    }

    Err(Error::TextNotFound)
}

fn preprocess_text(text: &mut [u32]) {
    for inst in text {
        if *inst & 0xfc000000 == 0x48000000 {
            *inst &= 0x48000001;
        }
    }
}

#[derive(Debug)]
struct Match {
    source_start: usize,
    target_start: usize,
    size: usize,
}

impl Match {
    fn source_end(&self) -> usize {
        self.source_start + self.size
    }

    fn target_end(&self) -> usize {
        self.target_start + self.size
    }

    fn contains_source(&self, source_offset: usize) -> bool {
        self.source_start <= source_offset && self.source_start + self.size > source_offset
    }

    fn contains_target(&self, target_offset: usize) -> bool {
        self.target_start <= target_offset && self.target_start + self.size > target_offset
    }
}

fn find_best_match(min_size: usize, source_text: &[u32], target_text: &[u32], matches: &[Match]) -> Option<Match> {
    let mut best_match = None;
    for source_offset in (min_size / 2..source_text.len()).step_by(min_size) {
        if matches.iter().any(|m| m.contains_source(source_offset)) {
            continue;
        }
        let mut target_offset = 0x0;
        while target_offset < target_text.len() {
            if let Some(m) = matches.iter().find(|m| m.contains_target(target_offset)) {
                target_offset += m.size;
            }
            let mut left_size = 0x0;
            loop {
                let source_val = source_text.get(source_offset - 0x1 - left_size);
                let target_val = target_text.get(target_offset - 0x1 - left_size);
                match (source_val, target_val) {
                    (Some(source_val), Some(target_val)) if source_val == target_val => (),
                    _ => break,
                }
                left_size += 0x1;
            }
            let mut right_size = 0x0;
            loop {
                let source_val = source_text.get(source_offset + right_size);
                let target_val = target_text.get(target_offset + right_size);
                match (source_val, target_val) {
                    (Some(source_val), Some(target_val)) if source_val == target_val => (),
                    _ => break,
                }
                right_size += 0x1;
            }
            let size = left_size + right_size;
            if size >= min_size {
                match best_match {
                    Some(Match { size: best_size, .. }) if best_size > size => (),
                    _ => {
                        let source_start = source_offset - left_size;
                        let target_start = target_offset - left_size;
                        best_match = Some(Match { source_start, target_start, size })
                    },
                }
            }
            target_offset += 0x1;
        }
    }
    return best_match;
}

fn read_u32(data: &[u8], offset: usize) -> Result<u32, Error> {
    let slice = data.get(offset..offset + 4).ok_or(Error::Oob)?;
    let array = <[u8; 4]>::try_from(slice).unwrap();
    Ok(u32::from_be_bytes(array))
}

#[derive(Debug)]
enum Error {
    Invalid,
    Io(io::Error),
    Oob,
    ParseInt(ParseIntError),
    TextNotFound,
    WrongArgCount,
}

impl From<io::Error> for Error {
    fn from(e: io::Error) -> Error {
        Error::Io(e)
    }
}

impl From<ParseIntError> for Error {
    fn from(e: ParseIntError) -> Error {
        Error::ParseInt(e)
    }
}
