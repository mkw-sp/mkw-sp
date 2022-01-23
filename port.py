#!/usr/bin/env python3


from argparse import ArgumentParser
from dataclasses import dataclass
import sys


@dataclass
class Section:
    start: int
    end: int

    def __contains__(self, address):
        return self.start <= address < self.end

@dataclass
class SrcBinary:
    start: int
    sections: [Section]

    def __contains__(self, address):
        return any(address in section for section in self.sections)

@dataclass
class DstBinary:
    start: int
    end: int

@dataclass
class Chunk:
    src_start: int
    src_end: int
    dst_start: int

    def __contains__(self, address):
        return self.src_start <= address < self.src_end

    def port(self, address):
        return address - self.src_start + self.dst_start


SRC_BINARIES = {
    'dol': SrcBinary(
        0x80004000,
        [
            Section(0x80004000, 0x80006460),
            Section(0x80006460, 0x80006a20),
            Section(0x80006a20, 0x800072c0),
            Section(0x800072c0, 0x80244de0),
            Section(0x80244de0, 0x80244e90),
            Section(0x80244ea0, 0x80244eac),
            Section(0x80244ec0, 0x80258580),
            Section(0x80258580, 0x802a4040),
            Section(0x802a4080, 0x80384c00),
            Section(0x80384c00, 0x80385fc0),
            Section(0x80385fc0, 0x80386fa0),
            Section(0x80386fa0, 0x80389140),
            Section(0x80389140, 0x8038917c),
        ],
    ),
    'rel': SrcBinary(
        0x805102e0,
        [
            Section(0x805103b4, 0x8088f400),
            Section(0x8088f400, 0x8088f704),
            Section(0x8088f704, 0x8088f710),
            Section(0x8088f720, 0x808b2bd0),
            Section(0x808b2bd0, 0x808dd3d4),
            Section(0x809bd6e0, 0x809c4f90),
        ],
    ),
}

DST_BINARIES = {
    'P': {
        'dol': DstBinary(0x80004000, 0x8038917c),
        'rel': DstBinary(0x80399180, 0x8076db50),
    },
    'E': {
        'dol': DstBinary(0x80004000, 0x80384dfc),
        'rel': DstBinary(0x80394e00, 0x807693f0),
    },
    'J': {
        'dol': DstBinary(0x80004000, 0x80388afc),
        'rel': DstBinary(0x80398b00, 0x8076cc90),
    },
    'K': {
        'dol': DstBinary(0x80004000, 0x8037719c),
        'rel': DstBinary(0x803871a0, 0x8075bfd0),
    },
}

CHUNKS = {
    'E': [
        Chunk(0x80004000, 0x80008004, 0x80004000),
        Chunk(0x800080f4, 0x8000ac50, 0x800080b4),
        Chunk(0x8000af78, 0x8000b6b4, 0x8000aed8),
        Chunk(0x80021bb0, 0x80225f14, 0x80021b10),
        Chunk(0x80226464, 0x802402e0, 0x802260e0),
        Chunk(0x802a4080, 0x80384c18, 0x8029fd00),
        Chunk(0x80385fc0, 0x80386008, 0x80381c40),
        Chunk(0x80386f48, 0x80386f90, 0x80382bc0),
    ],
    'J': [
        Chunk(0x80004000, 0x80008004, 0x80004000),
        Chunk(0x800080f4, 0x8000ac50, 0x80008050),
        Chunk(0x8000af78, 0x80021ba8, 0x8000ae9c),
        Chunk(0x80021bb0, 0x80244ea4, 0x80021ad0),
        Chunk(0x802a4080, 0x8038917c, 0x802a3a00),
    ],
    'K': [
        Chunk(0x80004000, 0x800074dc, 0x80004000),
        Chunk(0x800077c8, 0x800079d4, 0x80007894),
        Chunk(0x80007bc0, 0x80007bcc, 0x80007cac),
        Chunk(0x80007f2c, 0x80008004, 0x80008034),
        Chunk(0x80008c10, 0x80009198, 0x80008d60),
        Chunk(0x8000951c, 0x8000ac54, 0x80009624),
        Chunk(0x8000af78, 0x8000b610, 0x8000b024),
        Chunk(0x8000b654, 0x80021ba8, 0x8000b6bc),
        Chunk(0x80021bb0, 0x800ea264, 0x80021c10),
        Chunk(0x800ea4d4, 0x80164294, 0x800ea54c),
        Chunk(0x80164364, 0x801746fc, 0x80164400),
        Chunk(0x8017f680, 0x801e8414, 0x8017f9dc),
        Chunk(0x802100a0, 0x80244de0, 0x80210414),
        Chunk(0x802a4080, 0x803858e0, 0x80292080),
        Chunk(0x80385fc0, 0x8038917c, 0x80373fe0),
    ],
}


def write_symbol(out_file, name, address):
    out_file.write(f'    {name} = {address:#x};\n');

def get_binary_name(address):
    return next(name for name, src_binary in SRC_BINARIES.items() if address in src_binary)

def port(region, address):
    if region == 'P':
        return address

    return next((chunk.port(address) for chunk in CHUNKS[region] if address in chunk), None)


parser = ArgumentParser()
parser.add_argument('region')
parser.add_argument('in_path')
parser.add_argument('out_path')
args = parser.parse_args()

out_file = open(args.out_path, 'w')
out_file.write('SECTIONS {\n')

out_file.write('    .text base : { *(first) *(.text*) }\n')
out_file.write('    patches : { *(patches*) }\n')
out_file.write('    .rodata : { *(.rodata*) }\n')
out_file.write('    .data : { *(.data*) *(.bss*) *(.sbss*) }\n')
out_file.write('\n')

for name, dst_binary in DST_BINARIES[args.region].items():
    write_symbol(out_file, f'{name}_start', dst_binary.start)
    write_symbol(out_file, f'{name}_end', dst_binary.end)
    out_file.write('\n')

symbols = open(args.in_path)
for symbol in symbols.readlines():
    if symbol.isspace():
        out_file.write('\n')
        continue
    address, name = symbol.split()
    address = int(address, 16)
    binary_name = get_binary_name(address)
    is_rel_bss = 0x809bd6e0 <= address < 0x809c4f90
    address = port(args.region, address)
    if address is None:
        sys.exit(f'Couldn\'t port symbol {name} to region {args.region}!')
    if is_rel_bss:
        address -= 0xe02e0
    address -= SRC_BINARIES[binary_name].start
    address += DST_BINARIES[args.region][binary_name].start
    write_symbol(out_file, name, address)

out_file.write('}\n')
