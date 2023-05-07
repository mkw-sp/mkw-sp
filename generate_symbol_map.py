#!/usr/bin/env python3

from argparse import ArgumentParser
from elftools.elf.elffile import ELFFile
from itanium_demangler import parse
import re

argument_parser = ArgumentParser()
argument_parser.add_argument('elf_file_path')
argument_parser.add_argument('smap_file_path')
args = argument_parser.parse_args()

with open(args.elf_file_path, 'rb') as elf_file_stream:
    symtab_section = ELFFile(elf_file_stream).get_section_by_name('.symtab')

    bad_symbol_patterns = [
        re.compile(r'\.(.*)$'),
        re.compile(r'^_GLOBAL__sub_[A-Z]_'),
        re.compile(r'^patch_'),
        re.compile(r'^replaced_'),
        re.compile(r'^replacement_'),
    ]

    dictionary = {}
    for symbol in symtab_section.iter_symbols():
        if (symbol.entry['st_value'] < 0x80000000):
            continue

        symbol_name = symbol.name
        for bad_symbol_pattern in bad_symbol_patterns:
            symbol_name = bad_symbol_pattern.sub('', symbol_name)

        try:
            demangled_symbol_name = parse(symbol_name)
            if demangled_symbol_name:
                symbol_name = demangled_symbol_name
        except:
            pass

        dictionary[symbol.entry['st_value']] = symbol_name

with open(args.smap_file_path, 'w', newline='\n') as smap_file_stream:
    for item in sorted(dictionary.items()):
        smap_file_stream.write(f"0x{item[0]:X} {item[1]}\n")
