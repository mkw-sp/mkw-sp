use std::env;
use std::fs;
use std::io;
use std::mem;
use std::num::ParseIntError;
use std::path::Path;

fn main() -> Result<(), Error> {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 && args.len() != 5 {
        return Err(Error::WrongArgCount);
    }

    let source_extension = Path::new(&args[1]).extension();
    let target_extension = Path::new(&args[1]).extension();
    if source_extension != target_extension {
        return Err(Error::MismatchedExtensions);
    }
    let extension = source_extension
        .and_then(|source_extension| source_extension.to_str())
        .ok_or(Error::ExtensionNotFound)?;

    match (extension, args.len()) {
        ("dol", 3) => (),
        ("rel", 5) => (),
        _ => return Err(Error::WrongArgCount),
    }

    let source = fs::read(&args[1])?;
    let target = fs::read(&args[2])?;

    let (source_sections, target_sections) = match extension {
        "dol" => (parse_dol(source)?, parse_dol(target)?),
        "rel" => {
            let source_base = usize::from_str_radix(&args[3], 16)?;
            let target_base = usize::from_str_radix(&args[4], 16)?;
            (parse_rel(source, source_base)?, parse_rel(target, target_base)?)
        },
        _ => return Err(Error::InvalidExtension),
    };
    if source_sections.len() != target_sections.len() {
        return Err(Error::MismatchedSections);
    }
    for (source_section, target_section) in source_sections.iter().zip(target_sections.iter()) {
        if source_section.kind != target_section.kind {
            return Err(Error::MismatchedSections);
        }
    }

    for (source_section, target_section) in source_sections.iter().zip(target_sections.iter()) {
        if source_section.kind != SectionKind::Text {
            continue;
        }

        let mut matches = process(source_section, target_section);
        postprocess(source_section, &mut matches);
        matches.iter_mut().for_each(|m| mem::swap(&mut m.source_start, &mut m.target_start));
        postprocess(target_section, &mut matches);
        matches.iter_mut().for_each(|m| mem::swap(&mut m.source_start, &mut m.target_start));
        matches.sort_unstable_by_key(|m| m.source_start);
        dump(source_section, target_section, &matches);
        println!();
        matches.sort_unstable_by_key(|m| m.target_start);
        dump(source_section, target_section, &matches);
        println!();
    }

    Ok(())
}

#[derive(Debug, PartialEq)]
enum SectionKind {
    Text,
    Data,
    Bss,
}

#[derive(Debug)]
struct Section {
    address: usize,
    kind: SectionKind,
    vals: Vec<u32>,
}

fn parse_dol(dol: Vec<u8>) -> Result<Vec<Section>, Error> {
    let mut sections = Vec::with_capacity(18);
    for i in 0..18 {
        let offset = read_u32(&dol, 0x0 + i * 0x4)? as usize;
        let address = read_u32(&dol, 0x48 + i * 0x4)? as usize;
        let size = read_u32(&dol, 0x90 + i * 0x4)? as usize;
        if offset == 0 || address == 0 || size == 0 {
            continue;
        }
        if offset % 4 != 0 || address % 4 != 0 || size % 4 != 0 {
            return Err(Error::Invalid);
        }
        let kind = if i < 7 { SectionKind::Text } else { SectionKind::Data };
        let slice = dol.get(offset..offset + size).ok_or(Error::Oob)?;
        let mut vals: Vec<u32> = slice
            .chunks_exact(4)
            .map(|chunk| {
                let array = <[u8; 4]>::try_from(chunk).unwrap();
                u32::from_be_bytes(array)
            })
            .collect();
        if kind == SectionKind::Text {
            for inst in &mut vals {
                match *inst & 0xfc000000 {
                    0x38000000 => *inst &= 0xffff0000, // addi
                    0x30000000 => *inst &= 0xffff0000, // addic
                    0x34000000 => *inst &= 0xffff0000, // addic.
                    0x3c000000 => *inst &= 0xffff0000, // addis
                    0x48000000 => *inst &= 0xfc000001, // b, bl
                    0x88000000 => *inst &= 0xffff0000, // lbz
                    0x8c000000 => *inst &= 0xffff0000, // lbzu
                    0xc8000000 => *inst &= 0xffff0000, // lfd
                    0xcc000000 => *inst &= 0xffff0000, // lfdu
                    0xc0000000 => *inst &= 0xffff0000, // lfs
                    0xc4000000 => *inst &= 0xffff0000, // lfsu
                    0xa8000000 => *inst &= 0xffff0000, // lha
                    0xac000000 => *inst &= 0xffff0000, // lhau
                    0xa0000000 => *inst &= 0xffff0000, // lhz
                    0xa4000000 => *inst &= 0xffff0000, // lhzu
                    0x80000000 => *inst &= 0xffff0000, // lwz
                    0x84000000 => *inst &= 0xffff0000, // lwzu
                    0x60000000 => *inst &= 0xffff0000, // ori
                    0x98000000 => *inst &= 0xffff0000, // stb
                    0x9c000000 => *inst &= 0xffff0000, // stbu
                    0xd8000000 => *inst &= 0xffff0000, // stfd
                    0xdc000000 => *inst &= 0xffff0000, // stfdu
                    0xd0000000 => *inst &= 0xffff0000, // stfs
                    0xd4000000 => *inst &= 0xffff0000, // stfsu
                    0xb0000000 => *inst &= 0xffff0000, // sth
                    0xb4000000 => *inst &= 0xffff0000, // sthu
                    0x90000000 => *inst &= 0xffff0000, // stw
                    0x94000000 => *inst &= 0xffff0000, // stwu
                    _ => (),
                }
            }
        }
        sections.push(Section { address, kind, vals });
    }
    return Ok(sections);
}

