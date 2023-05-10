#!/usr/bin/env python3


from argparse import ArgumentParser
import json5
import lzma
import os
import struct
import sys

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

def decode_u8_node(out_path, node, retained, renamed):
    name = node['name']
    if name in renamed:
        name = renamed[name]
    out_path = os.path.join(out_path, name)
    if node['is_dir']:
        os.mkdir(out_path)
        for child in node['children']:
            decode_u8_node(out_path, child, retained, renamed)
    else:
        if retained is not None and out_path not in retained:
            return
        ext = out_path.split(os.extsep)[-1]
        in_data = node['content']
        unpack = ext_unpack.get(ext)
        if unpack is None or in_data[0:4] != ext_magic[ext]:
            out_data = in_data
            with open(out_path, 'wb') as out_file:
                out_file.write(out_data)
        else:
            val = unpack(in_data)
            out_data = json5.dumps(val, indent = 4, quote_keys = True)
            with open(out_path + '.json5', 'w', encoding = 'utf-8') as out_file:
                out_file.write(out_data)

def decode_u8(in_path, out_path, retained, renamed):
    with open(in_path, 'rb') as in_file:
        in_data = in_file.read()
    magic = in_data[0:4]
    ext = in_path.split(os.extsep)[-1]
    if ext != 'lzma':
        expected_magic = {
            'arc': b'U\xaa8-',
            'szs': b'Yaz0',
        }[ext]
        if magic != expected_magic:
            magic = magic.decode('ascii')
            expected_magic = expected_magic.decode('ascii')
            sys.exit(f'Unexpected magic {magic} for extension {ext} (expected {expected_magic}).')
    if ext == 'szs':
        in_data = unpack_yaz(in_data)
    elif ext == 'lzma':
        in_data = lzma.decompress(in_data)
    root = unpack_u8(in_data)
    if out_path is None:
        out_path = in_path + '.d'
    decode_u8_node(out_path, root, retained, renamed)

def decode(in_path, out_path, retained, renamed):
    if in_path.endswith('.arc') or in_path.endswith('.szs') or in_path.endswith('.arc.lzma'):
        decode_u8(in_path, out_path, retained, renamed)
        return
    ext = in_path.split(os.extsep)[-1]
    unpack = ext_unpack.get(ext)
    if unpack is None:
        sys.exit(f'Unknown file format with extension {ext}.')
    with open(in_path, 'rb') as in_file:
        in_data = in_file.read()
    magic = in_data[0:4]
    expected_magic = ext_magic[ext]
    if magic != expected_magic:
        magic = magic.decode('ascii')
        expected_magic = expected_magic.decode('ascii')
        sys.exit(f'Unexpected magic {magic} for extension {ext} (expected {expected_magic}).')
    val = unpack(in_data)
    out_data = json5.dumps(val, ensure_ascii = False, indent = 4, quote_keys = True)
    if out_path is None:
        out_path = in_path + '.json5'
    with open(out_path, 'w', encoding = 'utf-8') as out_file:
        out_file.write(out_data)

def encode_u8_node(in_path, retained, renamed):
    is_dir = os.path.isdir(in_path)
    if is_dir:
        if retained is not None and not any(r.startswith(in_path) for r in retained):
            return None
        out_path = in_path
        children = []
        for child_path in sorted(os.listdir(in_path)):
            child = encode_u8_node(os.path.join(in_path, child_path), retained, renamed)
            if child is not None:
                children += [child]
        node = {
            'children': children,
        }
    else:
        if retained is not None and in_path not in retained:
            return None
        parts = in_path.split(os.extsep)
        ext = parts[-2] if len(parts) >= 2 else None
        pack = ext_pack.get(ext)
        if pack is None:
            with open(in_path, 'rb') as in_file:
                out_data = in_file.read()
            out_path = in_path
        else:
            with open(in_path, 'r', encoding = 'utf-8') as in_file:
                in_data = in_file.read()
            val = json5.loads(in_data)
            out_data = pack(val)
            out_path = os.path.splitext(in_path)[0]
        node = {
            'content': out_data,
        }
    name = os.path.basename(out_path)
    if name in renamed:
        name = renamed[name]
    return {
        'is_dir': is_dir,
        'name': name,
        **node,
    }

def encode_u8(in_path, out_path, retained, renamed):
    ext = in_path.split(os.extsep)[-2]
    root = encode_u8_node(in_path, retained, renamed)
    out_data = pack_u8(root)
    if ext == 'szs':
        out_data = pack_yaz(out_data)
    elif ext == 'lzma':
        out_data = lzma.compress(out_data, lzma.FORMAT_ALONE)
    if out_path is None:
        out_path = os.path.splitext(in_path)[0]
    with open(out_path, 'wb') as out_file:
        out_file.write(out_data)

def encode(in_path, out_path, retained, renamed):
    if in_path.endswith('.arc.d') or in_path.endswith('.szs.d') or in_path.endswith('.arc.lzma.d'):
        encode_u8(in_path, out_path, retained, renamed)
        return
    ext = in_path.split(os.extsep)[-2]
    pack = ext_pack.get(ext)
    if pack is None:
        sys.exit(f'Unknown file format with binary extension {ext}.')
    with open(in_path, 'r', encoding = 'utf-8') as in_file:
        in_data = in_file.read()
    val = json5.loads(in_data)
    out_data = pack(val)
    if out_path is None:
        out_path = os.path.splitext(in_path)[0]
    with open(out_path, 'wb') as out_file:
        out_file.write(out_data)


parser = ArgumentParser()
parser.add_argument('operation', choices = ['decode', 'encode'])
parser.add_argument('inputs', nargs = '+')
parser.add_argument('-o', '--outputs', nargs = '*')
parser.add_argument('--retained', nargs = '*')
parser.add_argument('--renamed', action = 'append', nargs = 2)
args = parser.parse_args()

operations = {
    'decode': decode,
    'encode': encode,
}
if args.outputs is None:
    args.outputs = [None] * len(args.inputs)
if len(args.outputs) != len(args.inputs):
    sys.exit('Wrong number of output paths.')
renamed = {}
if args.renamed is not None:
    renamed = {src: dst for src, dst in args.renamed}
for in_path, out_path in zip(args.inputs, args.outputs):
    operations[args.operation](in_path, out_path, args.retained, renamed)
