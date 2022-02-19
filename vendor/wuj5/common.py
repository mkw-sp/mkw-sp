import struct
import sys


def unpack_pad8(in_data, offset, **kwargs):
    return None

def unpack_pad16(in_data, offset, **kwargs):
    return None

def unpack_pad24(in_data, offset, **kwargs):
    return None

def unpack_pad32(in_data, offset, **kwargs):
    return None

def unpack_u8(in_data, offset, **kwargs):
    return struct.unpack_from('>B', in_data, offset)[0]

def unpack_u16(in_data, offset, **kwargs):
    return struct.unpack_from('>H', in_data, offset)[0]

def unpack_u32(in_data, offset, **kwargs):
    return struct.unpack_from('>I', in_data, offset)[0]

def unpack_s16(in_data, offset, **kwargs):
    return struct.unpack_from('>h', in_data, offset)[0]

def unpack_bool8(in_data, offset, **kwargs):
    return unpack_u8(in_data, offset) != 0

def unpack_bool16(in_data, offset, **kwargs):
    return unpack_u16(in_data, offset) != 0

def unpack_f32(in_data, offset, **kwargs):
    return round(struct.unpack_from('>f', in_data, offset)[0], 6)

def unpack_magic(in_data, offset, **kwargs):
    return in_data[offset:offset + 4].decode('ascii')

def unpack_struct(in_data, offset, **kwargs):
    size = kwargs['size']
    unpack = kwargs['unpack']
    fields = kwargs['fields']
    val = {}
    field_offset = offset
    for field in fields:
        kwargs = {
            **kwargs,
            'struct_offset': offset,
            'field': field,
            **field.kwargs,
        }
        field_val = unpack[field.kind](in_data, field_offset, **kwargs)
        if field_val is not None:
            val[field.name] = field_val
        field_offset += size[field.kind]
    return val

def unpack_bitfield(in_data, offset, kind, **kwargs):
    unpack = kwargs['unpack']
    pack = kwargs['pack']
    fields = kwargs['fields']
    val = {}
    raw = unpack[kind](in_data, offset, **kwargs)
    for field in fields:
        bits = field.kwargs['bits']
        mask = 1 << bits - 1
        field_val = unpack[field.kind](pack[kind](raw & mask, **kwargs), 0, **kwargs)
        if field_val is not None:
            val[field.name] = field_val
        raw >>= bits
    return val

def unpack_bitfield8(in_data, offset, **kwargs):
    return unpack_bitfield(in_data, offset, 'u8', **kwargs)

def unpack_enum(in_data, offset, kind, **kwargs):
    unpack = kwargs['unpack']
    variants = kwargs['variants']
    val = unpack[kind](in_data, offset)
    variant = next((variant for variant in variants if variant.val == val), None)
    if variant is None:
        vals = [variant.val for variant in variants]
        sys.exit(f'Unknown enum variant with value {val} (expected one of {vals}).')
    return variant.name

def unpack_enum8(in_data, offset, **kwargs):
    return unpack_enum(in_data, offset, 'u8', **kwargs)

def unpack_enum16(in_data, offset, **kwargs):
    return unpack_enum(in_data, offset, 'u16', **kwargs)

def unpack_enum32(in_data, offset, **kwargs):
    return unpack_enum(in_data, offset, 'u32', **kwargs)

def pack_pad8(val, **kwargs):
    return b'\x00'

def pack_pad16(val, **kwargs):
    return b'\x00\x00'

def pack_pad24(val, **kwargs):
    return b'\x00\x00\x00'

def pack_pad32(val, **kwargs):
    return b'\x00\x00\x00\x00'

def pack_u8(val, **kwargs):
    return struct.pack('>B', val)

def pack_u16(val, **kwargs):
    return struct.pack('>H', val)

def pack_u32(val, **kwargs):
    return struct.pack('>I', val)

def pack_s16(val, **kwargs):
    return struct.pack('>h', val)

def pack_bool8(val, **kwargs):
    return pack_u8(val)

def pack_bool16(val, **kwargs):
    return pack_u16(val)

