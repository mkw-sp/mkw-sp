import sys

from common import *


def unpack_string64(in_data, offset, **kwargs):
    return in_data[offset:offset + 0x8].decode('ascii').rstrip('\0')

def unpack_string128(in_data, offset, **kwargs):
    return in_data[offset:offset + 0x10].decode('ascii').rstrip('\0')

def unpack_string160(in_data, offset, **kwargs):
    return in_data[offset:offset + 0x14].decode('ascii').rstrip('\0')

def unpack_vstring(in_data, offset, **kwargs):
    voffset = kwargs['voffset']
    offset = voffset + unpack_u32(in_data, offset)
    return in_data[offset:].split(b'\0')[0].decode('ascii')

def unpack_vwstring(in_data, offset, **kwargs):
    voffset = kwargs['voffset']
    offset = voffset + unpack_u32(in_data, offset)
    return in_data[offset:].split(b'\0\0')[0].decode('utf-16-be')

def unpack_pointer(in_data, offset, **kwargs):
    voffset = kwargs['voffset']
    offset = voffset + unpack_u32(in_data, offset)
    return unpack_struct(in_data, offset, **kwargs)

def unpack_array(in_data, offset, kind, **kwargs):
    size = kwargs['size']
    unpack = kwargs['unpack']
    fields = kwargs['fields']
    start_offset = offset + 2 * size[kind]
    count = unpack[kind](in_data, offset)
    size = sum(size[field.kind] for field in fields)
    kwargs = {
        **kwargs,
        'voffset': start_offset,
    }
    return [unpack_struct(in_data, start_offset + i * size, **kwargs) for i in range(count)]

def unpack_array8(in_data, offset, **kwargs):
    return unpack_array(in_data, offset, 'u8', **kwargs)

def unpack_array16(in_data, offset, **kwargs):
    return unpack_array(in_data, offset, 'u16', **kwargs)

def unpack_varray(in_data, offset, kind, has_offset, **kwargs):
    unpack = kwargs['unpack']
    voffset = kwargs['voffset']
    count = unpack[kind](in_data, offset)
    if has_offset:
        offset = voffset + unpack_u32(in_data, offset + 0x4)
    else:
        offset += 0x4
    vals = []
    for i in range(count):
        val_offset = voffset + unpack_u32(in_data, offset + i * 0x4)
        vals += [unpack_struct(in_data, val_offset, **kwargs)]
    return vals

def unpack_varray8o(in_data, offset, **kwargs):
    return unpack_varray(in_data, offset, 'u8', True, **kwargs)

def unpack_varray16(in_data, offset, **kwargs):
    return unpack_varray(in_data, offset, 'u16', False, **kwargs)

def unpack_vstruct(in_data, offset, **kwargs):
    size = kwargs['size']
    unpack = kwargs['unpack']
    fields = kwargs['fields']
    counts = unpack_u32(in_data, offset)
    field_offset = offset + 0x4
    val = {}
    for field in fields:
        shift = field.kwargs['shift']
        mask = field.kwargs['mask']
        count = counts >> shift & mask
        if field.kind is None or count == 0:
            continue
        kwargs = {
            **kwargs,
            'struct_offset': offset,
            **field.kwargs,
        }
        vals = []
        for _ in range(count):
            vals += [unpack[field.kind](in_data, field_offset, **kwargs)]
            if field.kind == 'struct':
                struct_fields = field.kwargs['fields']
                field_offset += sum(size[struct_field.kind] for struct_field in struct_fields)
            else:
                field_offset += size[field.kind]
        if mask == 0x1:
            val[field.name] = vals[0]
        else:
            val[field.name] = vals
    return val

def pack_string64(val, **kwargs):
    return val.encode('ascii').ljust(0x8, b'\0')

def pack_string128(val, **kwargs):
    return val.encode('ascii').ljust(0x10, b'\0')

