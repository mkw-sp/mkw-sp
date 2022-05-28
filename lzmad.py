#!/usr/bin/env python3


from argparse import ArgumentParser
import lzma


parser = ArgumentParser()
parser.add_argument('in_path')
parser.add_argument('out_path')
args = parser.parse_args()

with \
    lzma.open(args.in_path, 'rb') as in_file, \
    open(args.out_path, 'wb') as out_file:
    out_file.write(in_file.read())
