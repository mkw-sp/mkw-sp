#!/usr/bin/env python3


from argparse import ArgumentParser
import struct
import sys


MAJOR = 0
MINOR = 1
PATCH = 5


def write_u16(out_file, val):
    out_file.write(struct.pack('>H', val))

def write_type(out_file, val):
    val = {
        'debug': 0,
        'test': 1,
        'release': 2,
    }[val]
    write_u16(out_file, val)

def write_string(out_file, val):
    out_file.write(val.encode('ascii') + b'\0')

parser = ArgumentParser()
parser.add_argument('type', choices = ['debug', 'test', 'release'])
parser.add_argument('out_path')
args = parser.parse_args()

with open(args.out_path, 'wb') as out_file:
    write_type(out_file, args.type)
    write_u16(out_file, MAJOR)
    write_u16(out_file, MINOR)
    write_u16(out_file, PATCH)
    out_file.write(b'\0' * 0x18) # Reserved bytes
    string = f'{MAJOR}.{MINOR}.{PATCH} ({args.type[:1].upper() + args.type[1:]})'
    write_string(out_file, string)
    size = out_file.tell()
    if size > 0x60:
        sys.exit('Too long')
    out_file.write(b'\0' * (0x60 - size))
