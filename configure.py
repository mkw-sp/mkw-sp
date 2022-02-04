#!/usr/bin/env python3


import os
from vendor.ninja_syntax import Writer

try:
    import json5
    del json5
except ModuleNotFoundError:
    raise SystemExit("Error: pyjson5 not found. Please install it with `python -m pip install json5`")

n = Writer(open('build.ninja', 'w'))

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
]
cflags = [
    '-fms-extensions',
    '-fno-asynchronous-unwind-tables',
    '-fplan9-extensions',
    '-fshort-wchar',
    '-isystem', 'include',
    '-isystem', 'payload',
    '-O2',
    '-Wall',
    '-Werror=implicit-function-declaration',
    '-Werror=incompatible-pointer-types',
    '-Wextra',
    '-Wno-packed-bitfield-compat',
]
cppflags = [
    '-fms-extensions',
    '-fno-asynchronous-unwind-tables',
    # '-fplan9-extensions',
    '-fshort-wchar',
    '-isystem', 'include',
    '-isystem', 'payload',
    '-O2',
    '-Wall',
    # '-Werror=implicit-function-declaration',
    # '-Werror=incompatible-pointer-types',
    '-Wextra',
    '-Wno-packed-bitfield-compat',
    
    '-fno-exceptions',
    '-fno-unwind-tables',
]
ldflags = [
    '-nostdlib',
    '-Wl,--entry=start',
    '-Wl,--oformat,binary',
]
n.variable('asflags', ' '.join(asflags))
n.variable('cflags', ' '.join(cflags))
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
    command = 'python $port $region $in $out',
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

