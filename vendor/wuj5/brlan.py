from common import *


target_kind_variants = {
    'RLMC': [
        Variant('material color r', 0x00),
        Variant('material color g', 0x01),
        Variant('material color b', 0x02),
        Variant('material color a', 0x03),
        Variant('tev color 0 color r', 0x04),
        Variant('tev color 0 color g', 0x05),
        Variant('tev color 0 color b', 0x06),
        Variant('tev color 0 color a', 0x07),
        Variant('tev color 1 color r', 0x08),
        Variant('tev color 1 color g', 0x09),
        Variant('tev color 1 color b', 0x0a),
        Variant('tev color 1 color a', 0x0b),
        Variant('tev color 2 color r', 0x0c),
        Variant('tev color 2 color g', 0x0d),
        Variant('tev color 2 color b', 0x0e),
        Variant('tev color 2 color a', 0x0f),
        Variant('tev konst 0 color r', 0x10),
        Variant('tev konst 0 color g', 0x11),
        Variant('tev konst 0 color b', 0x12),
        Variant('tev konst 0 color a', 0x13),
        Variant('tev konst 1 color r', 0x14),
        Variant('tev konst 1 color g', 0x15),
        Variant('tev konst 1 color b', 0x16),
        Variant('tev konst 1 color a', 0x17),
        Variant('tev konst 2 color r', 0x18),
        Variant('tev konst 2 color g', 0x19),
        Variant('tev konst 2 color b', 0x1a),
        Variant('tev konst 2 color a', 0x1b),
        Variant('tev konst 3 color r', 0x1c),
        Variant('tev konst 3 color g', 0x1d),
        Variant('tev konst 3 color b', 0x1e),
        Variant('tev konst 3 color a', 0x1f),
    ],
    'RLPA': [
        Variant('translate x', 0x0),
        Variant('translate y', 0x1),
        Variant('translate z', 0x2),
        Variant('rotate x', 0x3),
        Variant('rotate y', 0x4),
        Variant('rotate z', 0x5),
        Variant('scale x', 0x6),
        Variant('scale y', 0x7),
        Variant('size w', 0x8),
        Variant('size h', 0x9),
    ],
    'RLTP': [
        Variant('image', 0x0),
        Variant('palette', 0x1),
    ],
    'RLTS': [
        Variant('translate s', 0x0),
        Variant('translate t', 0x1),
        Variant('rotate', 0x2),
        Variant('scale s', 0x3),
        Variant('scale t', 0x4),
    ],
    'RLVC': [
        Variant('top left r', 0x00),
        Variant('top left g', 0x01),
        Variant('top left b', 0x02),
        Variant('top left a', 0x03),
        Variant('top right r', 0x04),
        Variant('top right g', 0x05),
        Variant('top right b', 0x06),
        Variant('top right a', 0x07),
        Variant('bottom left r', 0x08),
        Variant('bottom left g', 0x09),
        Variant('bottom left b', 0x0a),
        Variant('bottom left a', 0x0b),
        Variant('bottom right r', 0x0c),
        Variant('bottom right g', 0x0d),
        Variant('bottom right b', 0x0e),
        Variant('bottom right a', 0x0f),
        Variant('pane alpha', 0x10),
    ],
    'RLVI': [
        Variant('visibility', 0x0),
    ],
}

curve_type_variants = [
    Variant('constant', 0x0),
    Variant('step', 0x1),
    Variant('hermite', 0x2),
]

content_kind_variants = [
    Variant('pane', 0x0),
    Variant('material', 0x1),
]

def unpack_pat1(in_data, offset):
    name_offset = offset + unpack_u32(in_data, offset + 0x0c)
    name = in_data[name_offset:].split(b'\0')[0].decode('ascii')

    group_count = unpack_u16(in_data, offset + 0x0a)
    groups_offset = unpack_u32(in_data, offset + 0x10)
    groups = []
    for i in range(group_count):
        group_offset = offset + groups_offset + i * 0x14
        groups += [{
            'name': in_data[group_offset:group_offset + 0x10].decode('ascii').rstrip('\0')
        }]

    return {
        'magic': unpack_magic(in_data, offset + 0x00),
        'id': unpack_u16(in_data, offset + 0x08),
        'groups': groups,
        'name': name,
        'start frame': unpack_s16(in_data, offset + 0x14),
        'end frame': unpack_s16(in_data, offset + 0x16),
        'descending bind': unpack_bool8(in_data, offset + 0x18),
    }

