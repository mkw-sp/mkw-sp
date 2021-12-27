#!/usr/bin/env python3


import os
from vendor.ninja_syntax import Writer


n = Writer(open('build.ninja', 'w'))

n.variable('ninja_required_version', '1.3')
n.newline()

n.variable('builddir', 'build')
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

sourcefiles = {
    'loader': [
        os.path.join('Loader.c'),
    ],
    'payload': [
        os.path.join('game', 'gfx', 'Camera.S'),
        os.path.join('game', 'gfx', 'CameraManager.S'),
        os.path.join('game', 'host_system', 'BootStrapScene.c'),
        os.path.join('game', 'host_system', 'Patcher.c'),
        os.path.join('game', 'host_system', 'Payload.c'),
        os.path.join('game', 'host_system', 'RkSystem.c'),
        os.path.join('game', 'host_system', 'SceneManager.S'),
        os.path.join('game', 'snd', 'Snd.S'),
        os.path.join('game', 'system', 'GhostFile.c'),
        os.path.join('game', 'system', 'InputManager.S'),
        os.path.join('game', 'system', 'InputManager.c'),
        os.path.join('game', 'system', 'MultiDvdArchive.c'),
        os.path.join('game', 'system', 'NandHelper.c'),
        os.path.join('game', 'system', 'NandManager.S'),
        os.path.join('game', 'system', 'RaceConfig.S'),
        os.path.join('game', 'system', 'RaceConfig.c'),
        os.path.join('game', 'system', 'RaceManager.S'),
        os.path.join('game', 'system', 'RaceManager.c'),
        os.path.join('game', 'system', 'ResourceManager.c'),
        os.path.join('game', 'system', 'SaveManager.c'),
        os.path.join('game', 'ui', 'GhostManagerPage.c'),
        os.path.join('game', 'ui', 'GhostSelectButton.c'),
        os.path.join('game', 'ui', 'GhostSelectControl.c'),
        os.path.join('game', 'ui', 'Section.c'),
        os.path.join('game', 'ui', 'SectionManager.c'),
        os.path.join('game', 'ui', 'TabControl.c'),
        os.path.join('game', 'ui', 'TimeAttackGhostListPage.c'),
        os.path.join('game', 'ui', 'UIAnimator.c'),
        os.path.join('game', 'ui', 'UIControl.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlMenuBackButton.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlMenuPageTitleText.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRace2DMap.S'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceBase.S'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceNameBalloon.S'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceNameBalloon.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceSpeed.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceTime.S'),
        os.path.join('game', 'ui', 'page', 'RacePage.S'),
        os.path.join('game', 'ui', 'page', 'RacePage.c'),
        os.path.join('nw4r', 'lyt', 'lyt_arcResourceAccessor.S'),
        os.path.join('nw4r', 'lyt', 'lyt_layout.S'),
        os.path.join('revolution', 'nand.c'),
    ],
}
ofiles = {target: [] for target in sourcefiles}

for target in sourcefiles:
    for sourcefile in sourcefiles[target]:
        sourcefile = os.path.join(target, sourcefile)
        _, ext = os.path.splitext(sourcefile)
        ofile = os.path.join('$builddir', sourcefile + '.o')
        rule = {
            '.S': 'as',
            '.c': 'cc',
        }[ext]
        n.build(
            ofile,
            rule,
            sourcefile,
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
            os.path.join('$builddir', f'{target}{region}.bin'),
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
    ],
)
