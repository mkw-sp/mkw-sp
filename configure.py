#!/usr/bin/env python3


from argparse import ArgumentParser
import io
import os, sys
from vendor.ninja_syntax import Writer

try:
    import json5
    del json5
except ModuleNotFoundError:
    raise SystemExit("Error: pyjson5 not found. Please install it with `python -m pip install json5`")

import subprocess

parser = ArgumentParser()
parser.add_argument('--gdb_compatible', action='store_true')
args = parser.parse_args()

# https://stackoverflow.com/questions/14989858/get-the-current-git-hash-in-a-python-script/14989911#14989911
def get_git_revision_hash() -> str:
    return subprocess.check_output(['git', 'rev-parse', 'HEAD']).decode('ascii').strip()

def get_git_revision_short_hash() -> str:
    return subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode('ascii').strip()

out_buf = io.StringIO()
n = Writer(out_buf)

n.variable('ninja_required_version', '1.3')
n.newline()

n.variable('builddir', 'build')
n.variable('outdir', 'out')
n.newline()

devkitppc = os.environ.get("DEVKITPPC")
n.variable('cc', os.path.join(devkitppc, 'bin', 'powerpc-eabi-gcc'))
n.variable('cpp', os.path.join(devkitppc, 'bin', 'powerpc-eabi-g++'))
n.variable('port', 'port.py')
n.newline()

asflags = [
    '-isystem', 'include',
    '-isystem', 'payload',
    '-isystem', 'vendor',
]
cflags_loader = [
    '-fms-extensions',
    '-fno-asynchronous-unwind-tables',
    '-fplan9-extensions',
    '-fshort-wchar',
    '-isystem', 'include',
    '-isystem', 'payload',
    '-isystem', 'vendor',
    '-O2',
    '-Wall',
    '-Werror=implicit-function-declaration',
    '-Werror=incompatible-pointer-types',
    '-Wextra',
    '-Wno-packed-bitfield-compat',
    f'-DGIT_HASH={get_git_revision_short_hash()}',
]
if args.gdb_compatible:
    cflags_loader += ['-DGDB_COMPATIBLE=1']
cflags_payload = [
    *cflags_loader,
    '-fstack-protector-strong',
]
cppflags = [
    '-fms-extensions',
    '-fno-asynchronous-unwind-tables',
    # '-fplan9-extensions',
    '-fshort-wchar',
    '-fstack-protector-strong',
    '-isystem', 'include',
    '-isystem', 'payload',
    '-isystem', 'vendor',
    '-O2',
    '-Wall',
    # '-Werror=implicit-function-declaration',
    # '-Werror=incompatible-pointer-types',
    '-Wextra',
    '-Wno-packed-bitfield-compat',
    f'-DGIT_HASH="{get_git_revision_short_hash()}"',
    
    '-fno-exceptions',
    '-fno-unwind-tables',
]
ldflags = [
    '-nostdlib',
    '-Wl,--entry=start',
    '-Wl,-n',
]
n.variable('asflags', ' '.join(asflags))
n.variable('cppflags', ' '.join(cppflags))
n.variable('ldflags', ' '.join(ldflags))
n.newline()

n.rule(
    'as',
    command = '$cc -MD -MT $out -MF $out.d $asflags -c $in -o $out',
    depfile = '$out.d',
    deps = 'gcc',
    description = 'AS $out',
)
n.newline()

n.rule(
    'incbin',
    command = '$cc -DNAME=$name -DPATH=$path -c Incbin.S -o $out',
    description = 'INCBIN $out',
)
n.newline()

n.rule(
    'cc',
    command = '$cc -MD -MT $out -MF $out.d $cflags -c $in -o $out',
    depfile = '$out.d',
    deps = 'gcc',
    description = 'CC $out',
)
n.newline()

n.rule(
    'cpp',
    command = '$cpp -MD -MT $out -MF $out.d $cppflags -c $in -o $out',
    depfile = '$out.d',
    deps = 'gcc',
    description = 'CPP $out',
)
n.newline()

n.rule(
    'port',
    command = f'{sys.executable} $port $region $in $out' + (' --base' if args.gdb_compatible else ''),
    description = 'PORT $out'
)
n.newline()

ldparams = [
    '-Wl,--defsym,base=$base',
    '-Wl,--oformat,$format',
    '-Wl,-T,$script',
]
n.rule(
    'ld',
    command = '$cc $ldflags ' + ' '.join(ldparams) + ' $in -o $out',
    description = 'LD $out',
)
n.newline()