def pack_string160(val, **kwargs):
    return val.encode('ascii').ljust(0x14, b'\0')

def pack_vstring(val, **kwargs):
    buffer = kwargs['buffer']
    offset = buffer.size()
    buffer.push(val.encode('ascii') + b'\0')
    return pack_u32(offset)

def pack_vwstring(val, **kwargs):
    buffer = kwargs['buffer']
    offset = buffer.size()
    buffer.push(val.encode('utf-16-be') + b'\0\0')
    return pack_u32(offset)

def pack_pointer(val, **kwargs):
    buffer = kwargs['buffer']
    offset = buffer.size()
    buffer.push(pack_struct(val, **kwargs))
    return pack_u32(offset)

def pack_array(vals, kind, **kwargs):
    size = kwargs['size']
    pack = kwargs['pack']
    fields = kwargs['fields']
    out_data = b''.join([
        pack[kind](len(vals)),
        b'\x00' * size[kind],
    ])
    buffer_offset = len(vals) * sum(size[field.kind] for field in fields)
    buffer = Buffer(buffer_offset)
    kwargs = {
        **kwargs,
        'buffer': buffer,
    }
    for val in vals:
        out_data += pack_struct(val, **kwargs)
    return out_data + buffer.buffer

def pack_array8(vals, **kwargs):
    return pack_array(vals, 'u8', **kwargs)

def pack_array16(vals, **kwargs):
    return pack_array(vals, 'u16', **kwargs)

def pack_varray(vals, kind, has_offset, **kwargs):
    size = kwargs['size']
    pack = kwargs['pack']
    buffer = kwargs['buffer']
    out_data = b''.join([
        pack[kind](len(vals)),
        b'\x00' * (4 - size[kind]),
    ])
    if has_offset:
        out_data += pack_u32(buffer.size())
        val_offset = 0x4 * len(vals) + buffer.size()
        val_data = b''
        for val in vals:
            buffer.push(pack_u32(val_offset + len(val_data)))
            val_data += pack_struct(val, **kwargs)
        buffer.push(val_data)
    else:
        for val in vals:
            out_data += pack_u32(0x4 * len(vals) + buffer.size())
            buffer.push(pack_struct(val, **kwargs))
    return out_data

def pack_varray8o(vals, **kwargs):
    return pack_varray(vals, 'u8', True, **kwargs)

def pack_varray16(vals, **kwargs):
    return pack_varray(vals, 'u16', False, **kwargs)

def pack_vstruct(val, **kwargs):
    size = kwargs['size']
    pack = kwargs['pack']
    fields = kwargs['fields']
    counts = 0
    out_data = b''
    for field in fields:
        field_val = val.get(field.name)
        if field_val is None:
            continue
        elif isinstance(field_val, dict):
            vals = [field_val]
        else:
            vals = field_val
        shift = field.kwargs['shift']
        count = len(vals)
        counts |= count << shift
        kwargs = {
            **kwargs,
            **field.kwargs,
        }
        for array_val in vals:
            out_data += pack[field.kind](array_val, **kwargs)
    return pack_u32(counts) + out_data

brlyt_size = {
    **size,
    'string64': 0x8,
    'string128': 0x10,
    'string160': 0x14,
    'vstring': 0x4,
    'vwstring': 0x4,
    'pointer': 0x4,
    'array8': 0x2,
    'array16': 0x4,
    'varray8o': 0x8,
    'varray16': 0x4,
    'vstruct': 0x4,
}

brlyt_unpack = {
    **unpack,
    'string64': unpack_string64,
    'string128': unpack_string128,
    'string160': unpack_string160,
    'vstring': unpack_vstring,
    'vwstring': unpack_vwstring,
    'pointer': unpack_pointer,
    'array8': unpack_array8,
    'array16': unpack_array16,
    'varray8o': unpack_varray8o,
    'varray16': unpack_varray16,
    'vstruct': unpack_vstruct,
}

