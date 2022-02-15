#!/usr/bin/env python3


import struct


file = open('mem2.raw', 'rb')
data = file.read()
base = 0x977c0000 - 0x90000000
data = data[base:base + 0x800000]
hits = {0x80000000 + i * 0x4: hits[0] for i, hits in enumerate(struct.iter_unpack('>I', data))}
hits = dict(reversed(sorted(hits.items(), key = lambda item: item[1])))
for address, hits in hits.items():
    if hits != 0:
        print(hex(address), hits)