code_in_files = {
    'payload': [
        os.path.join('payload', 'egg', 'core', 'eggColorFader.c'),
        os.path.join('payload', 'egg', 'core', 'eggDisplay.S'),
        os.path.join('payload', 'egg', 'core', 'eggDvdFile.c'),
        os.path.join('payload', 'egg', 'core', 'eggEffectCreator.S'),
        os.path.join('payload', 'egg', 'core', 'eggG3dUtil.S'),
        os.path.join('payload', 'egg', 'core', 'eggHeap.c'),
        os.path.join('payload', 'egg', 'core', 'eggSystem.c'),
        os.path.join('payload', 'game', 'effect', 'Effect.S'),
        os.path.join('payload', 'game', 'gfx', 'Camera.S'),
        os.path.join('payload', 'game', 'gfx', 'CameraManager.S'),
        os.path.join('payload', 'game', 'host_system', 'BootStrapScene.c'),
        os.path.join('payload', 'game', 'host_system', 'Patcher.c'),
        os.path.join('payload', 'game', 'host_system', 'Payload.c'),
        os.path.join('payload', 'game', 'host_system', 'RkSystem.S'),
        os.path.join('payload', 'game', 'host_system', 'RkSystem.c'),
        os.path.join('payload', 'game', 'host_system', 'SceneManager.S'),
        os.path.join('payload', 'game', 'host_system', 'SceneManager.c'),
        os.path.join('payload', 'game', 'item', 'ItemObjKouraAka.S'),
        os.path.join('payload', 'game', 'item', 'ItemObjKouraAo.S'),
        os.path.join('payload', 'game', 'item', 'ItemObjKouraMidori.S'),
        os.path.join('payload', 'game', 'kart', 'KartMove.S'),
        os.path.join('payload', 'game', 'kart', 'KartObject.S'),
        os.path.join('payload', 'game', 'kart', 'KartObjectManager.S'),
        os.path.join('payload', 'game', 'kart', 'KartObjectManager.c'),
        os.path.join('payload', 'game', 'kart', 'KartParam.S'),
        os.path.join('payload', 'game', 'kart', 'KartState.S'),
        os.path.join('payload', 'game', 'kart', 'KartSub.S'),
        os.path.join('payload', 'game', 'net', 'NetManager.S'),
        os.path.join('payload', 'game', 'obj', 'ObjEffect.S'),
        os.path.join('payload', 'game', 'obj', 'ObjManager.c'),
        os.path.join('payload', 'game', 'obj', 'ObjPylon01.S'),
        os.path.join('payload', 'game', 'obj', 'ObjPylon01.c'),
        os.path.join('payload', 'game', 'race', 'Driver.S'),
        os.path.join('payload', 'game', 'race', 'DriverManager.c'),
        os.path.join('payload', 'game', 'race', 'JugemManager.S'),
        os.path.join('payload', 'game', 'race', 'JugemManager.c'),
        os.path.join('payload', 'game', 'snd', 'DriverSound.S'),
        os.path.join('payload', 'game', 'snd', 'KartSound.S'),
        os.path.join('payload', 'game', 'snd', 'Snd.S'),
        os.path.join('payload', 'game', 'system', 'BugCheck.c'),
        os.path.join('payload', 'game', 'system', 'CourseMap.S'),
        os.path.join('payload', 'game', 'system', 'DvdArchive.S'),
        os.path.join('payload', 'game', 'system', 'FatalScene.c'),
        os.path.join('payload', 'game', 'system', 'GhostFile.c'),
        os.path.join('payload', 'game', 'system', 'HomeButton.S'),
        os.path.join('payload', 'game', 'system', 'InputManager.S'),
        os.path.join('payload', 'game', 'system', 'InputManager.c'),
        os.path.join('payload', 'game', 'system', 'Mii.S'),
        os.path.join('payload', 'game', 'system', 'MultiDvdArchive.S'),
        os.path.join('payload', 'game', 'system', 'MultiDvdArchive.c'),
        os.path.join('payload', 'game', 'system', 'NandManager.S'),
        os.path.join('payload', 'game', 'system', 'RaceConfig.S'),
        os.path.join('payload', 'game', 'system', 'RaceConfig.c'),
        os.path.join('payload', 'game', 'system', 'RaceManager.S'),
        os.path.join('payload', 'game', 'system', 'RaceManager.c'),
        os.path.join('payload', 'game', 'system', 'ResourceManager.S'),
        os.path.join('payload', 'game', 'system', 'ResourceManager.c'),
        os.path.join('payload', 'game', 'system', 'SaveManager.S'),
        os.path.join('payload', 'game', 'system', 'SaveManager.c'),
        os.path.join('payload', 'game', 'system', 'SceneCreatorDynamic.S'),
        os.path.join('payload', 'game', 'system', 'SceneCreatorDynamic.c'),
        os.path.join('payload', 'game', 'ui', 'ControlLoader.S'),
        os.path.join('payload', 'game', 'ui', 'Font.S'),
        os.path.join('payload', 'game', 'ui', 'FontManager.c'),
        os.path.join('payload', 'game', 'ui', 'GhostManagerPage.S'),
        os.path.join('payload', 'game', 'ui', 'GhostManagerPage.c'),
        os.path.join('payload', 'game', 'ui', 'GhostSelectButton.c'),
        os.path.join('payload', 'game', 'ui', 'GhostSelectControl.c'),
        os.path.join('payload', 'game', 'ui', 'Layout.S'),
        os.path.join('payload', 'game', 'ui', 'License.S'),
        os.path.join('payload', 'game', 'ui', 'License.c'),
        os.path.join('payload', 'game', 'ui', 'LicenseSelectButton.c'),
        os.path.join('payload', 'game', 'ui', 'LicenseSelectPage.c'),
        os.path.join('payload', 'game', 'ui', 'LicenseSettingsPage.c'),
        os.path.join('payload', 'game', 'ui', 'Map2DRenderer.c'),
        os.path.join('payload', 'game', 'ui', 'MiiGroup.c'),
        os.path.join('payload', 'game', 'ui', 'Model.S'),
        os.path.join('payload', 'game', 'ui', 'Page.c'),
        os.path.join('payload', 'game', 'ui', 'Save.S'),
        os.path.join('payload', 'game', 'ui', 'SaveManagerProxy.S'),
        os.path.join('payload', 'game', 'ui', 'Section.c'),
        os.path.join('payload', 'game', 'ui', 'SectionManager.S'),
        os.path.join('payload', 'game', 'ui', 'SectionManager.c'),
        os.path.join('payload', 'game', 'ui', 'TabControl.c'),
        os.path.join('payload', 'game', 'ui', 'TimeAttackGhostListPage.c'),
        os.path.join('payload', 'game', 'ui', 'TimeAttackRulesPage.c'),
        os.path.join('payload', 'game', 'ui', 'TimeAttackTopPage.S'),
        os.path.join('payload', 'game', 'ui', 'TitlePage.S'),
        os.path.join('payload', 'game', 'ui', 'TitlePage.c'),
        os.path.join('payload', 'game', 'ui', 'UIAnimator.c'),
        os.path.join('payload', 'game', 'ui', 'UIControl.c'),
        os.path.join('payload', 'game', 'ui', 'Wipe.S'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlMenuBackButton.c'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlMenuInstructionText.c'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlMenuPageTitleText.c'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRace2DMap.S'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceBase.S'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceBase.c'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceInputDisplay.c'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceLap.c'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceNameBalloon.S'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceNameBalloon.c'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceSpeed.c'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceTime.S'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceTime.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'CharacterSelectPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'CourseSelectPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'CupSelectPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'DemoPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'DriftSelectPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'MachineSelectPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'RaceMenuPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'RaceMenuPage.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'RacePage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'RacePage.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'SingleTopMenuPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'SingleTopMenuPage.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'TimeAttackSplitsPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'TimeAttackSplitsPage.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'TopMenuPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'TopMenuPage.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'VsMenuPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'VsMenuPage.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'VsRulesPage.c'),
        os.path.join('payload', 'game', 'util', 'Input.S'),
        os.path.join('payload', 'nw4r', 'db', 'dbException.c'),
        os.path.join('payload', 'nw4r', 'db', 'dbException.S'),
        os.path.join('payload', 'nw4r', 'g3d', 'g3dResFile.S'),
        os.path.join('payload', 'nw4r', 'lyt', 'lyt_arcResourceAccessor.S'),
        os.path.join('payload', 'nw4r', 'lyt', 'lyt_layout.S'),
        os.path.join('payload', 'nw4r', 'snd', 'snd_DvdSoundArchive.S'),
        os.path.join('payload', 'nw4r', 'ut', 'ut_DvdFileStream.S'),
        os.path.join('payload', 'platform', 'string.c'),
        os.path.join('payload', 'platform', 'wchar.c'),
        os.path.join('payload', 'revolution', 'dvd.S'),
        os.path.join('payload', 'revolution', 'dvd.c'),
        os.path.join('payload', 'revolution', 'dvdex.c'),
        os.path.join('payload', 'revolution', 'ios.S'),
        os.path.join('payload', 'revolution', 'ios.c'),
        os.path.join('payload', 'revolution', 'nand.c'),
        os.path.join('payload', 'revolution', 'os', 'OSError.c'),
        os.path.join('payload', 'revolution', 'os', 'OSMemory.S'),
        os.path.join('payload', 'revolution', 'os', 'OSThread.S'),
        os.path.join('payload', 'revolution', 'os', 'OSThread.c'),
        os.path.join('payload', 'sp', 'Fatal.c'),
        os.path.join('payload', 'sp', 'FatStorage.c'),
        os.path.join('payload', 'sp', 'FlameGraph.c'),
        os.path.join('payload', 'sp', 'Host.c'),
        os.path.join('payload', 'sp', 'IOSDolphin.c'),
        os.path.join('payload', 'sp', 'IOSKeyboard.c'),
        os.path.join('payload', 'sp', 'Keyboard.c'),
        os.path.join('payload', 'sp', 'LogFile.c'),
        os.path.join('payload', 'sp', 'Memory.c'),
        os.path.join('payload', 'sp', 'Net.c'),
        os.path.join('payload', 'sp', 'NetStorage.c'),
        os.path.join('payload', 'sp', 'NetStorageClient.c'),
        os.path.join('payload', 'sp', 'Sdi.c'),
        os.path.join('payload', 'sp', 'Slab.c'),
        os.path.join('payload', 'sp', 'Stack.c'),
        os.path.join('payload', 'sp', 'StackTrace.c'),
        os.path.join('payload', 'sp', 'Storage.c'),
        os.path.join('payload', 'sp', 'Tcp.c'),
        os.path.join('payload', 'sp', 'Usb.c'),
        os.path.join('payload', 'sp', 'UsbStorage.c'),
        os.path.join('payload', 'sp', 'Yaz.c'),
        os.path.join('vendor', 'arith64.c'),
        os.path.join('vendor', 'ff', 'diskio.c'),
        os.path.join('vendor', 'ff', 'ff.c'),
        os.path.join('vendor', 'ff', 'fftime.c'),
        os.path.join('vendor', 'ff', 'ffunicode.c'),
    ],
    'loader': [
        os.path.join('loader', 'Apploader.c'),
        os.path.join('loader', 'Cache.S'),
        os.path.join('loader', 'Delay.S'),
        os.path.join('loader', 'Di.c'),
        os.path.join('loader', 'Ios.c'),
        os.path.join('loader', 'Loader.c'),
        os.path.join('loader', 'Memcpy.c'),
        os.path.join('loader', 'Memset.c'),
        os.path.join('loader', 'Stack.c'),
        os.path.join('loader', 'Start.S'),
        os.path.join('loader', 'Strlen.c'),
    ],
}
code_out_files = {target: [] for target in code_in_files}
for target in code_in_files:
    for in_file in code_in_files[target]:
        _, ext = os.path.splitext(in_file)
        out_file = os.path.join('$builddir', in_file + '.o')
        rule = {
            '.S': 'as',
            '.c': 'cc',
            '.cpp': 'cpp',
        }[ext]
        n.build(
            out_file,
            rule,
            in_file,
            variables = {
                'cflags': ' '.join(cflags_loader) if target == 'loader' else ' '.join(cflags_payload),
            },
        )
        code_out_files[target] += [out_file]
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

