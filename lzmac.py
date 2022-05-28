#!/usr/bin/env python3


from argparse import ArgumentParser
import lzma


parser = ArgumentParser()
parser.add_argument('in_path')
parser.add_argument('out_path')
args = parser.parse_args()

with \
    open(args.in_path, 'rb') as in_file, \
    lzma.open(args.out_path, 'wb', format = lzma.FORMAT_ALONE) as out_file:
    out_file.write(in_file.read())