fn parse_rel(rel: Vec<u8>, base: usize) -> Result<Vec<Section>, Error> {
    let section_count = read_u32(&rel, 0xc)? as usize;
    let section_headers_offset = read_u32(&rel, 0x10)? as usize;
    if section_headers_offset % 4 != 0 {
        return Err(Error::Invalid);
    }
    let mut sections = Vec::with_capacity(section_count);
    for i in 0..section_count {
        let section_header_offset = section_headers_offset + i * 0x8;
        let val = read_u32(&rel, section_header_offset + 0x0)? as usize;
        let offset = val & !0x3;
        let size = read_u32(&rel, section_header_offset + 0x4)? as usize;
        if offset % 4 != 0 || size % 4 != 0 {
            return Err(Error::Invalid);
        }
        let kind = match (val & 0x1, offset, size) {
            (0x1, 0x0, _) => return Err(Error::Invalid),
            (0x1, _, 0x0) => return Err(Error::Invalid),
            (0x1, _, _) => SectionKind::Text,
            (_, _, 0x0) => continue,
            (_, 0x0, _) => SectionKind::Bss,
            _ => SectionKind::Data,
        };
        let slice = rel.get(offset..offset + size).ok_or(Error::Oob)?;
        let mut vals: Vec<u32> = slice
            .chunks_exact(4)
            .map(|chunk| {
                let array = <[u8; 4]>::try_from(chunk).unwrap();
                u32::from_be_bytes(array)
            })
            .collect();
        if kind == SectionKind::Text {
            for inst in &mut vals {
                if *inst & 0xfc000000 == 0x48000000 { // b, bl
                    *inst &= 0xfc000001;
                }
            }
        }
        let address = offset + base;
        sections.push(Section { address, kind, vals });
    }
    return Ok(sections);
}

fn process(source: &Section, target: &Section) -> Vec<Match> {
    let mut min_size = 0x10000.min(source.vals.len()).min(target.vals.len());
    let mut matches = vec![];
    while min_size >= 0x100.min(source.vals.len()).min(target.vals.len()) {
        while let Some(best_match) = find_best_match(min_size, &source, &target, &matches) {
            matches.push(best_match);

            let sum: usize = matches.iter().map(|m| m.size).sum();
            let source_coverage = sum as f32 / source.vals.len() as f32;
            let target_coverage = sum as f32 / target.vals.len() as f32;
            let coverage = source_coverage.max(target_coverage);

            eprintln!("{:?}", coverage);
        }
        min_size /= 2;
    }
    eprintln!();
    return matches;
}

fn postprocess(source: &Section, matches: &mut Vec<Match>) {
    if source.kind != SectionKind::Text {
        return;
    }

    matches.sort_unstable_by_key(|m| m.source_start);
    for i in 0..matches.len() - 1 {
        let matches = &mut matches[i..i + 2];
        if matches[0].source_end() <= matches[1].source_start {
            continue;
        }
        let overlap_size = matches[0].source_end() - matches[1].source_start;
        for j in 0..overlap_size {
            if source.vals[matches[1].source_start + j] == 0x4e800020 {
                matches[0].size -= overlap_size - (j + 0x1);
                matches[1].source_start += j + 0x1;
                matches[1].target_start += j + 0x1;
                matches[1].size -= j + 0x1;
                break;
            }
        }
    }
}

fn dump(source: &Section, target: &Section, matches: &Vec<Match>) {
    for m in matches {
        let source_start = source.address + m.source_start * 4;
        let target_start = target.address + m.target_start * 4;
        let source_end = source_start + m.size * 4;
        let target_end = target_start + m.size * 4;
        println!("{:x?} {:x?} => {:x?} {:x?}", source_start, source_end, target_start, target_end);
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

    fn contains_source(&self, source_offset: usize) -> bool {
        self.source_start <= source_offset && self.source_start + self.size > source_offset
    }

    fn contains_target(&self, target_offset: usize) -> bool {
        self.target_start <= target_offset && self.target_start + self.size > target_offset
    }
}

fn find_best_match(min_size: usize, source: &Section, target: &Section, matches: &[Match]) -> Option<Match> {
    let mut best_match = None;
    for source_offset in (min_size / 2..source.vals.len()).step_by(min_size) {
        if matches.iter().any(|m| m.contains_source(source_offset)) {
            continue;
        }
        let mut target_offset = 0x0;
        while target_offset < target.vals.len() {
            if let Some(m) = matches.iter().find(|m| m.contains_target(target_offset)) {
                target_offset += m.size;
            }
            let mut left_size = 0x0;
            loop {
                let source_val = source.vals.get(source_offset - 0x1 - left_size);
                let target_val = target.vals.get(target_offset - 0x1 - left_size);
                match (source_val, target_val) {
                    (Some(source_val), Some(target_val)) if source_val == target_val => (),
                    _ => break,
                }
                left_size += 0x1;
            }
            let mut right_size = 0x0;
            loop {
                let source_val = source.vals.get(source_offset + right_size);
                let target_val = target.vals.get(target_offset + right_size);
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
    ExtensionNotFound,
    Invalid,
    InvalidExtension,
    Io(io::Error),
    MismatchedExtensions,
    MismatchedSections,
    Oob,
    ParseInt(ParseIntError),
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