brlyt_pack = {
    **pack,
    'string64': pack_string64,
    'string128': pack_string128,
    'string160': pack_string160,
    'vstring': pack_vstring,
    'vwstring': pack_vwstring,
    'pointer': pack_pointer,
    'array8': pack_array8,
    'array16': pack_array16,
    'varray8o': pack_varray8o,
    'varray16': pack_varray16,
    'vstruct': pack_vstruct,
}


uv_set_fields = [
    Field('f32', 'top left u'),
    Field('f32', 'top left v'),
    Field('f32', 'top right u'),
    Field('f32', 'top right v'),
    Field('f32', 'bottom left u'),
    Field('f32', 'bottom left v'),
    Field('f32', 'bottom right u'),
    Field('f32', 'bottom right v'),
]

position_variants = [
    Variant('top left', 0),
    Variant('top center', 1),
    Variant('top right', 2),
    Variant('center left', 3),
    Variant('center', 4),
    Variant('center right', 5),
    Variant('bottom left', 6),
    Variant('bottom center', 7),
    Variant('bottom right', 8),
]

base_fields = [
    Field('magic', 'magic'),
    Field('u32', 'size'),
]

lyt1_fields = [
    *base_fields,
    Field('bool8', 'centered'),
    Field('pad24', None),
    Field('f32', 'size x'),
    Field('f32', 'size y'),
]

txl1_fields = [
    *base_fields,
    Field('array16', 'tpls', fields = [
        Field('vstring', 'name'),
        Field('pad32', None),
    ]),
]

fnl1_fields = [
    *base_fields,
    Field('array16', 'brfnts', fields = [
        Field('vstring', 'name'),
        Field('pad32', None),
    ]),
]

mat1_fields = [
    *base_fields,
    Field('varray16', 'materials', fields = [
        Field('string160', 'name'),
        Field('s16', 'tev color 0 r'),
        Field('s16', 'tev color 0 g'),
        Field('s16', 'tev color 0 b'),
        Field('s16', 'tev color 0 a'),
        Field('s16', 'tev color 1 r'),
        Field('s16', 'tev color 1 g'),
        Field('s16', 'tev color 1 b'),
        Field('s16', 'tev color 1 a'),
        Field('s16', 'tev color 2 r'),
        Field('s16', 'tev color 2 g'),
        Field('s16', 'tev color 2 b'),
        Field('s16', 'tev color 2 a'),
        Field('u8', 'tev k color 0 r'),
        Field('u8', 'tev k color 0 g'),
        Field('u8', 'tev k color 0 b'),
        Field('u8', 'tev k color 0 a'),
        Field('u8', 'tev k color 1 r'),
        Field('u8', 'tev k color 1 g'),
        Field('u8', 'tev k color 1 b'),
        Field('u8', 'tev k color 1 a'),
        Field('u8', 'tev k color 2 r'),
        Field('u8', 'tev k color 2 g'),
        Field('u8', 'tev k color 2 b'),
        Field('u8', 'tev k color 2 a'),
        Field('u8', 'tev k color 3 r'),
        Field('u8', 'tev k color 3 g'),
        Field('u8', 'tev k color 3 b'),
        Field('u8', 'tev k color 3 a'),
        Field('vstruct', 'attributes', fields = [
            Field('struct', 'texture maps', shift = 0, mask = 0xf, fields = [
                Field('u16', 'texture index'),
                Field('u8', 's'),
                Field('u8', 't'),
            ]),
            Field('struct', 'texture srts', shift = 4, mask = 0xf, fields = [
                Field('f32', 'translate x'),
                Field('f32', 'translate y'),
                Field('f32', 'rotate'),
                Field('f32', 'scale x'),
                Field('f32', 'scale y'),
            ]),
            Field('struct', 'texture uv gens', shift = 8, mask = 0xf, fields = [
                Field('u8', 'type'),
                Field('u8', 'source'),
                Field('u8', 'matrix'),
                Field('pad8', None),
            ]),
            Field('struct', 'channel control', shift = 25, mask = 0x1, fields = [
                Field('u8', 'color source material'),
                Field('u8', 'alpha source material'),
                Field('pad16', None),
            ]),
            Field('struct', 'material color', shift = 27, mask = 0x1, fields = [
                Field('u8', 'r'),
                Field('u8', 'g'),
                Field('u8', 'b'),
                Field('u8', 'a'),
            ]),
            Field('struct', 'tev swaps', shift = 12, mask = 0x1, fields = [
                Field('u8', 'tev swap 0'),
                Field('u8', 'tev swap 1'),
                Field('u8', 'tev swap 2'),
                Field('u8', 'tev swap 3'),
            ]),
            Field('struct', 'indirect texture srts', shift = 13, mask = 0x3, fields = [
                Field('f32', 'translate x'),
                Field('f32', 'translate y'),
                Field('f32', 'rotate'),
                Field('f32', 'scale x'),
                Field('f32', 'scale y'),
            ]),
            Field('struct', 'indirect texture stages', shift = 15, mask = 0x7, fields = [
                Field('u8', 'uv gen'),
                Field('u8', 'map'),
                Field('u8', 'wrap s'),
                Field('u8', 'wrap t'),
            ]),
            Field('struct', 'tev stages', shift = 18, mask = 0x1f, fields = [
                Field('u8', 'uv gen'),
                Field('u8', 'color channel'),
                Field('u8', 'map'),
                Field('u8', 'swap sels'),
                Field('u32', 'color op'),
                Field('u32', 'alpha op'),
                Field('u32', 'indirect'),
            ]),
            Field('struct', 'alpha compare', shift = 23, mask = 0x1, fields = [
                Field('u8', 'condition'),
                Field('u8', 'operator'),
                Field('u8', 'value 0'),
                Field('u8', 'value 1'),
            ]),
            Field('struct', 'blend mode', shift = 24, mask = 0x1, fields = [
                Field('u8', 'type'),
                Field('u8', 'src factor'),
                Field('u8', 'dst factor'),
                Field('u8', 'operator'),
            ]),
        ]),
    ]),
]