def pack_f32(val, **kwargs):
    return struct.pack('>f', val)

def pack_magic(val, **kwargs):
    return val.encode('ascii')

def pack_struct(val, **kwargs):
    size = kwargs['size']
    pack = kwargs['pack']
    fields = kwargs['fields']
    out_data = b''
    for field in fields:
        kwargs = {
            **kwargs,
            'field': field,
            **field.kwargs,
        }
        out_data += pack[field.kind](val.get(field.name), **kwargs)
    return out_data

def pack_bitfield(val, kind, **kwargs):
    unpack = kwargs['unpack']
    pack = kwargs['pack']
    fields = kwargs['fields']
    raw = 0
    for field in reversed(fields):
        bits = field.kwargs['bits']
        raw <<= bits
        raw |= unpack[kind](pack[field.kind](val[field.name], **kwargs), 0, **kwargs)
    return pack[kind](raw)

def pack_bitfield8(val, **kwargs):
    return pack_bitfield(val, 'u8', **kwargs)

def pack_enum(name, kind, **kwargs):
    pack = kwargs['pack']
    variants = kwargs['variants']
    variant = next((variant for variant in variants if variant.name == name), None)
    if variant is None:
        names = [variant.name for variant in variants]
        sys.exit(f'Unknown enum variant with name {name} (expected one of {names}).')
    return pack[kind](variant.val)

def pack_enum8(val, **kwargs):
    return pack_enum(val, 'u8', **kwargs)

def pack_enum16(val, **kwargs):
    return pack_enum(val, 'u16', **kwargs)

def pack_enum32(val, **kwargs):
    return pack_enum(val, 'u32', **kwargs)

size = {
    'pad8': 0x1,
    'pad16': 0x2,
    'pad24': 0x3,
    'pad32': 0x4,
    'u8': 0x1,
    'u16': 0x2,
    'u32': 0x4,
    's16': 0x2,
    'bool8': 0x1,
    'bool16': 0x2,
    'f32': 0x4,
    'magic': 0x4,
    'bitfield8': 0x1,
    'enum8': 0x1,
}

unpack = {
    'pad8': unpack_pad8,
    'pad16': unpack_pad16,
    'pad24': unpack_pad24,
    'pad32': unpack_pad32,
    'u8': unpack_u8,
    'u16': unpack_u16,
    'u32': unpack_u32,
    's16': unpack_s16,
    'bool8': unpack_bool8,
    'bool16': unpack_bool16,
    'f32': unpack_f32,
    'magic': unpack_magic,
    'struct': unpack_struct,
    'bitfield8': unpack_bitfield8,
    'enum8': unpack_enum8,
}

pack = {
    'pad8': pack_pad8,
    'pad16': pack_pad16,
    'pad24': pack_pad24,
    'pad32': pack_pad32,
    'u8': pack_u8,
    'u16': pack_u16,
    'u32': pack_u32,
    's16': pack_s16,
    'bool8': pack_bool8,
    'bool16': pack_bool16,
    'f32': pack_f32,
    'magic': pack_magic,
    'struct': pack_struct,
    'bitfield8': pack_bitfield8,
    'enum8': pack_enum8,
}

class Field:
    def __init__(self, kind, name, **kwargs):
        self.kind = kind
        self.name = name
        self.kwargs = kwargs

class Variant:
    def __init__(self, name, val):
        self.name = name
        self.val = val

class Buffer:
    def __init__(self, offset):
        self.buffer = b''
        self.offset = offset

    def size(self):
        return self.offset + len(self.buffer)

    def push(self, data):
        size = self.size()
        self.buffer += data
        return size

class Strings:
    def __init__(self, encoding, terminator):
        self.encoding = encoding
        self.terminator = terminator
        self.buffer = terminator
        self.offsets = { '': 0 }

    def insert(self, string):
        if string in self.offsets:
            return self.offsets[string]

        offset = len(self.buffer)
        self.offsets[string] = offset
        self.buffer += string.encode(self.encoding) + self.terminator
        return offset
