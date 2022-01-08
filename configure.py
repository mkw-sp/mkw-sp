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

srcdirs = {}
for target in bintargets:
    pattern = os.path.join(target, '**', '')
    srcdirs[target] = glob.glob(pattern, recursive=True)
    srcdirs[target] = [os.path.normpath(srcdir) for srcdir in srcdirs[target]]

srcfiles = {}
for target in bintargets:
    srcfiles[target] = []
    for ext in ['.S', '.c']:
        pattern = os.path.join(target, '**', '*' + ext)
        srcfiles[target] += glob.glob(pattern, recursive=True)
    srcfiles[target] = [os.path.relpath(srcfile, target) for srcfile in srcfiles[target]]
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

for region in ['P', 'E', 'J', 'K']:
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
    for region in ['P', 'E', 'J', 'K']:
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
            implicit = os.path.join('$builddir', 'scripts', f'RMC{region}.ld'),
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
    command = '$wuj5 encode $szsin -o $out',
    description = 'SZS $out',
)
n.newline()

assettargets = [
    os.path.join('Scene', 'UI', 'MenuSingleSP'),
    os.path.join('Scene', 'UI', 'MenuSingleSP_E'),
    os.path.join('Scene', 'UI', 'RaceSP'),
]

assetdirs = {}
for target in assettargets:
    pattern = os.path.join('assets', target + '.szs.d', '**', '')
    assetdirs[target] = glob.glob(pattern, recursive=True)
    assetdirs[target] = [os.path.normpath(assetdir) for assetdir in assetdirs[target]]

assetfiles = {}
for target in assettargets:
    assetfiles[target] = []
    for ext in ['.json5', '.tpl']:
        pattern = os.path.join('assets', target + '.szs.d', '**', '*' + ext)
        assetfiles[target] += glob.glob(pattern, recursive=True)
    assetfiles[target] = [os.path.relpath(assetfile, 'assets') for assetfile in assetfiles[target]]
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
        variables = {
            'szsin': os.path.join('$builddir', 'assets', target + '.szs.d'),
        },
        implicit = outfiles[target],
    )
    n.newline()

n.variable('configure', os.path.join('.', 'configure.py'))
n.newline()

n.rule(
    'configure',
    command = '$configure',
    generator = True,
)
n.build(
    'build.ninja',
    'configure',
    implicit = [
        '$configure',
        os.path.join('vendor', 'ninja_syntax.py'),
        *sum(srcdirs.values(), []),
        *sum(assetdirs.values(), []),
    ],
)
