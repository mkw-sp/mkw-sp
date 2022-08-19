import sys

from common import *


font_variants = [
    Variant('caps', 0x0), # tt_kart_font_rodan_ntlg_pro_b.brfnt
    Variant('regular', 0x1), # kart_kanji_font.brfnt / kart_font_korea.brfnt
    Variant('extension', 0x2), # tt_kart_extension_font.brfnt
    Variant('indicator', 0x3), # indicator_font.brfnt
    Variant('red', 0x4), # mario_font_number_red.brfnt
    Variant('blue', 0x5), # mario_font_number_blue.brfnt
]

tag_variants = [
    # The font scale as an integer percentage.
    Variant('font scale', 0x08000000),
    # The color of the text.
    Variant('color', 0x08000001),
    # 1 unicode character.
    Variant('1 char', 0x08010000),
    # The current player's Mii name, or "Player" if no license is selected.
    Variant('current player', 0x06020000),
    # From `intVals` and with a number of digits.
    Variant('arg integer', 0x0a020010),
    # From `messageId`.
    Variant('arg message', 0x08020011),
    # From `miis`.
    Variant('arg player', 0x08020012),
    # From `licenseIds`.
    Variant('arg license player', 0x08020013),
    # From `playerIds`.
    Variant('arg border', 0x08020014),
    # From `playerIds`, looks unused.
    Variant('arg question mark', 0x08020015),
    # From `intVals` and with a number of digits.
    Variant('arg signed integer', 0x0a020016),
    # From `playerIds`.
    Variant('arg back button', 0x8020017),
    # From `strings`.
    Variant('arg string', 0x8020020),
    # 2 unicode characters.
    Variant('2 chars', 0x0e030000),
    # From `playerIds`.
    Variant('arg front button', 0x08030010),
    # From `intVals`, second message if 1, otherwise first.
    Variant('arg cond messages', 0x0c040000),
]

color_variants = [
    Variant('unspecified', 0x0),
    Variant('transparent', 0x8),
    Variant('yor 0', 0x10),
    Variant('yor 1', 0x11),
    Variant('yor 2', 0x12),
    Variant('yor 3', 0x13),
    Variant('yor 4', 0x14),
    Variant('yor 5', 0x15),
    Variant('yor 6', 0x16),
    Variant('yor 7', 0x17),
    Variant('team red', 0x20),
    Variant('team blue', 0x21),
    Variant('player 1', 0x30),
    Variant('player 2', 0x31),
    Variant('player 3', 0x32),
    Variant('player 4', 0x33),
    Variant('red', 0x40),
    Variant('sp green', 0x50),
    Variant('sp blue', 0x51),
    Variant('sp pink', 0x52),
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
            sys.exit(f'Duplicate bmg section {magic}.')
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
        if offset == 0x0:
            string = None
        else:
            string = ''
            while unpack_u16(dat1, offset) != 0x0:
                if unpack_u16(dat1, offset) == 0x1a:
                    tag = unpack_enum32(
                        dat1,
                        offset + 0x2,
                        size = size,
                        unpack = unpack,
                        variants = tag_variants,
                    )
                    if tag == 'color':
                        val = unpack_enum16(
                            dat1,
                            offset + 0x6,
                            size = size,
                            unpack = unpack,
                            variants = color_variants,
                        )
                    elif tag == '1 char':
                        val = dat1[offset + 0x6:offset + 0x6 + 0x2].decode('utf-16-be')
                    elif tag == 'current player':
                        val = ''
                    elif tag == 'arg integer' or tag == 'arg signed integer':
                        index = unpack_u16(dat1, offset + 0x6)
                        digits = unpack_u16(dat1, offset + 0x8)
                        val = f'{index} {digits}'
                    elif tag == '2 chars':
                        c0 = dat1[offset + 0x6:offset + 0x6 + 0x4].decode('utf-16-be')
                        c1 = dat1[offset + 0xa:offset + 0xa + 0x4].decode('utf-16-be')
                        val = f'{c0} {c1}'
                    elif tag == 'arg cond messages':
                        index = unpack_u16(dat1, offset + 0x6)
                        m0 = unpack_u16(dat1, offset + 0x8)
                        m1 = unpack_u16(dat1, offset + 0xa)
                        val = f'{index} {m0} {m1}'
                    else:
                        val = unpack_u16(dat1, offset + 0x6)
                    string += f'{{{tag}|{val}}}'
                    offset += unpack_u8(dat1, offset + 0x2)
                else:
                    string += dat1[offset:offset + 0x2].decode('utf-16-be')
                    offset += 0x2
        messages[message_id] = {
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
    strings = Buffer(0x0)
    strings.buffer = b'\0\0'
    for message_id in messages:
        in_string = messages[message_id]['string']
        if in_string is None:
            string_offset = 0x0
        else:
            out_string = b''
            parts = in_string.split('{', maxsplit = 1)
            while len(parts) > 1:
                out_string += parts[0].encode('utf-16-be')
                pattern, in_string = parts[1].split('}', maxsplit = 1)
                out_string += pack_u16(0x1a)
                tag, val = pattern.split('|')
                out_string += pack_enum32(
                    tag,
                    pack = pack,
                    variants = tag_variants,
                )
                if tag == 'color':
                    out_string += pack_enum16(
                        val,
                        pack = pack,
                        variants = color_variants,
                    )
                elif tag == '1 char':
                    out_string += val.encode('utf-16-be')
                elif tag == 'arg integer' or tag == 'arg signed integer':
                    index, digits = val.split(' ')
                    out_string += pack_u16(int(index))
                    out_string += pack_u16(int(digits))
                elif tag == '2 chars':
                    c0, c1 = val.split(' ')
                    out_string += c0.encode('utf-16-be')
                    out_string += c1.encode('utf-16-be')
                elif tag == 'arg cond messages':
                    index, m0, m1 = val.split(' ')
                    out_string += pack_u16(int(index))
                    out_string += pack_u16(int(m0))
                    out_string += pack_u16(int(m1))
                elif tag != 'current player':
                    out_string += pack_u16(int(val))
                parts = in_string.split('{', maxsplit = 1)
            out_string += in_string.encode('utf-16-be')
            out_string += b'\0\0'
            string_offset = strings.push(out_string)
        inf1 += [{
            'string offset': string_offset,
            'font': messages[message_id]['font'],
        }]
        mid1 += [int(message_id, 0)]

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
