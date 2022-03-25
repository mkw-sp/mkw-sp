from common import *


def unpack_yaz(in_data):
    in_size = len(in_data)
    in_offset = 0x10
    out_size = unpack_u32(in_data, 0x4)
    out_data = bytearray(out_size)
    out_offset = 0
    i = 0
    while in_offset < in_size and out_offset < out_size:
        if i == 0:
            group_header = unpack_u8(in_data, in_offset)
            in_offset += 0x1
        if (group_header >> (7 - i) & 0x1):
            out_data[out_offset] = in_data[in_offset]
            in_offset += 0x1
            out_offset += 0x1
        else:
            val = unpack_u16(in_data, in_offset)
            in_offset += 0x2
            ref_offset = out_offset - (val & 0xfff) - 0x1
            ref_size = (val >> 12) + 0x2
            if ref_size == 0x2:
                ref_size = unpack_u8(in_data, in_offset) + 0x12
                in_offset += 0x1
            for _ in range(ref_size):
                out_data[out_offset] = out_data[ref_offset]
                ref_offset += 0x1
                out_offset += 0x1
        i = (i + 1) % 8
    assert(out_offset == out_size)
    return out_data

def pack_yaz(in_data):
    in_size = len(in_data)
    in_offset = 0x0

    out_data = bytearray()

    i = 0
    patterns = {}
    while in_offset < in_size:
        if i == 0:
            group_header_offset = len(out_data)
            out_data += b'\0'
        pattern = in_data[in_offset:in_offset + 0x3]
        ref_offsets = patterns.get(pattern, set())
        best_ref_size = 0x1
        for ref_offset in ref_offsets:
            ref_size = 0x3
            max_ref_size = min(in_size - in_offset, 0x111)
            if best_ref_size < max_ref_size:
                if in_data[in_offset + best_ref_size] != in_data[ref_offset + best_ref_size]:
                    continue
                while ref_size < max_ref_size:
                    if in_data[in_offset + ref_size] != in_data[ref_offset + ref_size]:
                        break
                    ref_size += 0x1
                if ref_size > best_ref_size:
                    best_ref_size = ref_size
                    best_ref_offset = ref_offset
                    if best_ref_size == 0x111:
                        break
            else:
                break
        if best_ref_size < 0x3:
            best_ref_size = 0x1
            out_data[group_header_offset] |= 1 << (7 - i)
            out_data += pack_u8(in_data[in_offset])
        elif best_ref_size < 0x12:
            val = (best_ref_size - 0x2) << 12 | (in_offset - best_ref_offset - 0x1)
            out_data += pack_u16(val)
        else:
            best_ref_size = min(best_ref_size, 0x111)
            out_data += pack_u16(in_offset - best_ref_offset - 1)
            out_data += pack_u8(best_ref_size - 0x12)
        next_in_offset = in_offset + best_ref_size
        while in_offset < next_in_offset:
            if in_offset >= 0x1000:
                pattern = in_data[in_offset - 0x1000:in_offset - 0x1000 + 0x3]
                ref_offsets = patterns[pattern]
                ref_offsets.remove(in_offset - 0x1000)
            pattern = in_data[in_offset:in_offset + 0x3]
            ref_offsets = patterns.setdefault(pattern, set())
            ref_offsets.add(in_offset)
            in_offset += 0x1
        i = (i + 1) % 8

    return b''.join([
        pack_magic('Yaz0'),
        pack_u32(in_size),
        pack_pad32(None),
        pack_pad32(None),
        out_data
    ])