pan1_fields = [
    *base_fields,
    Field('bitfield8', 'flags', fields = [
        Field('bool8', 'visible', bits = 1),
        Field('bool8', 'influenced alpha', bits = 1),
        Field('bool8', 'location adjust', bits = 1),
    ]),
    Field('enum8', 'base position', variants = position_variants),
    Field('u8', 'opacity'),
    Field('pad8', None),
    Field('string128', 'name'),
    Field('string64', 'user data'),
    Field('f32', 'translation x'),
    Field('f32', 'translation y'),
    Field('f32', 'translation z'),
    Field('f32', 'rotation x'),
    Field('f32', 'rotation y'),
    Field('f32', 'rotation z'),
    Field('f32', 'scale x'),
    Field('f32', 'scale y'),
    Field('f32', 'size x'),
    Field('f32', 'size y'),
]

pas1_fields = [
    *base_fields,
]

pae1_fields = [
    *base_fields,
]

pic1_fields = [
    *pan1_fields,
    Field('u8', 'vertex color top left r'),
    Field('u8', 'vertex color top left g'),
    Field('u8', 'vertex color top left b'),
    Field('u8', 'vertex color top left a'),
    Field('u8', 'vertex color top right r'),
    Field('u8', 'vertex color top right g'),
    Field('u8', 'vertex color top right b'),
    Field('u8', 'vertex color top right a'),
    Field('u8', 'vertex color bottom left r'),
    Field('u8', 'vertex color bottom left g'),
    Field('u8', 'vertex color bottom left b'),
    Field('u8', 'vertex color bottom left a'),
    Field('u8', 'vertex color bottom right r'),
    Field('u8', 'vertex color bottom right g'),
    Field('u8', 'vertex color bottom right b'),
    Field('u8', 'vertex color bottom right a'),
    Field('u16', 'material'),
    Field('array8', 'uv sets', fields = uv_set_fields),
]