def unpack_step_key(in_data, offset):
    return {
        'frame': unpack_f32(in_data, offset + 0x0),
        'value': unpack_u16(in_data, offset + 0x4),
    }

def unpack_hermite_key(in_data, offset):
    return {
        'frame': unpack_f32(in_data, offset + 0x0),
        'value': unpack_f32(in_data, offset + 0x4),
        'slope': unpack_f32(in_data, offset + 0x8),
    }

def unpack_target(in_data, offset, magic):
    target_kind = unpack_enum8(
        in_data,
        offset + 0x1,
        size = size,
        unpack = unpack,
        variants = target_kind_variants[magic],
    )

    curve_type = unpack_enum8(
        in_data,
        offset + 0x2,
        size = size,
        unpack = unpack,
        variants = curve_type_variants,
    )

    key_count = unpack_u16(in_data, offset + 0x4)
    keys_offset = unpack_u32(in_data, offset + 0x8)
    keys = []
    for i in range(key_count):
        key_size = {
            'step': 0x8,
            'hermite': 0xc,
        }[curve_type]
        key_offset = offset + keys_offset + i * key_size
        unpack_key = {
            'step': unpack_step_key,
            'hermite': unpack_hermite_key,
        }[curve_type]
        keys += [unpack_key(in_data, key_offset)]

    return {
        'id': unpack_u8(in_data, offset + 0x0),
        'kind': target_kind,
        'curve type': curve_type,
        'keys': keys,
    }

def unpack_animation(in_data, offset):
    magic = unpack_magic(in_data, offset + 0x0)

    target_count = unpack_u8(in_data, offset + 0x4)
    targets = []
    for i in range(target_count):
        target_offset = offset + unpack_u32(in_data, offset + 0x8 + i * 0x4)
        targets += [unpack_target(in_data, target_offset, magic)]
    return {
        'magic': magic,
        'targets': targets,
    }

def unpack_content(in_data, offset):
    kind = unpack_enum8(
        in_data,
        offset + 0x15,
        size = size,
        unpack = unpack,
        variants = content_kind_variants,
    )

    animation_count = unpack_u8(in_data, offset + 0x14)
    animations = []
    for i in range(animation_count):
        animation_offset = offset + unpack_u32(in_data, offset + 0x18 + i * 0x4)
        animations += [unpack_animation(in_data, animation_offset)]

    return {
        'name': in_data[offset:offset + 0x14].decode('ascii').rstrip('\0'),
        'kind': kind,
        'animations': animations,
    }

def unpack_pai1(in_data, offset):
    tpl_count = unpack_u16(in_data, offset + 0x0c)
    tpls = []
    for i in range(tpl_count):
        tpl_offset = offset + 0x14 + unpack_u32(in_data, offset + 0x14 + i * 0x4)
        tpls += [in_data[tpl_offset:].split(b'\0')[0].decode('ascii')]

    content_count = unpack_u16(in_data, offset + 0x0e)
    contents_offset = unpack_u32(in_data, offset + 0x10)
    contents = []
    for i in range(content_count):
        content_offset = offset + unpack_u32(in_data, offset + contents_offset + i * 0x4)
        contents += [unpack_content(in_data, content_offset)]

    return {
        'magic': unpack_magic(in_data, offset + 0x00),
        'frame count': unpack_u16(in_data, offset + 0x08),
        'loop': unpack_bool8(in_data, offset + 0x0a),
        'tpls': tpls,
        'contents': contents,
    }

def unpack_sections(in_data, offset):
    sections = []
    while offset < len(in_data):
        magic = unpack_magic(in_data, offset + 0x00)
        size = unpack_u32(in_data, offset + 0x04)
        section = {
            'pat1': unpack_pat1,
            'pai1': unpack_pai1,
        }[magic](in_data, offset)
        sections += [section]
        offset += size
    return sections

def unpack_brlan(in_data):
    return {
        'version': unpack_u16(in_data, 0x06),
        'sections': unpack_sections(in_data, 0x10),
    }

def pack_pat1(val):
    name_data = val['name'].encode('ascii') + b'\0'
    name_data = name_data.ljust((len(name_data) + 0x3) & ~0x3, b'\0')

    groups_offset = 0x1c + len(name_data)
    groups_data = b''
    for group in val['groups']:
        groups_data += group['name'].encode('ascii').ljust(0x14, b'\0')

    return b''.join([
        pack_magic(val['magic']),
        pack_pad32(None),
        pack_u16(val['id']),
        pack_u16(len(val['groups'])),
        pack_u32(0x1c),
        pack_u32(groups_offset),
        pack_s16(val['start frame']),
        pack_s16(val['end frame']),
        pack_bool8(val['descending bind']),
        pack_pad24(None),
        name_data,
        groups_data,
    ])

