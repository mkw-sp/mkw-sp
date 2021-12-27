from common import *


font_variants = [
    Variant('caps', 0x0), # tt_kart_font_rodan_ntlg_pro_b.brfnt
    Variant('regular', 0x1), # kart_kanji_font.brfnt / kart_font_korea.brfnt
    Variant('extension', 0x2), # tt_kart_extension_font.brfnt
    Variant('indicator', 0x3), # indicator_font.brfnt
    Variant('red', 0x4), # mario_font_number_red.brfnt
    Variant('blue', 0x5), # mario_font_number_blue.brfnt
]

def unpack_inf1(in_data, offset):
    entry_count = unpack_u16(in_data, offset + 0x08)
    entries = []
    for i in range(entry_count):
        entries += [{
            'string offset': unpack_u32(in_data, offset + 0x10 + i * 0x8),
            'font': unpack_enum8(
                in_data,
                offset + 0x10 + i * 0x8 + 0x4,
                size = size,
                unpack = unpack,
                variants = font_variants,
            ),
        }]
    return entries

def unpack_dat1(in_data, offset):
    size = unpack_u32(in_data, offset + 0x4)
    return in_data[offset + 0x8:offset + 0x8 + size]

def unpack_mid1(in_data, offset):
    entry_count = unpack_u16(in_data, offset + 0x08)
    entries = []
    for i in range(entry_count):
        entries += [unpack_u32(in_data, offset + 0x10 + i * 0x4)]
    return entries

def unpack_bmg(in_data):
    offset = 0x20
    sections = {}
    while offset < len(in_data):
        magic = unpack_magic(in_data, offset + 0x00)
        size = unpack_u32(in_data, offset + 0x04)
        if magic in sections:
            exit(f'Duplicate bmg section {magic}.')
        section = {
            'INF1': unpack_inf1,
            'DAT1': unpack_dat1,
            'MID1': unpack_mid1,
        }[magic](in_data, offset)
        sections[magic] = section
        offset += size

    messages = {}
    for index, message_id in enumerate(sections['MID1']):
        inf1 = sections['INF1']
        font = inf1[index]['font']
        string_start = inf1[index]['string offset']
        dat1 = sections['DAT1']
        offset = string_start
        while unpack_u16(dat1, offset) != 0x0:
            if unpack_u16(dat1, offset) == 0x1a:
                offset += unpack_u8(dat1, offset + 0x2)
            else:
                offset += 0x2
        string = dat1[string_start:offset].decode('utf-16-be')
        messages[hex(message_id)] = {
            'font': font,
            'string': string,
        }
    return messages

def pack_inf1(entries):
    entries_data = b''
    for entry in entries:
        entries_data += b''.join([
            pack_u32(entry['string offset']),
            pack_enum8(
                entry['font'],
                pack = pack,
                variants = font_variants,
            ),
            pack_pad24(None),
        ])

    return b''.join([
        pack_magic('INF1'),
        pack_pad32(None),
        pack_u16(len(entries)),
        pack_u16(0x8),
        pack_pad32(None),
        entries_data,
    ])

def pack_dat1(strings_data):
    return b''.join([
        pack_magic('DAT1'),
        pack_pad32(None),
        strings_data,
    ])

def pack_mid1(entries):
    entries_data = b''
    for entry in entries:
        entries_data += pack_u32(entry)

    return b''.join([
        pack_magic('MID1'),
        pack_pad32(None),
        pack_u16(len(entries)),
        pack_u8(0x10),
        pack_pad8(None),
        pack_pad32(None),
        entries_data,
    ])

def pack_bmg(messages):
    inf1 = []
    mid1 = []
    strings = Strings('utf-16-be', b'\0\0')
    for message_id in messages:
        inf1 += [{
            'string offset': strings.insert(messages[message_id]['string']),
            'font': messages[message_id]['font'],
        }]
        mid1 += [int(message_id, 16)]

    sections = {
        'INF1': inf1,
        'DAT1': strings.buffer,
        'MID1': mid1,
    }

    sections_data = b''
    for magic in sections:
        section_data = {
            'INF1': pack_inf1,
            'DAT1': pack_dat1,
            'MID1': pack_mid1,
        }[magic](sections[magic])
        section_data = section_data.ljust((len(section_data) + 0x1f) & ~0x1f, b'\0')
        section_data = section_data[0x0:0x4] + pack_u32(len(section_data)) + section_data[0x8:]
        sections_data += section_data

    return b''.join([
        pack_magic('MESG'),
        pack_magic('bmg1'),
        pack_u32(0x20 + len(sections_data)),
        pack_u32(0x3),
        pack_u8(0x2),
        pack_pad24(None),
        pack_pad32(None),
        pack_pad32(None),
        pack_pad32(None),
        sections_data,
    ])