code_in_files = {
    'loader': [
        os.path.join('Loader.c'),
    ],
    'payload': [
        os.path.join('egg', 'core', 'eggHeap.c'),
        os.path.join('egg', 'core', 'eggVideo.S'),
        os.path.join('egg', 'core', 'eggSystem.c'),
        os.path.join('game', 'effect', 'Effect.S'),
        os.path.join('game', 'gfx', 'Camera.S'),
        os.path.join('game', 'gfx', 'CameraManager.S'),
        os.path.join('game', 'host_system', 'BootStrapScene.c'),
        os.path.join('game', 'host_system', 'Patcher.c'),
        os.path.join('game', 'host_system', 'Payload.c'),
        os.path.join('game', 'host_system', 'RkSystem.c'),
        os.path.join('game', 'host_system', 'SceneManager.S'),
        os.path.join('game', 'host_system', 'SceneManager.c'),
        os.path.join('game', 'item', 'ItemObjKouraAka.S'),
        os.path.join('game', 'item', 'ItemObjKouraAo.S'),
        os.path.join('game', 'item', 'ItemObjKouraMidori.S'),
        os.path.join('game', 'kart', 'KartMove.S'),
        os.path.join('game', 'kart', 'KartObject.S'),
        os.path.join('game', 'kart', 'KartObjectManager.S'),
        os.path.join('game', 'kart', 'KartObjectManager.c'),
        os.path.join('game', 'kart', 'KartParam.S'),
        os.path.join('game', 'kart', 'KartState.S'),
        os.path.join('game', 'kart', 'KartSub.S'),
        os.path.join('game', 'obj', 'ObjManager.c'),
        os.path.join('game', 'obj', 'ObjPylon01.S'),
        os.path.join('game', 'obj', 'ObjPylon01.c'),
        os.path.join('game', 'race', 'Driver.S'),
        os.path.join('game', 'race', 'DriverManager.c'),
        os.path.join('game', 'snd', 'Snd.S'),
        os.path.join('game', 'system', 'CourseMap.S'),
        os.path.join('game', 'system', 'DvdArchive.S'),
        os.path.join('game', 'system', 'GhostFile.c'),
        os.path.join('game', 'system', 'HomeButton.S'),
        os.path.join('game', 'system', 'InputManager.S'),
        os.path.join('game', 'system', 'InputManager.c'),
        os.path.join('game', 'system', 'Mii.S'),
        os.path.join('game', 'system', 'MultiDvdArchive.S'),
        os.path.join('game', 'system', 'MultiDvdArchive.c'),
        os.path.join('game', 'system', 'NandHelper.c'),
        os.path.join('game', 'system', 'NandManager.S'),
        os.path.join('game', 'system', 'RaceConfig.S'),
        os.path.join('game', 'system', 'RaceConfig.c'),
        os.path.join('game', 'system', 'RaceManager.S'),
        os.path.join('game', 'system', 'RaceManager.c'),
        os.path.join('game', 'system', 'ResourceManager.S'),
        os.path.join('game', 'system', 'ResourceManager.c'),
        os.path.join('game', 'system', 'SaveManager.S'),
        os.path.join('game', 'system', 'SaveManager.c'),
        os.path.join('game', 'system', 'Yaz.c'),
        os.path.join('game', 'ui', 'GhostManagerPage.S'),
        os.path.join('game', 'ui', 'GhostManagerPage.c'),
        os.path.join('game', 'ui', 'GhostSelectButton.c'),
        os.path.join('game', 'ui', 'GhostSelectControl.c'),
        os.path.join('game', 'ui', 'License.S'),
        os.path.join('game', 'ui', 'License.c'),
        os.path.join('game', 'ui', 'LicenseSelectButton.c'),
        os.path.join('game', 'ui', 'LicenseSelectPage.c'),
        os.path.join('game', 'ui', 'LicenseSettingsPage.c'),
        os.path.join('game', 'ui', 'Map2DRenderer.c'),
        os.path.join('game', 'ui', 'MiiGroup.c'),
        os.path.join('game', 'ui', 'SaveManagerProxy.S'),
        os.path.join('game', 'ui', 'Section.S'),
        os.path.join('game', 'ui', 'Section.c'),
        os.path.join('game', 'ui', 'SectionManager.S'),
        os.path.join('game', 'ui', 'SectionManager.c'),
        os.path.join('game', 'ui', 'TabControl.c'),
        os.path.join('game', 'ui', 'TimeAttackGhostListPage.c'),
        os.path.join('game', 'ui', 'TimeAttackRulesPage.c'),
        os.path.join('game', 'ui', 'TimeAttackTopPage.S'),
        os.path.join('game', 'ui', 'TitlePage.S'),
        os.path.join('game', 'ui', 'TitlePage.c'),
        os.path.join('game', 'ui', 'UIAnimator.c'),
        os.path.join('game', 'ui', 'UIControl.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlMenuBackButton.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlMenuInstructionText.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlMenuPageTitleText.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRace2DMap.S'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceBase.S'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceBase.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceLap.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceNameBalloon.S'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceNameBalloon.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceSpeed.c'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceTime.S'),
        os.path.join('game', 'ui', 'ctrl', 'CtrlRaceTime.c'),
        os.path.join('game', 'ui', 'page', 'CharacterSelectPage.S'),
        os.path.join('game', 'ui', 'page', 'CourseSelectPage.S'),
        os.path.join('game', 'ui', 'page', 'CupSelectPage.S'),
        os.path.join('game', 'ui', 'page', 'DriftSelectPage.S'),
        os.path.join('game', 'ui', 'page', 'RaceMenuPage.S'),
        os.path.join('game', 'ui', 'page', 'RaceMenuPage.c'),
        os.path.join('game', 'ui', 'page', 'RacePage.S'),
        os.path.join('game', 'ui', 'page', 'RacePage.c'),
        os.path.join('game', 'ui', 'page', 'SingleTopMenuPage.S'),
        os.path.join('game', 'ui', 'page', 'SingleTopMenuPage.c'),
        os.path.join('game', 'ui', 'page', 'TimeAttackSplitsPage.S'),
        os.path.join('game', 'ui', 'page', 'TimeAttackSplitsPage.c'),
        os.path.join('game', 'ui', 'page', 'TopMenuPage.S'),
        os.path.join('game', 'ui', 'page', 'TopMenuPage.c'),
        os.path.join('game', 'ui', 'page', 'VsMenuPage.S'),
        os.path.join('game', 'ui', 'page', 'VsMenuPage.c'),
        os.path.join('game', 'util', 'Input.S'),
        os.path.join('nw4r', 'lyt', 'lyt_arcResourceAccessor.S'),
        os.path.join('nw4r', 'lyt', 'lyt_layout.S'),
        os.path.join('nw4r', 'snd', 'snd_DvdSoundArchive.S'),
        os.path.join('revolution', 'nand.c'),
        os.path.join('sp', 'Fatal.c'),
        os.path.join('sp', 'IOSKeyboard.c'),
        os.path.join('sp', 'Keyboard.c'),
    ],
}
code_out_files = {target: [] for target in code_in_files}
for target in code_in_files:
    for in_file in code_in_files[target]:
        in_file = os.path.join(target, in_file)
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