def pack_step_key(val):
    return b''.join([
        pack_f32(val['frame']),
        pack_u16(val['value']),
        pack_pad16(None),
    ])

def pack_hermite_key(val):
    return b''.join([
        pack_f32(val['frame']),
        pack_f32(val['value']),
        pack_f32(val['slope']),
    ])

def pack_target(val, magic):
    keys_data = b''
    for key in val['keys']:
        keys_data += {
            'step': pack_step_key,
            'hermite': pack_hermite_key,
        }[val['curve type']](key)

    return b''.join([
        pack_u8(val['id']),
        pack_enum8(
            val['kind'],
            pack = pack,
            variants = target_kind_variants[magic],
        ),
        pack_enum8(
            val['curve type'],
            pack = pack,
            variants = curve_type_variants,
        ),
        pack_pad8(None),
        pack_u16(len(val['keys'])),
        pack_pad16(None),
        pack_u32(0xc),
        keys_data,
    ])

def pack_animation(val):
    target_offset = 0x8 + 0x4 * len(val['targets'])
    target_offsets_data = b''
    targets_data = b''
    for target in val['targets']:
        target_offsets_data += pack_u32(target_offset)
        target_data = pack_target(target, val['magic'])
        targets_data += target_data
        target_offset += len(target_data)
    targets_data = target_offsets_data + targets_data

    return b''.join([
        pack_magic(val['magic']),
        pack_u8(len(val['targets'])),
        pack_pad24(None),
        targets_data,
    ])

def pack_content(val):
    animation_offset = 0x18 + 0x4 * len(val['animations'])
    animation_offsets_data = b''
    animations_data = b''
    for animation in val['animations']:
        animation_offsets_data += pack_u32(animation_offset)
        animation_data = pack_animation(animation)
        animations_data += animation_data
        animation_offset += len(animation_data)
    animations_data = animation_offsets_data + animations_data

    return b''.join([
        val['name'].encode('ascii').ljust(0x14, b'\0'),
        pack_u8(len(val['animations'])),
        pack_enum8(
            val['kind'],
            pack = pack,
            variants = content_kind_variants,
        ),
        pack_pad16(None),
        animations_data,
    ])

def pack_pai1(val):
    tpl_offset = 0x4 * len(val['tpls'])
    tpl_offsets_data = b''
    tpls_data = b''
    for tpl in val['tpls']:
        tpl_offsets_data += pack_u32(tpl_offset)
        tpl_data = tpl.encode('ascii') + b'\0'
        tpls_data += tpl_data
        tpl_offset += len(tpl_data)
    tpls_data = tpl_offsets_data + tpls_data
    tpls_data = tpls_data.ljust((len(tpls_data) + 0x3) & ~0x3, b'\0')

    contents_offset = 0x14 + len(tpls_data)
    content_offset = contents_offset + 0x4 * len(val['contents'])
    content_offsets_data = b''
    contents_data = b''
    for content in val['contents']:
        content_offsets_data += pack_u32(content_offset)
        content_data = pack_content(content)
        contents_data += content_data
        content_offset += len(content_data)
    contents_data = content_offsets_data + contents_data

    return b''.join([
        pack_magic(val['magic']),
        pack_pad32(None),
        pack_u16(val['frame count']),
        pack_bool8(val['loop']),
        pack_pad8(None),
        pack_u16(len(val['tpls'])),
        pack_u16(len(val['contents'])),
        pack_u32(contents_offset),
        tpls_data,
        contents_data,
    ])

def pack_sections(sections):
    out_data = b''
    for section in sections:
        section_data = {
            'pat1': pack_pat1,
            'pai1': pack_pai1,
        }[section['magic']](section)
        section_data = section_data.ljust((len(section_data) + 0x3) & ~0x3, b'\0')
        section_data = section_data[0x0:0x4] + pack_u32(len(section_data)) + section_data[0x8:]
        out_data += section_data
    return out_data

def pack_brlan(val):
    sections_data = pack_sections(val['sections'])

    return b''.join([
        pack_magic('RLAN'),
        b'\xfe\xff',
        pack_u16(val['version']),
        pack_u32(0x10 + len(sections_data)),
        pack_u16(0x10),
        pack_u16(len(val['sections'])),
        sections_data,
    ])