for region in ['P', 'E', 'J', 'K']:
    for fmt in ['binary', 'elf32-powerpc']:
        n.build(
            os.path.join('$builddir', 'bin', f'payload{region}.' + ('bin' if fmt == 'binary' else 'elf')),
            'ld',
            code_out_files['payload'],
            variables = {
                'base': {
                    'P': '0x8076db60' if not args.gdb_compatible else '0x809C4FA0',
                    'E': '0x80769400',
                    'J': '0x8076cca0',
                    'K': '0x8075bfe0',
                }[region],
                'format': fmt,
                'script': os.path.join('$builddir', 'scripts', f'RMC{region}.ld'),
            },
            implicit = os.path.join('$builddir', 'scripts', f'RMC{region}.ld'),
        )
        n.newline()

for region in ['P', 'E', 'J', 'K']:
    out_file = os.path.join('$builddir', 'loader', f'payload{region}.o')
    n.build(
        out_file,
        'incbin',
        os.path.join('$builddir', 'bin', f'payload{region}.bin'),
        variables = {
            'name': f'payload{region}',
            'path': '/'.join(['$builddir', 'bin', f'payload{region}.bin']),
        },
        implicit = 'Incbin.S',
    )
    code_out_files['loader'] += [out_file]

n.build(
    os.path.join('$outdir', 'boot.elf'),
    'ld',
    code_out_files['loader'],
    variables = {
        'base': '0x80910000' if not args.gdb_compatible else '0x80E50F90',
        'format': 'elf32-powerpc',
        'script': os.path.join('loader', 'RMC.ld'),
    },
    implicit = os.path.join('loader', 'RMC.ld'),
)
n.newline()

