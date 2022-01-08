#!/usr/bin/env python3


from argparse import ArgumentParser
import json5
import os

from bmg import unpack_bmg, pack_bmg
from brctr import unpack_brctr, pack_brctr
from brlan import unpack_brlan, pack_brlan
from brlyt import unpack_brlyt, pack_brlyt
from u8 import unpack_u8, pack_u8
from yaz import unpack_yaz, pack_yaz


ext_unpack = {
    'bmg': unpack_bmg,
    'brctr': unpack_brctr,
    'brlan': unpack_brlan,
    'brlyt': unpack_brlyt,
}

ext_magic = {
    'bmg': b'MESG',
    'brctr': b'bctr',
    'brlan': b'RLAN',
    'brlyt': b'RLYT',
}

ext_pack = {
    'bmg': pack_bmg,
    'brctr': pack_brctr,
    'brlan': pack_brlan,
    'brlyt': pack_brlyt,
}

def decode_szs_node(out_path, node):
    out_path = os.path.join(out_path, node['name'])
    if node['is_dir']:
        os.mkdir(out_path)
        for child in node['children']:
            decode_szs_node(out_path, child)
    else:
        ext = out_path.split(os.extsep)[-1]
        in_data = node['content']
        unpack = ext_unpack.get(ext)
        if unpack is None or in_data[0:4] != ext_magic[ext]:
            out_data = in_data
            out_file = open(out_path, 'wb')
        else:
            val = unpack(in_data)
            out_data = json5.dumps(val, indent = 4, quote_keys = True)
            out_file = open(out_path + '.json5', 'w')
        out_file.write(out_data)

def decode_szs(in_path, out_path):
    in_file = open(in_path, 'rb')
    in_data = in_file.read()
    magic = in_data[0:4]
    expected_magic = b'Yaz0'
    if magic != expected_magic:
        magic = magic.decode('ascii')
        expected_magic = expected_magic.decode('ascii')
        exit(f'Unexpected magic {magic} for extension {ext} (expected {expected_magic}).')
    in_data = unpack_yaz(in_data)
    root = unpack_u8(in_data)
    if out_path is None:
        out_path = in_path + '.d'
    decode_szs_node(out_path, root)

def decode(in_path, out_path):
    ext = in_path.split(os.extsep)[-1]
    if ext == 'szs':
        decode_szs(in_path, out_path)
        return
    unpack = ext_unpack.get(ext)
    if unpack is None:
        exit(f'Unknown file format with extension {ext}.')
    in_file = open(in_path, 'rb')
    in_data = in_file.read()
    magic = in_data[0:4]
    expected_magic = ext_magic[ext]
    if magic != expected_magic:
        magic = magic.decode('ascii')
        expected_magic = expected_magic.decode('ascii')
        exit(f'Unexpected magic {magic} for extension {ext} (expected {expected_magic}).')
    val = unpack(in_data)
    out_data = json5.dumps(val, indent = 4, quote_keys = True)
    if out_path is None:
        out_path = in_path + '.json5'
    out_file = open(out_path, 'w')
    out_file.write(out_data)

def encode_szs_node(in_path):
    is_dir = os.path.isdir(in_path)
    if is_dir:
        out_path = in_path
        children = []
        for child_path in sorted(os.listdir(in_path)):
            children += [encode_szs_node(os.path.join(in_path, child_path))]
        node = {
            'children': children,
        }
    else:
        parts = in_path.split(os.extsep)
        ext = parts[-2] if len(parts) >= 2 else None
        pack = ext_pack.get(ext)
        if pack is None:
            in_file = open(in_path, 'rb')
            out_data = in_file.read()
            out_path = in_path
        else:
            in_file = open(in_path, 'r')
            in_data = in_file.read()
            val = json5.loads(in_data)
            out_data = pack(val)
            out_path = os.path.splitext(in_path)[0]
        node = {
            'content': out_data,
        }
    return {
        'is_dir': is_dir,
        'name': os.path.basename(out_path),
        **node,
    }

def encode_szs(in_path, out_path):
    root = encode_szs_node(in_path)
    out_data = pack_u8(root)
    out_data = pack_yaz(out_data)
    if out_path is None:
        out_path = os.path.splitext(in_path)[0]
    out_file = open(out_path, 'wb')
    out_file.write(out_data)

def encode(in_path, out_path):
    ext = in_path.split(os.extsep)[-2]
    if ext == 'szs':
        encode_szs(in_path, out_path)
        return
    pack = ext_pack.get(ext)
    if pack is None:
        exit(f'Unknown file format with binary extension {ext}.')
    in_file = open(in_path, 'r')
    in_data = in_file.read()
    val = json5.loads(in_data)
    out_data = pack(val)
    if out_path is None:
        out_path = os.path.splitext(in_path)[0]
    out_file = open(out_path, 'wb')
    out_file.write(out_data)


parser = ArgumentParser()
parser.add_argument('operation', choices = ['decode', 'encode'])
parser.add_argument('inputs', nargs = '+')
parser.add_argument('-o', '--outputs', nargs = '*')
args = parser.parse_args()

operations = {
    'decode': decode,
    'encode': encode,
}
if args.outputs is None:
    args.outputs = [None] * len(args.inputs)
if len(args.outputs) != len(args.inputs):
    exit('Wrong number of output paths.')
for in_path, out_path in zip(args.inputs, args.outputs):
    operations[args.operation](in_path, out_path)