bnd1_fields = [
    *pan1_fields,
]

txt1_fields = [
    *pan1_fields,
    Field('u16', 'maximum string size'),
    Field('u16', 'string size'),
    Field('u16', 'material'),
    Field('u16', 'font'),
    Field('enum8', 'text position', variants = position_variants),
    Field('enum8', 'text alignment', variants = [
        Variant('unspecified', 0),
        Variant('left', 1),
        Variant('center', 2),
        Variant('right', 3),
    ]),
    Field('pad16', None),
    Field('vwstring', 'text'),
    Field('u8', 'top color r'),
    Field('u8', 'top color g'),
    Field('u8', 'top color b'),
    Field('u8', 'top color a'),
    Field('u8', 'bottom color r'),
    Field('u8', 'bottom color g'),
    Field('u8', 'bottom color b'),
    Field('u8', 'bottom color a'),
    Field('f32', 'font size x'),
    Field('f32', 'font size y'),
    Field('f32', 'character space'),
    Field('f32', 'line space'),
]

wnd1_fields = [
    *pan1_fields,
    Field('f32', 'overlap left'),
    Field('f32', 'overlap right'),
    Field('f32', 'overlap top'),
    Field('f32', 'overlap bottom'),
    Field('pointer', 'content', fields = [
        Field('u8', 'vertex color top left r'),
        Field('u8', 'vertex color top left g'),
        Field('u8', 'vertex color top left b'),
        Field('u8', 'vertex color top left a'),
        Field('u8', 'vertex color top right r'),
        Field('u8', 'vertex color top right g'),
        Field('u8', 'vertex color top right b'),
        Field('u8', 'vertex color top right a'),
        Field('u8', 'vertex color bottom left r'),
        Field('u8', 'vertex color bottom left g'),
        Field('u8', 'vertex color bottom left b'),
        Field('u8', 'vertex color bottom left a'),
        Field('u8', 'vertex color bottom right r'),
        Field('u8', 'vertex color bottom right g'),
        Field('u8', 'vertex color bottom right b'),
        Field('u8', 'vertex color bottom right a'),
        Field('u16', 'material'),
        Field('array8', 'uv sets', fields = uv_set_fields),
    ]),
    Field('varray8o', 'frames', fields = [
        Field('u16', 'material'),
        Field('enum8', 'transform', variants = [
            Variant('none', 0),
            Variant('hflip', 1),
            Variant('vflip', 2),
            Variant('rotate 90', 3),
            Variant('rotate 180', 4),
            Variant('rotate 270', 5),
        ]),
        Field('pad8', None),
    ]),
]

grp1_fields = [
    *base_fields,
    Field('string128', 'name'),
    Field('array16', 'panes', fields = [
        Field('string128', 'name'),
    ]),
]

grs1_fields = [
    *base_fields,
]

gre1_fields = [
    *base_fields,
]

section_fields = {
    'lyt1': lyt1_fields,
    'txl1': txl1_fields,
    'fnl1': fnl1_fields,
    'mat1': mat1_fields,
    'pan1': pan1_fields,
    'pas1': pas1_fields,
    'pae1': pae1_fields,
    'pic1': pic1_fields,
    'bnd1': bnd1_fields,
    'txt1': txt1_fields,
    'wnd1': wnd1_fields,
    'grp1': grp1_fields,
    'grs1': grs1_fields,
    'gre1': gre1_fields,
}

