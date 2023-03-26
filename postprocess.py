#!/usr/bin/env python3


from argparse import ArgumentParser
import copy
from elftools.elf.elffile import ELFFile
import itanium_demangler
import sys


parser = ArgumentParser()
parser.add_argument('in_elf_path')
parser.add_argument('in_symbols_path')
parser.add_argument('out_symbols_path')
parser.add_argument('out_replacements_path')
args = parser.parse_args()

replaced_symbols = []
replacement_symbols = []
regular_symbols = []
with open(args.in_elf_path, 'rb') as elf_file:
    elf = ELFFile(elf_file)

    replacements_section_index = None
    for index, section in enumerate(elf.iter_sections()):
        if section.name == 'replacements':
            replacements_section_index = index

    symtab = elf.get_section_by_name('.symtab')

    for symbol in symtab.iter_symbols():
        symbol_type = symbol['st_info']['type']
        if symbol['st_shndx'] == 'SHN_UNDEF':
            demangled = itanium_demangler.parse(symbol.name)
            if demangled is not None:
                demangled = str(demangled)
            else:
                demangled = symbol.name
            if 'thunk_replaced_' not in demangled:
                continue
            demangled = demangled.replace('thunk_replaced_', '', 1)
            replaced_symbols += [(symbol.name, demangled)]
        elif symbol['st_shndx'] == replacements_section_index:
            if symbol_type != 'STT_FUNC':
                continue

            replacement_symbols += [symbol.name]
        elif symbol_type == 'STT_FUNC' or symbol_type == 'STT_OBJECT':
            regular_symbols += [symbol.name]

thunk_symbols = {}
for symbol_name, demangled in replaced_symbols:
    replacement_name = None
    for other_name in replacement_symbols:
        other_demangled = itanium_demangler.parse(other_name)
        if other_demangled is not None:
            other_demangled = str(other_demangled)
        else:
            other_demangled = other_name
        if other_demangled == demangled:
            replacement_name = other_name
            break
    if replacement_name is None:
        sys.exit(f'REPLACED was used without REPLACE for symbol {symbol.name}!')
    thunk_symbols[replacement_name] = symbol_name

backup = copy.deepcopy(replacement_symbols)
out_symbols = ''
with open(args.in_symbols_path, 'r') as in_symbols_file:
    for symbol in in_symbols_file.readlines():
        if symbol.isspace():
            out_symbols += '\n'
            continue

        address, name = symbol.split()
        address = int(address, 16)

        if name in regular_symbols:
            sys.exit(f'Multiple definitions for symbol {name}!')

        if name in replacement_symbols:
            replacement_symbols.remove(name)
            name = 'replaced_' + name
        out_symbols += f'0x{address:08x} {name}\n'
for name in replacement_symbols:
    sys.exit(f'Attempted to REPLACE {name}, but it doesn\'t exist in symbols.txt!')
replacement_symbols = backup

out_replacements = '#include <Common.h>\n'
out_replacements += '\n'
for name in replacement_symbols:
    out_replacements += f'extern void replaced_{name};\n'
    out_replacements += f'extern void {name};\n'
    if name not in thunk_symbols:
        out_replacements += f'PATCH_B(replaced_{name}, {name});\n'
    else:
        out_replacements += f'__attribute__((section("thunks"))) u32 {thunk_symbols[name]}[2];\n'
        out_replacements += f'PATCH_B_THUNK(replaced_{name}, {name}, {thunk_symbols[name]});\n'
    out_replacements += '\n'

with open(args.out_symbols_path, 'w') as out_symbols_file:
    out_symbols_file.write(out_symbols)

with open(args.out_replacements_path, 'w') as out_replacements_file:
    out_replacements_file.write(out_replacements)
