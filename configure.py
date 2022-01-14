#!/usr/bin/env python3


import glob
import os
from vendor.ninja_syntax import Writer


n = Writer(open('build.ninja', 'w'))

n.variable('ninja_required_version', '1.3')
n.newline()

n.variable('builddir', 'build')
n.variable('outdir', 'out')
n.newline()

os.makedirs('build', exist_ok = True)
os.makedirs(os.path.join('build', 'assets', 'Scene', 'UI'), exist_ok = True)
alldeps = []
listfiles = []

n.variable('cc', 'powerpc-eabi-gcc')
n.variable('port', os.path.join('.', 'port.py'))
n.newline()

asflags = [
    '-isystem', 'include',
    '-isystem', 'payload',
]
cflags = [
    '-fms-extensions',
    '-fno-asynchronous-unwind-tables',
    '-fno-zero-initialized-in-bss',
    '-fplan9-extensions',
    '-fshort-wchar',
    '-isystem', 'include',
    '-isystem', 'payload',
    '-O2',
    '-Wall',
    '-Werror=implicit-function-declaration',
    '-Wextra',
]
ldflags = [
    '-nostdlib',
    '-Wl,--entry=start',
    '-Wl,--oformat,binary',
]
n.variable('asflags', ' '.join(asflags))
n.variable('cflags', ' '.join(cflags))
n.variable('ldflags', ' '.join(ldflags))
n.newline()

n.rule(
    'as',
    command = '$cc -MMD -MT $out -MF $out.d $asflags -c $in -o $out',
    depfile = '$out.d',
    deps = 'gcc',
    description = 'AS $out',
)
n.newline()

n.rule(
    'cc',
    command = '$cc -MMD -MT $out -MF $out.d $cflags -c $in -o $out',
    depfile = '$out.d',
    deps = 'gcc',
    description = 'CC $out',
)
n.newline()

n.rule(
    'port',
    command = '$port $region $in $out',
    description = 'PORT $out'
)
n.newline()

ldparams = [
    '-Wl,--defsym,base=$base',
    '-Wl,-T,$script',
]
n.rule(
    'ld',
    command = '$cc $ldflags ' + ' '.join(ldparams) + ' $in -o $out',
    description = 'LD $out',
)
n.newline()

bintargets = ['loader', 'payload']

srcfiles = {}
for target in bintargets:
    deps = []
    srcfiles[target] = []
    for ext in ['.S', '.c']:
        pattern = os.path.join(target, '**', '*' + ext)
        srcfiles[target] += glob.glob(pattern, recursive=True)
    deps += srcfiles[target]
    srcfiles[target] = [os.path.relpath(srcfile, target) for srcfile in srcfiles[target]]
    pattern = os.path.join(target, '**', '')
    srcdirs = glob.glob(pattern, recursive=True)
    srcdirs = [os.path.normpath(srcdir) for srcdir in srcdirs]
    deps += srcdirs
    alldeps += deps
    for region in ['P']:
        listfile = os.path.join('build', target + '.listfile')
        listfile_data = ' '.join(deps)
        try:
            old_data = open(listfile, 'r').read()
        except FileNotFoundError:
            old_data = None
        if listfile_data != old_data:
            open(listfile, 'w').write(listfile_data)
        listfiles += [listfile]

ofiles = {target: [] for target in srcfiles}
for target in srcfiles:
    for srcfile in srcfiles[target]:
        srcfile = os.path.join(target, srcfile)
        _, ext = os.path.splitext(srcfile)
        ofile = os.path.join('$builddir', srcfile + '.o')
        rule = {
            '.S': 'as',
            '.c': 'cc',
        }[ext]
        n.build(
            ofile,
            rule,
            srcfile,
        )
        ofiles[target] += [ofile]
    n.newline()

for region in ['P']:
    n.build(
        os.path.join('$builddir', 'scripts', f'RMC{region}.ld'),
        'port',
        os.path.join('.', 'symbols.txt'),
        variables = {
            'region': region,
        },
        implicit = '$port',
    )
    n.newline()