n.build(
    os.path.join('$outdir', f'loader.bin'),
    'ld',
    code_out_files['loader'],
    variables = {
        'base': '0x80004000',
        'script': os.path.join('loader', 'RMC.ld'),
    },
    implicit = os.path.join('loader', 'RMC.ld'),
)
n.newline()

for region in ['P', 'E', 'J', 'K']:
    n.build(
        os.path.join('$outdir', 'disc', 'bin', f'payload{region}.bin'),
        'ld',
        code_out_files['payload'],
        variables = {
            'base': {
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
    command = 'python $wuj5 encode $in -o $out',
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
    command = 'python $wuj5 encode $szsin -o $out --retained $in $args',
    description = 'SZS $out',
)
n.newline()

asset_in_files = {
    'MultSP_E.szs': [
        os.path.join('message', 'Menu_E.bmg.json5'),
    ],
    'MultSP_F.szs': [
        os.path.join('message', 'Menu_F.bmg.json5'),
    ],
    'MultSP_G.szs': [
        os.path.join('message', 'Menu_G.bmg.json5'),
    ],
    'MultSP_I.szs': [
        os.path.join('message', 'Menu_I.bmg.json5'),
    ],
    'MultSP_J.szs': [
        os.path.join('message', 'Menu_J.bmg.json5'),
    ],
    'MultSP_K.szs': [
        os.path.join('message', 'Menu_K.bmg.json5'),
    ],
    'MultSP_M.szs': [
        os.path.join('message', 'Menu_M.bmg.json5'),
    ],
    'MultSP_Q.szs': [
        os.path.join('message', 'Menu_Q.bmg.json5'),
    ],
    'MultSP_S.szs': [
        os.path.join('message', 'Menu_S.bmg.json5'),
    ],
    'MultSP_U.szs': [
        os.path.join('message', 'Menu_U.bmg.json5'),
    ],
    'OthrSP.szs': [
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
    'OthrSP_E.szs': [
        os.path.join('message', 'Menu_E.bmg.json5'),
    ],
    'OthrSP_F.szs': [
        os.path.join('message', 'Menu_F.bmg.json5'),
    ],
    'OthrSP_G.szs': [
        os.path.join('message', 'Menu_G.bmg.json5'),
    ],
    'OthrSP_I.szs': [
        os.path.join('message', 'Menu_I.bmg.json5'),
    ],
    'OthrSP_J.szs': [
        os.path.join('message', 'Menu_J.bmg.json5'),
    ],
    'OthrSP_K.szs': [
        os.path.join('message', 'Menu_K.bmg.json5'),
    ],
    'OthrSP_M.szs': [
        os.path.join('message', 'Menu_M.bmg.json5'),
    ],
    'OthrSP_Q.szs': [
        os.path.join('message', 'Menu_Q.bmg.json5'),
    ],
    'OthrSP_S.szs': [
        os.path.join('message', 'Menu_S.bmg.json5'),
    ],
    'OthrSP_U.szs': [
        os.path.join('message', 'Menu_U.bmg.json5'),
    ],
    'SnglSP.szs': [
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
    ],
    'SnglSP_E.szs': [
        os.path.join('message', 'Menu_E.bmg.json5'),
    ],
    'SnglSP_F.szs': [
        os.path.join('message', 'Menu_F.bmg.json5'),
    ],
    'SnglSP_G.szs': [
        os.path.join('message', 'Menu_G.bmg.json5'),
    ],
    'SnglSP_I.szs': [
        os.path.join('message', 'Menu_I.bmg.json5'),
    ],
    'SnglSP_J.szs': [
        os.path.join('message', 'Menu_J.bmg.json5'),
    ],
    'SnglSP_K.szs': [
        os.path.join('message', 'Menu_K.bmg.json5'),
    ],
    'SnglSP_M.szs': [
        os.path.join('message', 'Menu_M.bmg.json5'),
    ],
    'SnglSP_Q.szs': [
        os.path.join('message', 'Menu_Q.bmg.json5'),
    ],
    'SnglSP_S.szs': [
        os.path.join('message', 'Menu_S.bmg.json5'),
    ],
    'SnglSP_U.szs': [
        os.path.join('message', 'Menu_U.bmg.json5'),
    ],
    'RaceSP.szs': [
        os.path.join('button', 'ctrl', 'AfterMenuTimeAttack.brctr.json5'),
        os.path.join('game_image', 'anim', 'game_image_speed_texture_pattern_0_9.brlan.json5'),
        os.path.join('game_image', 'blyt', 'game_image_speed.brlyt.json5'),
        os.path.join('game_image', 'ctrl', 'battle_total_point.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'lap_number.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'position_multi.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'speed_number.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'time_number.brctr.json5'),
        os.path.join('game_image', 'timg', 'tt_d_number_3d_minus.tpl'),
        os.path.join('game_image', 'timg', 'tt_d_number_3d_none.tpl'),
    ],
    'RaceSP_E.szs': [
        os.path.join('game_image', 'timg', 'tt_speed_E.tpl'),
        os.path.join('message', 'Menu_E.bmg.json5'),
        os.path.join('message', 'Race_E.bmg.json5'),
    ],
    'RaceSP_F.szs': [
        os.path.join('game_image', 'timg', 'tt_speed_F.tpl'),
        os.path.join('message', 'Menu_F.bmg.json5'),
        os.path.join('message', 'Race_F.bmg.json5'),
    ],
    'RaceSP_G.szs': [
        os.path.join('game_image', 'timg', 'tt_speed_G.tpl'),
        os.path.join('message', 'Menu_G.bmg.json5'),
        os.path.join('message', 'Race_G.bmg.json5'),
    ],
    'RaceSP_I.szs': [
        os.path.join('game_image', 'timg', 'tt_speed_I.tpl'),
        os.path.join('message', 'Menu_I.bmg.json5'),
        os.path.join('message', 'Race_I.bmg.json5'),
    ],
    'RaceSP_J.szs': [
        os.path.join('game_image', 'timg', 'tt_speed_E.tpl'),
        os.path.join('message', 'Menu_J.bmg.json5'),
        os.path.join('message', 'Race_J.bmg.json5'),
    ],
    'RaceSP_K.szs': [
        os.path.join('game_image', 'timg', 'tt_speed_E.tpl'),
        os.path.join('message', 'Menu_K.bmg.json5'),
        os.path.join('message', 'Race_K.bmg.json5'),
    ],
    'RaceSP_M.szs': [
        os.path.join('game_image', 'timg', 'tt_speed_S.tpl'),
        os.path.join('message', 'Menu_M.bmg.json5'),
        os.path.join('message', 'Race_M.bmg.json5'),
    ],
    'RaceSP_Q.szs': [
        os.path.join('game_image', 'timg', 'tt_speed_F.tpl'),
        os.path.join('message', 'Menu_Q.bmg.json5'),
        os.path.join('message', 'Race_Q.bmg.json5'),
    ],
    'RaceSP_S.szs': [
        os.path.join('game_image', 'timg', 'tt_speed_S.tpl'),
        os.path.join('message', 'Menu_S.bmg.json5'),
        os.path.join('message', 'Race_S.bmg.json5'),
    ],
    'RaceSP_U.szs': [
        os.path.join('game_image', 'timg', 'tt_speed_E.tpl'),
        os.path.join('message', 'Menu_U.bmg.json5'),
        os.path.join('message', 'Race_U.bmg.json5'),
    ],
    'TitlSP.szs': [
        os.path.join('button', 'blyt', 'common_w076_license_icon_center.brlyt.json5'),
        os.path.join('button', 'ctrl', 'LicenseSelect.brctr.json5'),
        os.path.join('button', 'ctrl', 'TopMenuMultiWaku.brctr.json5'),
        os.path.join('button', 'ctrl', 'TopMenuSingleWaku.brctr.json5'),
        os.path.join('button', 'timg', 'tt_license_icon_004.tpl'),
    ],
    'TitlSP_E.szs': [
        os.path.join('message', 'Menu_E.bmg.json5'),
    ],
    'TitlSP_F.szs': [
        os.path.join('message', 'Menu_F.bmg.json5'),
    ],
    'TitlSP_G.szs': [
        os.path.join('message', 'Menu_G.bmg.json5'),
    ],
    'TitlSP_I.szs': [
        os.path.join('message', 'Menu_I.bmg.json5'),
    ],
    'TitlSP_J.szs': [
        os.path.join('message', 'Menu_J.bmg.json5'),
    ],
    'TitlSP_K.szs': [
        os.path.join('message', 'Menu_K.bmg.json5'),
    ],
    'TitlSP_M.szs': [
        os.path.join('message', 'Menu_M.bmg.json5'),
    ],
    'TitlSP_Q.szs': [
        os.path.join('message', 'Menu_Q.bmg.json5'),
    ],
    'TitlSP_S.szs': [
        os.path.join('message', 'Menu_S.bmg.json5'),
    ],
    'TitlSP_U.szs': [
        os.path.join('message', 'Menu_U.bmg.json5'),
    ],
}
asset_out_files = {target: [] for target in asset_in_files}
for target in asset_in_files:
    for in_file in asset_in_files[target]:
        base, ext = os.path.splitext(in_file)
        outext = {
            '.json5': '',
            '.tpl': '.tpl',
        }[ext]
        out_file = os.path.join('$builddir', 'Shared.szs.d', base + outext)
        in_file = os.path.join('assets', in_file)
        out_files = [out_file for out_files in asset_out_files.values() for out_file in out_files]
        if out_file not in out_files:
            rule = {
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

renamed = {
    'Menu_E.bmg': 'Menu.bmg',
    'Menu_F.bmg': 'Menu.bmg',
    'Menu_G.bmg': 'Menu.bmg',
    'Menu_I.bmg': 'Menu.bmg',
    'Menu_J.bmg': 'Menu.bmg',
    'Menu_K.bmg': 'Menu.bmg',
    'Menu_M.bmg': 'Menu.bmg',
    'Menu_Q.bmg': 'Menu.bmg',
    'Menu_S.bmg': 'Menu.bmg',
    'Menu_U.bmg': 'Menu.bmg',
    'Race_E.bmg': 'Race.bmg',
    'Race_F.bmg': 'Race.bmg',
    'Race_G.bmg': 'Race.bmg',
    'Race_I.bmg': 'Race.bmg',
    'Race_J.bmg': 'Race.bmg',
    'Race_K.bmg': 'Race.bmg',
    'Race_M.bmg': 'Race.bmg',
    'Race_Q.bmg': 'Race.bmg',
    'Race_S.bmg': 'Race.bmg',
    'Race_U.bmg': 'Race.bmg',
    'tt_speed_E.tpl': 'tt_speed.tpl',
    'tt_speed_F.tpl': 'tt_speed.tpl',
    'tt_speed_G.tpl': 'tt_speed.tpl',
    'tt_speed_I.tpl': 'tt_speed.tpl',
    'tt_speed_S.tpl': 'tt_speed.tpl',
}
renamed = ' '.join([f'--renamed {src} {dst}' for src, dst in renamed.items()])
for target in asset_out_files:
    n.build(
        os.path.join('$outdir', 'disc', 'Scene', 'UI', target),
        'szs',
        asset_out_files[target],
        variables = {
            'szsin': os.path.join('$builddir', 'Shared.szs.d'),
            'args': renamed,
        },
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