n.variable('merge', os.path.join('.', 'merge.py'))
n.variable('wuj5', os.path.join('vendor', 'wuj5', 'wuj5.py'))
n.newline()

n.rule(
    'merge',
    command = f'{sys.executable} $merge $in -o $out',
    description = 'MERGE $out',
)
n.newline()

n.rule(
    'wuj5',
    command = f'{sys.executable} $wuj5 encode $in -o $out',
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
    command = f'{sys.executable} $wuj5 encode $szsin -o $out --retained $in $args',
    description = 'SZS $out',
)
n.newline()

LANGUAGES = [
    'E', # English (PAL)
    'F', # French (PAL)
    'G', # German
    'I', # Italian
    'J', # Japanese
    'K', # Korean
    'M', # Spanish (NTSC)
    'Q', # French (NTSC)
    'S', # Spanish (PAL)
    'U', # English (NTSC)
    'N', # Dutch
]
HUD_LANGUAGES = {
    'E': 'E',
    'F': 'F',
    'G': 'G',
    'I': 'I',
    'J': 'E',
    'K': 'E',
    'M': 'S',
    'Q': 'F',
    'S': 'S',
    'U': 'E',
    'N': 'N',
}

asset_in_files = {
    os.path.join('Scene', 'UI', 'CrashSP.szs'): [
        os.path.join('fatal', 'blyt', 'Fatal.brlyt'),
        os.path.join('fatal', 'font', 'sf_light_i8_utf16.brfnt'),
        os.path.join('fatal', 'font', 'sf_medium_basic.brfnt'),
    ],
    os.path.join('Scene', 'UI', 'FontSP.szs'): [
        os.path.join('kart_font_K.brfnt'),
        os.path.join('kart_font_R.brfnt'),
        os.path.join('tt_kart_font_rodan_ntlg_pro_b_K.brfnt'),
        os.path.join('tt_kart_font_rodan_ntlg_pro_b_R.brfnt'),
    ],
    os.path.join('Scene', 'UI', 'MenuOtherSP.szs'): [
        os.path.join('button', 'ctrl', 'LicenseManagementButton.brctr.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_fade_in_after.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_fade_in_before.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_fade_in.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_fade_out.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_fuchi_check_loop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_01_ok.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_01_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_02_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_active_off.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_active_off_to_on.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_active_on.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_fade_in_after.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_fade_in_before.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_fade_in.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_fade_out.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_fuchi_check_loop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_text_light_01_ok.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_text_light_01_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_text_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_text_light_02_stop.brlan.json5'),
        os.path.join('control', 'blyt', 'common_w024_rule_icon.brlyt.json5'),
        os.path.join('control', 'blyt', 'common_w076_license_icon_center.brlyt.json5'),
        os.path.join('control', 'blyt', 'common_w201_setting_menu.brlyt.json5'),
        os.path.join('control', 'ctrl', 'LicenseDisplay.brctr.json5'),
        os.path.join('control', 'ctrl', 'LicenseManagement.brctr.json5'),
        os.path.join('control', 'ctrl', 'LicenseSettingRadioBase.brctr.json5'),
        os.path.join('control', 'ctrl', 'LicenseSettingRadioOption.brctr.json5'),
        os.path.join('control', 'timg', 'tt_license_icon_004.tpl'),
    ],
    os.path.join('Scene', 'UI', 'MenuSingleSP.szs'): [
        os.path.join('button', 'blyt', 'common_w129_movie_button_single_top.brlyt.json5'),
        os.path.join('button', 'ctrl', 'SingleTop.brctr.json5'),
        os.path.join('button', 'ctrl', 'TimeAttackGhostListArrowLeft.brctr.json5'),
        os.path.join('button', 'ctrl', 'TimeAttackGhostListArrowRight.brctr.json5'),
        os.path.join('button', 'ctrl', 'TimeAttackGhostList.brctr.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_active_off.brlan.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_active_off_to_on.brlan.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_active_on.brlan.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_light_01_ok.brlan.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_light_01_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_light_02_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w200_ghost_button_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'friend_room_comment_container_center_to_right.brlan.json5'),
        os.path.join('control', 'anim', 'friend_room_comment_container_hide.brlan.json5'),
        os.path.join('control', 'anim', 'friend_room_comment_container_left_to_center.brlan.json5'),
        os.path.join('control', 'anim', 'friend_room_comment_container_show.brlan.json5'),
        os.path.join('control', 'blyt', 'common_w200_ghost_button.brlyt.json5'),
        os.path.join('control', 'blyt', 'ghost_container.brlyt.json5'),
        os.path.join('control', 'ctrl', 'GhostSelectBase.brctr.json5'),
        os.path.join('control', 'ctrl', 'GhostSelectOption.brctr.json5'),
        os.path.join('control', 'ctrl', 'TASettingRadioBase.brctr.json5'),
        os.path.join('control', 'ctrl', 'TASettingRadioOption.brctr.json5'),
        os.path.join('control', 'ctrl', 'TimeAttackGhostListPageNum.brctr.json5'),
        os.path.join('control', 'ctrl', 'VSSettingRadioOption.brctr.json5'),
    ],
    os.path.join('Scene', 'UI', 'RaceSP.szs'): [
        os.path.join('button', 'blyt', 'common_w202_menu_compact.brlyt.json5'),
        os.path.join('button', 'ctrl', 'AfterMenuBT.brctr.json5'),
        os.path.join('button', 'ctrl', 'AfterMenuBTLast.brctr.json5'),
        os.path.join('button', 'ctrl', 'AfterMenuEndConfirm.brctr.json5'),
        os.path.join('button', 'ctrl', 'AfterMenuTimeAttack.brctr.json5'),
        os.path.join('button', 'ctrl', 'AfterMenuVS.brctr.json5'),
        os.path.join('button', 'ctrl', 'AfterMenuVSLast.brctr.json5'),
        os.path.join('button', 'ctrl', 'PauseMenuGhostWatch.brctr.json5'),
        os.path.join('button', 'ctrl', 'PauseMenuReplayTA.brctr.json5'),
        os.path.join('button', 'ctrl', 'PauseMenuTimeAttack.brctr.json5'),
        os.path.join('button', 'ctrl', 'PauseMenuVS.brctr.json5'),
        # For in-race licence settings editor
        os.path.join('control', 'anim', 'common_w023_rule_menu_fade_in_after.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_fade_in_before.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_fade_in.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_fade_out.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_fuchi_check_loop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_01_ok.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_01_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_02_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_active_off.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_active_off_to_on.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_active_on.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_fade_in_after.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_fade_in_before.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_fade_in.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_fade_out.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_fuchi_check_loop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_text_light_01_ok.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_text_light_01_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_text_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w024_rule_icon_text_light_02_stop.brlan.json5'),
        os.path.join('control', 'blyt', 'common_w024_rule_icon.brlyt.json5'),
        os.path.join('control', 'blyt', 'common_w076_license_icon_center.brlyt.json5'),
        os.path.join('control', 'blyt', 'common_w201_setting_menu.brlyt.json5'),
        os.path.join('control', 'ctrl', 'LicenseDisplay.brctr.json5'),
        os.path.join('control', 'ctrl', 'LicenseManagement.brctr.json5'),
        os.path.join('control', 'ctrl', 'LicenseSettingRadioBase.brctr.json5'),
        os.path.join('control', 'ctrl', 'LicenseSettingRadioOption.brctr.json5'),
        os.path.join('control', 'timg', 'tt_license_icon_004.tpl'),
        os.path.join('game_image', 'anim', 'game_image_speed_texture_pattern_0_9.brlan.json5'),
        os.path.join('game_image', 'blyt', 'game_image_speed.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'InputDisplay.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'race_message_half.brlyt'),
        os.path.join('game_image', 'ctrl', 'battle_total_point.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'InputDisplay.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'lap_number.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'position_multi.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'speed_number.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'time_number.brctr.json5'),
        os.path.join('game_image', 'timg', 'basic_accel_off.tpl'),
        os.path.join('game_image', 'timg', 'basic_accel_on.tpl'),
        os.path.join('game_image', 'timg', 'basic_cstick_bg.tpl'),
        os.path.join('game_image', 'timg', 'basic_cstick_center.tpl'),
        os.path.join('game_image', 'timg', 'basic_dpad_down.tpl'),
        os.path.join('game_image', 'timg', 'basic_dpad_left.tpl'),
        os.path.join('game_image', 'timg', 'basic_dpad_off.tpl'),
        os.path.join('game_image', 'timg', 'basic_dpad_right.tpl'),
        os.path.join('game_image', 'timg', 'basic_dpad_up.tpl'),
        os.path.join('game_image', 'timg', 'basic_trigger_bd_off.tpl'),
        os.path.join('game_image', 'timg', 'basic_trigger_bd_on.tpl'),
        os.path.join('game_image', 'timg', 'basic_trigger_l_off.tpl'),
        os.path.join('game_image', 'timg', 'basic_trigger_l_on.tpl'),
        os.path.join('game_image', 'timg', 'basic_trigger_r_off.tpl'),
        os.path.join('game_image', 'timg', 'basic_trigger_r_on.tpl'),
        os.path.join('game_image', 'timg', 'tt_d_number_3d_minus.tpl'),
        os.path.join('game_image', 'timg', 'tt_d_number_3d_none.tpl'),
    ],
    os.path.join('Scene', 'UI', 'TitleSP.szs'): [
        os.path.join('button', 'blyt', 'common_w076_license_icon_center.brlyt.json5'),
        os.path.join('button', 'ctrl', 'LicenseSelect.brctr.json5'),
        os.path.join('button', 'ctrl', 'TopMenuMultiWaku.brctr.json5'),
        os.path.join('button', 'ctrl', 'TopMenuSingleWaku.brctr.json5'),
        os.path.join('button', 'timg', 'tt_license_icon_004.tpl'),
    ],
}
for language in LANGUAGES:
    hud_language = HUD_LANGUAGES[language]
    if language != 'K':
        asset_in_files[os.path.join('Race', f'Common_{language}.szs')] = [
            os.path.join(f'jugemu_lap_{language}.brres'),
            os.path.join(f'jugemu_lapf_{language}.brres'),
        ]
    asset_in_files[os.path.join('Scene', 'UI', f'AwardSP_{language}.szs')] = [
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
        os.path.join('message', f'Race_{language}.bmg.json5'),
    ]
    asset_in_files[os.path.join('Scene', 'UI', f'MenuMultiSP_{language}.szs')] = [
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
    ]
    asset_in_files[os.path.join('Scene', 'UI', f'MenuOtherSP_{language}.szs')] = [
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
    ]
    asset_in_files[os.path.join('Scene', 'UI', f'MenuSingleSP_{language}.szs')] = [
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
    ]
    asset_in_files[os.path.join('Scene', 'UI', f'RaceSP_{language}.szs')] = [
        os.path.join('game_image', 'timg', f'tt_speed_{hud_language}.tpl'),
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
        os.path.join('message', f'Race_{language}.bmg.json5'),
    ]
    if hud_language != 'E':
        asset_in_files[os.path.join('Scene', 'UI', f'RaceSP_{language}.szs')] += [
            os.path.join('game_image', 'timg', f'tt_lap_{hud_language}.tpl'),
            os.path.join('game_image', 'timg', f'tt_lap_{hud_language}_lap1.tpl'),
            os.path.join('game_image', 'timg', f'tt_lap_{hud_language}_lap2.tpl'),
            os.path.join('game_image', 'timg', f'tt_lap_{hud_language}_lap3.tpl'),
            os.path.join('game_image', 'timg', f'tt_time_{hud_language}.tpl'),
        ]
        for i in range(12):
            for base in ['tt_position_no_st_64x64', 'tt_multi_position_no_st_64x64']:
                asset_in_files[os.path.join('Scene', 'UI', f'RaceSP_{language}.szs')] += [
                    os.path.join('game_image', 'timg', f'{base}_{hud_language}_{i + 1:02d}.tpl')
                ]
    asset_in_files[os.path.join('Scene', 'UI', f'TitleSP_{language}.szs')] = [
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
    ]
asset_out_files = {target: [] for target in asset_in_files}
for target in asset_in_files:
    for in_file in asset_in_files[target]:
        base, ext = os.path.splitext(in_file)
        outext = {
            '.brfna': '.brfna',
            '.brfnt': '.brfnt',
            '.brlyt': '.brlyt',
            '.brres': '.brres',
            '.json5': '',
            '.tpl': '.tpl',
        }[ext]
        out_file = os.path.join('$builddir', 'Shared.szs.d', base + outext)
        basebase, baseext = os.path.splitext(base)
        out_files = [out_file for out_files in asset_out_files.values() for out_file in out_files]
        if baseext == '.bmg':
            merged_file = os.path.join('$builddir', 'merged', in_file)
            if out_file not in out_files:
                n.build(
                    merged_file,
                    'merge',
                    [
                        os.path.join('assets', in_file),
                        os.path.join('assets', basebase.rsplit('_', 1)[0] + 'SP_U.bmg.json5'),
                        os.path.join('assets', basebase.replace('_', 'SP_') + '.bmg.json5'),
                    ],
                    implicit = '$merge',
                )
                in_file = merged_file
        else:
            in_file = os.path.join('assets', in_file)
        if out_file not in out_files:
            rule = {
                '.brfna': 'cp',
                '.brfnt': 'cp',
                '.brlyt': 'cp',
                '.brres': 'cp',
                '.json5': 'wuj5',
                '.tpl': 'cp',
            }[ext]
            n.build(
                out_file,
                rule,
                in_file,
            )
        asset_out_files[target] += [out_file]
n.newline()

renamed = {}
for language in LANGUAGES:
    renamed[f'jugemu_lap_{language}.brres'] = 'jugemu_lap.brres'
    renamed[f'jugemu_lapf_{language}.brres'] = 'jugemu_lapf.brres'
    renamed[f'Common_{language}.bmg'] = 'Common.bmg'
    renamed[f'Menu_{language}.bmg'] = 'Menu.bmg'
    renamed[f'Race_{language}.bmg'] = 'Race.bmg'
for hud_language in HUD_LANGUAGES.values():
    renamed[f'tt_lap_{hud_language}.tpl'] = 'tt_lap_E.tpl'
    renamed[f'tt_lap_{hud_language}_lap1.tpl'] = 'tt_lap_E_Lap1.tpl'
    renamed[f'tt_lap_{hud_language}_lap2.tpl'] = 'tt_lap_E_lap2.tpl'
    renamed[f'tt_lap_{hud_language}_lap3.tpl'] = 'tt_lap_E_lap3.tpl'
    renamed[f'tt_speed_{hud_language}.tpl'] = 'tt_speed.tpl'
    renamed[f'tt_time_{hud_language}.tpl'] = 'tt_time_E.tpl'
    for i in range(12):
        for base in ['tt_position_no_st_64x64', 'tt_multi_position_no_st_64x64']:
            renamed[f'{base}_{hud_language}_{i + 1:02d}.tpl'] = f'{base}_{i + 1:02d}.tpl'
for target in asset_out_files:
    target_renamed = {}
    for out_file in asset_out_files[target]:
        out_file = os.path.basename(out_file)
        if out_file in renamed:
            target_renamed[out_file] = renamed[out_file]
    target_renamed = ' '.join([f'--renamed {src} {dst}' for src, dst in target_renamed.items()])
    n.build(
        os.path.join('$outdir', 'disc', target),
        'szs',
        asset_out_files[target],
        variables = {
            'szsin': os.path.join('$builddir', 'Shared.szs.d'),
            'args': target_renamed,
        },
    )
    n.newline()

n.variable('configure', 'configure.py')
n.newline()

n.rule(
    'configure',
    command = f'{sys.executable} $configure' + (' --gdb_compatible' if args.gdb_compatible else ''),
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

with open('build.ninja', 'w') as out_file:
    out_file.write(out_buf.getvalue())
n.close()
