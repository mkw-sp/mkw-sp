from common import *


def unpack_string(in_data, offset, **kwargs):
    strings_offset = kwargs['strings_offset']
    offset = strings_offset + unpack_u16(in_data, offset)
    return in_data[offset:].split(b'\0')[0].decode('ascii')

def unpack_array(in_data, offset, **kwargs):
    size = kwargs['size']
    struct_offset = kwargs['struct_offset']
    fields = kwargs['fields']
    start_offset = struct_offset + unpack_u16(in_data, offset + 0x0)
    count = unpack_u16(in_data, offset + 0x2)
    size = sum(size[field.kind] for field in fields)
    return [unpack_struct(in_data, start_offset + i * size, **kwargs) for i in range(count)]

def pack_string(val, **kwargs):
    strings = kwargs['strings']
    offset = strings.insert(val)
    return pack_u16(offset)

def pack_array(vals, **kwargs):
    buffer = kwargs['buffer']
    offset = buffer.size()
    for val in vals:
        buffer.push(pack_struct(val, **kwargs))
    return struct.pack('>HH', offset, len(vals))

brctr_size = {
    **size,
    'string': 0x2,
    'array': 0x4,
}

brctr_unpack = {
    **unpack,
    'string': unpack_string,
    'array': unpack_array,
}

brctr_pack = {
    **pack,
    'string': pack_string,
    'array': pack_array,
}


group_fields = [
    Field('array', 'groups', fields = [
        Field('string', 'name'),
        Field('string', 'pane'),
        Field('u16', 'first animation'),
        Field('u16', 'animation count'),
    ]),
    Field('array', 'animations', fields = [
        Field('string', 'name'),
        Field('string', 'brlan'),
        Field('string', 'next'),
        Field('bool16', 'reversed'),
        Field('f32', 'speed'),
    ]),
]

variant_fields = [
    Field('array', 'variants', fields = [
        Field('string', 'name'),
        Field('u16', 'opacity'),
        Field('bool16', 'animated'),
        Field('pad16', None),
        Field('f32', 'animation delay'),
        Field('f32', 'translation x 4:3'),
        Field('f32', 'translation y 4:3'),
        Field('f32', 'translation z 4:3'),
        Field('f32', 'scale x 4:3'),
        Field('f32', 'scale y 4:3'),
        Field('f32', 'translation x 16:9'),
        Field('f32', 'translation y 16:9'),
        Field('f32', 'translation z 16:9'),
        Field('f32', 'scale x 16:9'),
        Field('f32', 'scale y 16:9'),
        Field('u16', 'first message'),
        Field('u16', 'message count'),
        Field('u16', 'first picture'),
        Field('u16', 'picture count'),
    ]),
    Field('array', 'messages', fields = [
        Field('string', 'pane'),
        Field('string', 'name'),
        Field('u32', 'message id'),
    ]),
    Field('array', 'pictures', fields = [
        Field('string', 'destination pane'),
        Field('string', 'source pane'),
    ]),
]

def unpack_brctr(in_data):
    strings_offset = unpack_u16(in_data, 0x10)
    group_offset = unpack_u16(in_data, 0x0c)
    variant_offset = unpack_u16(in_data, 0x0e)

    return {
        'main brlyt': unpack_string(in_data, 0x06, strings_offset = strings_offset),
        'bmg': unpack_string(in_data, 0x08, strings_offset = strings_offset), 
        'picture source brlyt': unpack_string(in_data, 0x0a, strings_offset = strings_offset),
        **unpack_struct(in_data, group_offset, size = brctr_size, unpack = brctr_unpack,
                        fields = group_fields, strings_offset = strings_offset),
        **unpack_struct(in_data, variant_offset, size = brctr_size, unpack = brctr_unpack,
                        fields = variant_fields, strings_offset = strings_offset),
    }

def pack_brctr(val):
    strings = Strings('ascii', b'\0')

    header_data = b''.join([
        pack_magic('bctr'),
        pack_u16(2),
        pack_string(val['main brlyt'], strings = strings),
        pack_string(val['bmg'], strings = strings),
        pack_string(val['picture source brlyt'], strings = strings),
    ])

    group_val = { field.name: val[field.name] for field in group_fields }
    buffer = Buffer(sum(brctr_size[field.kind] for field in group_fields))
    group_data = pack_struct(group_val, size = brctr_size, pack = brctr_pack,
                                 fields = group_fields, buffer = buffer, strings = strings)
    group_data += buffer.buffer

    buffer = Buffer(sum(brctr_size[field.kind] for field in variant_fields))
    variant_val = { field.name: val[field.name] for field in variant_fields }
    variant_data = pack_struct(variant_val, size = brctr_size, pack = brctr_pack,
                              fields = variant_fields, buffer = buffer, strings = strings)
    variant_data += buffer.buffer

    offset = 0x14
    header_data += pack_u16(offset)
    offset += len(group_data)
    header_data += pack_u16(offset)
    offset += len(variant_data)
    header_data += pack_u16(offset)
    header_data += pack_pad16(None)

    return header_data + group_data + variant_data + strings.buffer
