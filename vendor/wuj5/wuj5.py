#!/usr/bin/env python3


from argparse import ArgumentParser
import json5
import os

from bmg import unpack_bmg, pack_bmg
from brctr import unpack_brctr, pack_brctr
from brlan import unpack_brlan, pack_brlan
from brlyt import unpack_brlyt, pack_brlyt


def decode(in_path):
    in_file = open(in_path, 'rb')
    in_data = in_file.read()
    magic = in_data[0:4]
    unpack = {
        b'MESG': unpack_bmg,
        b'bctr': unpack_brctr,
        b'RLAN': unpack_brlan,
        b'RLYT': unpack_brlyt,
    }.get(magic)
    if unpack is None:
        exit(f'Unknown file format with magic {magic}.')
    val = unpack(in_data)
    out_data = json5.dumps(val, indent = 4, quote_keys = True)
    out_path = in_path + '.json5'
    out_file = open(out_path, 'w')
    out_file.write(out_data)

def encode(in_path):
    ext = in_path.split(os.extsep)[-2]
    pack = {
        'bmg': pack_bmg,
        'brctr': pack_brctr,
        'brlan': pack_brlan,
        'brlyt': pack_brlyt,
    }.get(ext)
    if pack is None:
        exit(f'Unknown file format with binary extension {ext}.')
    in_file = open(in_path, 'r')
    in_data = in_file.read()
    val = json5.loads(in_data)
    out_data = pack(val)
    out_path = os.path.splitext(in_path)[0]
    out_file = open(out_path, 'wb')
    out_file.write(out_data)


parser = ArgumentParser()
parser.add_argument('operation', choices = ['decode', 'encode'])
parser.add_argument('in_path')
args = parser.parse_args()

operations = {
    'decode': decode,
    'encode': encode,
}
operations[args.operation](args.in_path)
