#!/usr/bin/env python3


from argparse import ArgumentParser


parser = ArgumentParser()
parser.add_argument('string')
parser.add_argument('out_path')
args = parser.parse_args()

with open(args.out_path, 'w') as out_file:
    out_file.write(args.string.replace('\\n', '\n'))
