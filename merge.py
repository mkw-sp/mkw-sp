#!/usr/bin/env python3


from argparse import ArgumentParser
import json5


parser = ArgumentParser()
parser.add_argument('inputs', nargs = '+')
parser.add_argument('-o', '--output')
args = parser.parse_args()

messages = {}
for in_path in args.inputs:
    in_file = open(in_path, 'r', encoding = 'utf-8')
    in_data = in_file.read()
    messages = { **messages, **json5.loads(in_data) }
messages = dict(sorted(messages.items(), key = lambda item: int(item[0], 0)))
out_data = json5.dumps(messages, ensure_ascii = False, indent = 4, quote_keys = True)
out_file = open(args.output, 'w', encoding = 'utf-8')
out_file.write(out_data)