def unpack_sections(in_data, offset, parent_magic):
    sections = []
    last_section = None
    while offset < len(in_data):
        magic = unpack_magic(in_data, offset + 0x00)
        # HACK: Group frame count and frame array offset to simplify unpacking
        if magic == 'wnd1':
            in_data = b''.join([
                in_data[:offset + 0x5c],
                in_data[offset + 0x60:offset + 0x64],
                in_data[offset + 0x5c:offset + 0x60],
                in_data[offset + 0x64:],
            ])
        kwargs = {
            'size': brlyt_size,
            'unpack': brlyt_unpack,
            'pack': brlyt_pack,
            'fields': section_fields[magic],
            'voffset': offset,
        }
        section = unpack_struct(in_data, offset, **kwargs)
        offset += section['size']
        del section['size']
        if magic == 'pas1' or magic == 'grs1':
            last_magic = last_section.get('magic')
            if last_magic is None:
                sys.exit(f'Unexpected {magic} without parent.')
            expected_last_magics = {
                'pas1': ['pan1', 'pic1', 'bnd1', 'txt1', 'wnd1'],
                'grs1': ['grp1'],
            }[magic]
            if last_magic not in expected_last_magics:
                sys.exit(f'Unexpected {magic} after {last_magic}.')
            offset, child_sections = unpack_sections(in_data, offset, last_magic)
            last_section['children'] = child_sections
        elif magic == 'pae1' or magic == 'gre1':
            expected_parent_magics = {
                'pae1': ['pan1', 'pic1', 'bnd1', 'txt1', 'wnd1'],
                'gre1': ['grp1'],
            }[magic]
            if parent_magic not in expected_parent_magics:
                sys.exit(f'Unexpected {magic} after {parent_magic}.')
            break
        else:
            sections += [section]
        last_section = section
    return offset, sections

def unpack_brlyt(in_data):
    return {
        'version': unpack_u16(in_data, 0x06),
        'sections': unpack_sections(in_data, 0x10, None)[1]
    }

def pack_section(section):
    magic = section['magic']
    buffer = Buffer(sum(brlyt_size[field.kind] for field in section_fields[magic]))
    kwargs = {
        'size': brlyt_size,
        'unpack': brlyt_unpack,
        'pack': brlyt_pack,
        'fields': section_fields[magic],
        'buffer': buffer,
    }
    section['size'] = 0x0
    out_data = pack_struct(section, **kwargs) + buffer.buffer
    out_data = out_data.ljust((len(out_data) + 0x3) & ~0x3, b'\x00')
    out_data = out_data[0x0:0x4] + pack_u32(len(out_data)) + out_data[0x8:]
    # HACK: Ungroup back frame count and frame array offset
    if magic == 'wnd1':
        out_data = b''.join([
            out_data[:0x5c],
            out_data[0x60:0x64],
            out_data[0x5c:0x60],
            out_data[0x64:],
        ])
    section_count = 1
    children = section.get('children')
    if children is not None:
        sections = [
            {
                'magic': {
                    'pan1': 'pas1',
                    'pic1': 'pas1',
                    'bnd1': 'pas1',
                    'txt1': 'pas1',
                    'wnd1': 'pas1',
                    'grp1': 'grs1',
                }[magic],
            },
            *children,
            {
                'magic': {
                    'pan1': 'pae1',
                    'pic1': 'pae1',
                    'bnd1': 'pae1',
                    'txt1': 'pae1',
                    'wnd1': 'pae1',
                    'grp1': 'gre1',
                }[magic],
            },
        ]
        sections_data, sections_section_count = pack_sections(sections)
        out_data += sections_data
        section_count += sections_section_count
    return out_data, section_count

def pack_sections(sections):
    out_data = b''
    section_count = 0
    for section in sections:
        section_data, section_section_count = pack_section(section)
        out_data += section_data
        section_count += section_section_count
    return out_data, section_count

def pack_brlyt(val):
    sections_data, section_count = pack_sections(val['sections'])

    header_data = b''.join([
        pack_magic('RLYT'),
        b'\xfe\xff',
        pack_u16(val['version']),
        pack_u32(0x10 + len(sections_data)),
        pack_u16(0x10),
        pack_u16(section_count),
    ])

    return header_data + sections_data