for target in ofiles:
    for region in ['P']:
        n.build(
            os.path.join('$outdir', 'mkw-sp', 'bin', f'{target}{region}.bin'),
            'ld',
            ofiles[target],
            variables = {
                'base': '0x80003f00' if target == 'loader' else {
                    'P': '0x8076db60',
                    'E': '0x80769400',
                    'J': '0x8076cca0',
                    'K': '0x8075bfe0',
                }[region],
                'script': os.path.join('$builddir', 'scripts', f'RMC{region}.ld'),
            },
            implicit = [
                os.path.join('$builddir', 'scripts', f'RMC{region}.ld'),
                os.path.join('build', target + '.listfile'),
            ],
        )
        n.newline()

n.variable('wuj5', os.path.join('vendor', 'wuj5', 'wuj5.py'))
n.newline()

n.rule(
    'wuj5',
    command = '$wuj5 encode $in -o $out',
    description = 'WUJ5 $out',
)
n.newline()

n.rule(
    'cp',
    command = 'cp $in $out',
    description = 'CP $out',
)
n.newline()

n.rule(
    'szs',
    command = '$wuj5 encode $szsin -o $out --retained $in',
    description = 'SZS $out',
)
n.newline()

assettargets = [
    os.path.join('Scene', 'UI', 'MenuOtherSP'),
    os.path.join('Scene', 'UI', 'MenuSingleSP'),
    os.path.join('Scene', 'UI', 'MenuSingleSP_E'),
    os.path.join('Scene', 'UI', 'RaceSP'),
    os.path.join('Scene', 'UI', 'TitleSP'),
]

assetfiles = {}
for target in assettargets:
    deps = []
    assetfiles[target] = []
    for ext in ['.json5', '.tpl']:
        pattern = os.path.join('assets', target + '.szs.d', '**', '*' + ext)
        assetfiles[target] += glob.glob(pattern, recursive=True)
    deps += assetfiles[target]
    assetfiles[target] = [os.path.relpath(assetfile, 'assets') for assetfile in assetfiles[target]]
    pattern = os.path.join('assets', target + '.szs.d', '**', '')
    assetdirs = glob.glob(pattern, recursive=True)
    assetdirs = [os.path.normpath(assetdir) for assetdir in assetdirs]
    deps += assetdirs
    alldeps += deps
    listfile = os.path.join('build', 'assets', target + '.listfile')
    listfile_data = ' '.join(deps)
    try:
        old_data = open(listfile, 'r').read()
    except FileNotFoundError:
        old_data = None
    if listfile_data != old_data:
        open(listfile, 'w').write(listfile_data)
    listfiles += [listfile]

outfiles = {target: [] for target in assetfiles}
for target in assetfiles:
    for assetfile in assetfiles[target]:
        assetfile = os.path.join('assets', assetfile)
        base, ext = os.path.splitext(assetfile)
        outext = {
            '.json5': '',
            '.tpl': '.tpl',
        }[ext]
        outfile = os.path.join('$builddir', base + outext)
        rule = {
            '.json5': 'wuj5',
            '.tpl': 'cp',
        }[ext]
        n.build(
            outfile,
            rule,
            assetfile,
        )
        outfiles[target] += [outfile]
    n.newline()

for target in outfiles:
    n.build(
        os.path.join('$outdir', 'mkw-sp', 'disc', target + '.szs'),
        'szs',
        outfiles[target],
        variables = {
            'szsin': os.path.join('$builddir', 'assets', target + '.szs.d'),
        },
        implicit = [
            os.path.join('build', 'assets', target + '.listfile'),
        ],
    )
    n.newline()

open('build.depfile', 'w').write('build.ninja: ' + ' '.join(alldeps))

n.variable('configure', os.path.join('.', 'configure.py'))
n.newline()

n.rule(
    'configure',
    command = '$configure',
    depfile = '$depfile',
    deps = 'gcc',
    generator = True,
    restat = True,
)
n.build(
    'build.ninja',
    'configure',
    variables = {
        'depfile': 'build.depfile',
    },
    implicit = [
        '$configure',
        os.path.join('vendor', 'ninja_syntax.py'),
    ],
    implicit_outputs = listfiles,
)
