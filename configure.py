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

out_buf = io.StringIO()
n = Writer(out_buf)

n.variable('ninja_required_version', '1.3')
n.newline()

n.variable('builddir', 'build')
n.variable('outdir', 'out')
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
    'arc',
    command = f'{sys.executable} $wuj5 encode $arcin -o $out --retained $in $args',
    description = 'ARC $out',
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
    os.path.join('Scene', 'UI', 'ChannelSP.arc.lzma'): [
        os.path.join('bg', 'ctrl', 'MenuObiTitleTextOption.brctr.json5'),
        os.path.join('button', 'ctrl', 'ServicePackTopButton.brctr.json5'),
        os.path.join('button', 'ctrl', 'StorageBenchmarkButton.brctr.json5'),
    ],
    os.path.join('Scene', 'UI', 'CrashSP.arc.lzma'): [
        os.path.join('fatal', 'blyt', 'Fatal.brlyt'),
        os.path.join('fatal', 'font', 'sf_light_i8_utf16.brfnt'),
        os.path.join('fatal', 'font', 'sf_medium_basic.brfnt'),
    ],
    os.path.join('Scene', 'UI', 'FontSP_K.arc.lzma'): [
        os.path.join('kart_font_korea.brfnt'),
        os.path.join('tt_kart_font_rodan_ntlg_pro_b_K.brfnt'),
    ],
    os.path.join('Scene', 'UI', 'FontSP_R.arc.lzma'): [
        os.path.join('kart_kanji_font.brfnt'),
        os.path.join('tt_kart_font_rodan_ntlg_pro_b_R.brfnt'),
    ],
    os.path.join('Scene', 'UI', 'MenuMultiSP.arc.lzma'): [
        # 200cc
        os.path.join('control', 'ctrl', 'VSSettingRadioOption.brctr.json5'),
    ],
    os.path.join('Scene', 'UI', 'MenuOtherSP.arc.lzma'): [
        # License display and license management
        os.path.join('button', 'ctrl', 'LicenseManagementButton.brctr.json5'),
        os.path.join('control', 'blyt', 'common_w076_license_icon_center.brlyt.json5'),
        os.path.join('control', 'ctrl', 'LicenseDisplay.brctr.json5'),
        os.path.join('control', 'ctrl', 'LicenseManagement.brctr.json5'),
        os.path.join('control', 'timg', 'tt_license_icon_004.tpl'),
        # Settings
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_02_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_chara_light_01_ok.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_chara_light_01_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_chara_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_chara_light_02_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_fuchi_check_loop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_chara_light_01_ok.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_chara_light_01_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_chara_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_chara_light_02_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_fuchi_check_loop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy0_loop.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy0_onetime.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy1_loop.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy1_onetime.brlan.json5'),
        os.path.join('control', 'blyt', 'common_w134_rule_rap_yaji_l.brlyt.json5'),
        os.path.join('control', 'blyt', 'common_w135_rule_rap_yaji_r.brlyt.json5'),
        os.path.join('control', 'blyt', 'common_w201_setting_menu.brlyt.json5'),
        os.path.join('control', 'blyt', 'gr_area_select_menu.brlyt.json5'),
        os.path.join('control', 'ctrl', 'CategoryUpDownBase.brctr.json5'),
        os.path.join('control', 'ctrl', 'CategoryUpDownButtonL.brctr.json5'),
        os.path.join('control', 'ctrl', 'CategoryUpDownButtonR.brctr.json5'),
        os.path.join('control', 'ctrl', 'SettingUpDownBase.brctr.json5'),
        os.path.join('control', 'ctrl', 'VSSettingUpDownButtonL.brctr.json5'),
        os.path.join('control', 'ctrl', 'VSSettingUpDownButtonR.brctr.json5'),
        os.path.join('control', 'timg', 'tt_yajirushi_type1.tpl'),
        os.path.join('ranking', 'anim', 'area_select_window_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_free_to_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_fuchi_check_loop.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_select_to_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_center_to_right.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_hide.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_left_to_center.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_01_ok.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_01_stop.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_02_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_02_stop.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_stop.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_free_to_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_select_to_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_text_center_to_right.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_text_hide.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_text_left_to_center.brlan.json5'),
        os.path.join('ranking', 'blyt', 'area_select_window.brlyt.json5'),
        os.path.join('ranking', 'blyt', 'area_select_window_text.brlyt.json5'),
        os.path.join('ranking', 'blyt', 'category_window.brlyt.json5'),
        os.path.join('ranking', 'blyt', 'category_window_text.brlyt.json5'),
        os.path.join('ranking', 'ctrl', 'CategoryUpDownText.brctr.json5'),
        os.path.join('ranking', 'ctrl', 'CategoryUpDownValue.brctr.json5'),
        os.path.join('ranking', 'ctrl', 'SettingUpDownText.brctr.json5'),
        os.path.join('ranking', 'ctrl', 'SettingUpDownValue.brctr.json5'),
        os.path.join('ranking', 'timg', 'tt_pattern_chek_alpha_nasi_32x32.tpl'),
        os.path.join('ranking', 'timg', 'tt_pattern_chek_bokashi_32x32.tpl'),
    ],
    os.path.join('Scene', 'UI', 'MenuSingleSP.arc.lzma'): [
        # GP removal
        os.path.join('button', 'blyt', 'common_w129_movie_button_single_top.brlyt.json5'),
        os.path.join('button', 'blyt', 'common_w129_movie_button_single_top_no_mr.brlyt.json5'),
        os.path.join('button', 'ctrl', 'MissionInstruction.brctr.json5'),
        os.path.join('button', 'ctrl', 'SingleTop.brctr.json5'),
        os.path.join('button', 'ctrl', 'SingleTopNoMR.brctr.json5'),
        # Ghost list
        os.path.join('button', 'blyt', 'common_w203_launch_button_half.brlyt.json5'),
        os.path.join('button', 'ctrl', 'TimeAttackGhostListArrowLeft.brctr.json5'),
        os.path.join('button', 'ctrl', 'TimeAttackGhostListArrowRight.brctr.json5'),
        os.path.join('button', 'ctrl', 'TimeAttackGhostList.brctr.json5'),
        os.path.join('button', 'ctrl', 'TimeAttackGhostListHalf.brctr.json5'),
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
        os.path.join('control', 'ctrl', 'TimeAttackGhostListPageNum.brctr.json5'),
        os.path.join('message_window', 'ctrl', 'TimeAttackGhostListMessageWindowHalf.brctr.json5'),
        # 200cc
        os.path.join('control', 'ctrl', 'VSSettingRadioOption.brctr.json5'),
        # Settings
        os.path.join('control', 'anim', 'common_w201_setting_menu_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy0_loop.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy0_onetime.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy1_loop.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy1_onetime.brlan.json5'),
        os.path.join('control', 'blyt', 'common_w201_setting_menu.brlyt.json5'),
        os.path.join('control', 'blyt', 'gr_area_select_menu.brlyt.json5'),
        os.path.join('control', 'ctrl', 'CategoryUpDownBase.brctr.json5'),
        os.path.join('control', 'ctrl', 'CategoryUpDownButtonL.brctr.json5'),
        os.path.join('control', 'ctrl', 'CategoryUpDownButtonR.brctr.json5'),
        os.path.join('control', 'ctrl', 'SettingUpDownBase.brctr.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_free_to_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_fuchi_check_loop.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_select_to_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_center_to_right.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_hide.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_left_to_center.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_01_ok.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_01_stop.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_02_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_02_stop.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_stop.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_free_to_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_select_to_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_text_center_to_right.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_text_hide.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_text_left_to_center.brlan.json5'),
        os.path.join('ranking', 'blyt', 'area_select_window.brlyt.json5'),
        os.path.join('ranking', 'blyt', 'area_select_window_text.brlyt.json5'),
        os.path.join('ranking', 'blyt', 'category_window.brlyt.json5'),
        os.path.join('ranking', 'blyt', 'category_window_text.brlyt.json5'),
        os.path.join('ranking', 'ctrl', 'CategoryUpDownText.brctr.json5'),
        os.path.join('ranking', 'ctrl', 'CategoryUpDownValue.brctr.json5'),
        os.path.join('ranking', 'ctrl', 'SettingUpDownText.brctr.json5'),
        os.path.join('ranking', 'ctrl', 'SettingUpDownValue.brctr.json5'),
        os.path.join('ranking', 'timg', 'tt_pattern_chek_alpha_nasi_32x32.tpl'),
        os.path.join('ranking', 'timg', 'tt_pattern_chek_bokashi_32x32.tpl'),
    ],
    os.path.join('Scene', 'UI', 'RaceSP.arc.lzma'): [
        # Menu
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
        # Settings
        os.path.join('bg', 'blyt', 'race_obi_bottom_message.brlyt.json5'),
        os.path.join('bg', 'ctrl', 'RaceObiInstructionText.brctr.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w023_rule_menu_text_light_02_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_chara_light_01_ok.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_chara_light_01_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_chara_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_chara_light_02_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_fuchi_check_loop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w134_rule_rap_yaji_l_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_chara_light_01_ok.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_chara_light_01_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_chara_light_02_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_chara_light_02_stop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_fuchi_check_loop.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w135_rule_rap_yaji_r_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_free.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_free_to_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_select.brlan.json5'),
        os.path.join('control', 'anim', 'common_w201_setting_menu_select_to_free.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy0_loop.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy0_onetime.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy1_loop.brlan.json5'),
        os.path.join('control', 'anim', 'gr_area_select_menu_dummy1_onetime.brlan.json5'),
        os.path.join('control', 'blyt', 'common_w134_rule_rap_yaji_l.brlyt.json5'),
        os.path.join('control', 'blyt', 'common_w135_rule_rap_yaji_r.brlyt.json5'),
        os.path.join('control', 'blyt', 'common_w201_setting_menu.brlyt.json5'),
        os.path.join('control', 'blyt', 'gr_area_select_menu.brlyt.json5'),
        os.path.join('control', 'ctrl', 'CategoryUpDownBase.brctr.json5'),
        os.path.join('control', 'ctrl', 'CategoryUpDownButtonL.brctr.json5'),
        os.path.join('control', 'ctrl', 'CategoryUpDownButtonR.brctr.json5'),
        os.path.join('control', 'ctrl', 'SettingUpDownBase.brctr.json5'),
        os.path.join('control', 'ctrl', 'VSSettingUpDownButtonL.brctr.json5'),
        os.path.join('control', 'ctrl', 'VSSettingUpDownButtonR.brctr.json5'),
        os.path.join('control', 'timg', 'tt_yajirushi_type1.tpl'),
        os.path.join('ranking', 'anim', 'area_select_window_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_free_to_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_fuchi_check_loop.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_select_to_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_center_to_right.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_hide.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_left_to_center.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_01_ok.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_01_stop.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_02_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_light_02_stop.brlan.json5'),
        os.path.join('ranking', 'anim', 'area_select_window_text_text_stop.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_free_to_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_select.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_select_to_free.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_text_center_to_right.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_text_hide.brlan.json5'),
        os.path.join('ranking', 'anim', 'category_window_text_text_left_to_center.brlan.json5'),
        os.path.join('ranking', 'blyt', 'area_select_window.brlyt.json5'),
        os.path.join('ranking', 'blyt', 'area_select_window_text.brlyt.json5'),
        os.path.join('ranking', 'blyt', 'category_window.brlyt.json5'),
        os.path.join('ranking', 'blyt', 'category_window_text.brlyt.json5'),
        os.path.join('ranking', 'ctrl', 'CategoryUpDownText.brctr.json5'),
        os.path.join('ranking', 'ctrl', 'CategoryUpDownValue.brctr.json5'),
        os.path.join('ranking', 'ctrl', 'SettingUpDownText.brctr.json5'),
        os.path.join('ranking', 'ctrl', 'SettingUpDownValue.brctr.json5'),
        os.path.join('ranking', 'timg', 'tt_pattern_chek_alpha_nasi_32x32.tpl'),
        os.path.join('ranking', 'timg', 'tt_pattern_chek_bokashi_32x32.tpl'),
        # HUD
        os.path.join('game_image', 'anim', 'game_image_speed_texture_pattern_0_9.brlan.json5'),
        os.path.join('game_image', 'blyt', 'game_image_speed.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'InputDisplay.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'race_message_half.brlyt'),
        os.path.join('game_image', 'ctrl', 'battle_total_point.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'InputDisplay.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'lap_number_sp.brctr.json5'),
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
    os.path.join('Scene', 'UI', 'TitleSP.arc.lzma'): [
        # License selection
        os.path.join('button', 'blyt', 'common_w076_license_icon_center.brlyt.json5'),
        os.path.join('button', 'ctrl', 'LicenseSelect.brctr.json5'),
        os.path.join('button', 'timg', 'tt_license_icon_004.tpl'),
        # WFC and channel removal
        os.path.join('button', 'ctrl', 'TopMenuMultiWaku.brctr.json5'),
        os.path.join('button', 'ctrl', 'TopMenuSingleWaku.brctr.json5'),
    ],
}
for language in LANGUAGES:
    hud_language = HUD_LANGUAGES[language]
    if language != 'K':
        asset_in_files[os.path.join('Race', f'Common_{language}.arc.lzma')] = [
            os.path.join(f'jugemu_lap_{language}.brres'),
            os.path.join(f'jugemu_lapf_{language}.brres'),
        ]
    asset_in_files[os.path.join('Scene', 'UI', f'AwardSP_{language}.arc.lzma')] = [
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
        os.path.join('message', f'Race_{language}.bmg.json5'),
    ]
    asset_in_files[os.path.join('Scene', 'UI', f'ChannelSP_{language}.arc.lzma')] = [
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
    ]
    asset_in_files[os.path.join('Scene', 'UI', f'MenuMultiSP_{language}.arc.lzma')] = [
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
    ]
    asset_in_files[os.path.join('Scene', 'UI', f'MenuOtherSP_{language}.arc.lzma')] = [
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
    ]
    asset_in_files[os.path.join('Scene', 'UI', f'MenuSingleSP_{language}.arc.lzma')] = [
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
    ]
    asset_in_files[os.path.join('Scene', 'UI', f'RaceSP_{language}.arc.lzma')] = [
        os.path.join('game_image', 'timg', f'tt_speed_{hud_language}.tpl'),
        os.path.join('message', f'Common_{language}.bmg.json5'),
        os.path.join('message', f'Menu_{language}.bmg.json5'),
        os.path.join('message', f'Race_{language}.bmg.json5'),
    ]
    if hud_language != 'E':
        asset_in_files[os.path.join('Scene', 'UI', f'RaceSP_{language}.arc.lzma')] += [
            os.path.join('game_image', 'timg', f'tt_lap_{hud_language}.tpl'),
            os.path.join('game_image', 'timg', f'tt_lap_{hud_language}_lap1.tpl'),
            os.path.join('game_image', 'timg', f'tt_lap_{hud_language}_lap2.tpl'),
            os.path.join('game_image', 'timg', f'tt_lap_{hud_language}_lap3.tpl'),
            os.path.join('game_image', 'timg', f'tt_time_{hud_language}.tpl'),
        ]
        for i in range(12):
            for base in ['tt_position_no_st_64x64', 'tt_multi_position_no_st_64x64']:
                asset_in_files[os.path.join('Scene', 'UI', f'RaceSP_{language}.arc.lzma')] += [
                    os.path.join('game_image', 'timg', f'{base}_{hud_language}_{i + 1:02d}.tpl')
                ]
    asset_in_files[os.path.join('Scene', 'UI', f'TitleSP_{language}.arc.lzma')] = [
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
        out_file = os.path.join('$builddir', 'Shared.arc.lzma.d', base + outext)
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
        os.path.join('$builddir', 'contents.arc.d', target),
        'arc',
        asset_out_files[target],
        variables = {
            'arcin': os.path.join('$builddir', 'Shared.arc.lzma.d'),
            'args': target_renamed,
        },
    )
    n.newline()

devkitppc = os.environ.get("DEVKITPPC")
n.variable('nanopb', os.path.join('vendor', 'nanopb', 'generator', 'nanopb_generator.py'))
n.variable('compiler', os.path.join(devkitppc, 'bin', 'powerpc-eabi-gcc'))
n.variable('postprocess', 'postprocess.py')
n.variable('port', 'port.py')
n.variable('lzmac', 'lzmac.py')
n.variable('version', 'version.py')
n.variable('elf2dol', 'elf2dol.py')
n.newline()

n.rule(
    'nanopb',
    command = f'{sys.executable} $nanopb $in -L "#include <vendor/nanopb/%s>" -D build -q',
    description = 'NANOPB $out',
)
n.newline()

common_Sflags = [
    '-isystem', 'include',
    '-isystem', 'payload',
    '-isystem', 'vendor',
]
profile_Sflags = {
    'DEBUG': [
        '-DSP_DEBUG'
    ],
    'TEST': [
        '-DSP_TEST'
    ],
    'RELEASE': [
        '-DSP_RELEASE'
    ],
    'CHANNEL': [
        '-DSP_CHANNEL'
    ],
}
common_cflags = [
    '-DREVOLUTION',
    '-fms-extensions',
    '-fno-asynchronous-unwind-tables',
    '-fplan9-extensions',
    '-fshort-wchar',
    '-isystem', '.',
    '-isystem', 'include',
    '-isystem', 'payload',
    '-isystem', 'vendor',
    '-isystem', 'build',
    '-msdata=none',
    '-Wall',
    '-Werror=implicit-function-declaration',
    '-Werror=incompatible-pointer-types',
    '-Werror=vla',
    '-Wextra',
    '-Wno-packed-bitfield-compat',
]
common_ccflags = [
    '-DREVOLUTION',
    '-fno-asynchronous-unwind-tables',
    '-fno-exceptions',
    '-fno-rtti',
    '-fshort-wchar',
    '-isystem', '.',
    '-isystem', 'include',
    '-isystem', 'payload',
    '-isystem', 'vendor',
    '-isystem', 'build',
    '-msdata=none',
    '-std=c++20',
    '-Wall',
    '-Werror=vla',
    '-Wextra',
    '-Wno-delete-non-virtual-dtor',
    '-Wno-packed-bitfield-compat',
    '-Wsuggest-override',
]
if args.gdb_compatible:
    common_cflags += ['-DGDB_COMPATIBLE=1']
    common_ccflags += ['-DGDB_COMPATIBLE=1']

target_cflags = {
    'stub': [],
    'loader': [],
    'payload': [
        '-fstack-protector-strong',
    ],
}
profile_cflags = {
    'DEBUG': [
        '-O0',
        '-g',
        '-DSP_DEBUG'
    ],
    'TEST': [
        '-O2',
        '-DSP_TEST'
    ],
    'RELEASE': [
        '-O2',
        '-DSP_RELEASE'
    ],
    'CHANNEL': [
        '-O2',
        '-DSP_CHANNEL'
    ],
}
common_ldflags = [
    '-nostdlib',
    '-Wl,-n',
]

n.rule(
    'S',
    command = '$compiler -MD -MT $out -MF $out.d $Sflags -c $in -o $out',
    depfile = '$out.d',
    deps = 'gcc',
    description = 'S $out',
)
n.newline()

n.rule(
    'incbin',
    command = '$compiler -DNAME=$name -DPATH=$path -c Incbin.S -o $out',
    description = 'INCBIN $out',
)
n.newline()

n.rule(
    'c',
    command = '$compiler -MD -MT $out -MF $out.d $cflags -c $in -o $out',
    depfile = '$out.d',
    deps = 'gcc',
    description = 'C $out',
)
n.newline()

n.rule(
    'cc',
    command = '$compiler -MD -MT $out -MF $out.d $ccflags -c $in -o $out',
    depfile = '$out.d',
    deps = 'gcc',
    description = 'CC $out',
)
n.newline()

n.rule(
    'postprocess',
    command = f'{sys.executable} $postprocess $region $in $out',
    description = 'POSTPROCESS $out'
)
n.newline()

n.rule(
    'port',
    command = f'{sys.executable} $port $region $in $out' + (' --base' if args.gdb_compatible else ''),
    description = 'PORT $out'
)
n.newline()

n.rule(
    'ld',
    command = '$compiler $ldflags $in -o $out',
    description = 'LD $out',
)
n.newline()

n.rule(
    'lzmac',
    command = f'{sys.executable} $lzmac $in $out',
    description = 'LZMA $out',
)
n.newline()

n.rule(
    'version',
    command = f'{sys.executable} $version $type $out',
    description = 'VERSION $out',
)
n.newline()

n.rule(
    'elf2dol',
    command = f'{sys.executable} $elf2dol $in $out',
    description = 'DOL $out',
)
n.newline()

protobuf_proto_files = [
    os.path.join('protobuf', 'NetStorage.proto'),
    os.path.join('protobuf', 'Update.proto'),
]
protobuf_h_files = []
protobuf_c_files = []
for proto_file in protobuf_proto_files:
    base, _ = os.path.splitext(proto_file)
    options_file = base + '.options'
    h_file = os.path.join('$builddir', base + '.pb.h')
    c_file = os.path.join('$builddir', base + '.pb.c')
    protobuf_h_files += [h_file]
    protobuf_c_files += [c_file]
    n.build(
        [
            h_file,
            c_file,
        ],
        'nanopb',
        proto_file,
        implicit = options_file,
    )
n.newline()

code_in_files = {
    'payload': [
        *protobuf_c_files,
        os.path.join('common', 'Console.cc'),
        os.path.join('common', 'DCache.cc'),
        os.path.join('common', 'Font.c'),
        os.path.join('common', 'Font.cc'),
        os.path.join('common', 'VI.cc'),
        os.path.join('payload', 'egg', 'core', 'eggColorFader.c'),
        os.path.join('payload', 'egg', 'core', 'eggDisplay.S'),
        os.path.join('payload', 'egg', 'core', 'eggDVDRipper.cc'),
        os.path.join('payload', 'egg', 'core', 'eggEffectCreator.S'),
        os.path.join('payload', 'egg', 'core', 'eggG3dUtil.S'),
        os.path.join('payload', 'egg', 'core', 'eggHeap.c'),
        os.path.join('payload', 'egg', 'core', 'eggSceneManager.cc'),
        os.path.join('payload', 'egg', 'core', 'eggSystem.S'),
        os.path.join('payload', 'egg', 'core', 'eggSystem.c'),
        os.path.join('payload', 'game', 'effect', 'Effect.S'),
        os.path.join('payload', 'game', 'gfx', 'Camera.S'),
        os.path.join('payload', 'game', 'gfx', 'CameraManager.S'),
        os.path.join('payload', 'game', 'host_system', 'RkSystem.S'),
        os.path.join('payload', 'game', 'host_system', 'RkSystem.c'),
        os.path.join('payload', 'game', 'host_system', 'SceneManager.S'),
        os.path.join('payload', 'game', 'host_system', 'SceneManager.c'),
        os.path.join('payload', 'game', 'host_system', 'SystemManager.c'),
        os.path.join('payload', 'game', 'host_system', 'SystemManager.cc'),
        os.path.join('payload', 'game', 'item', 'ItemObjKouraAka.S'),
        os.path.join('payload', 'game', 'item', 'ItemObjKouraAo.S'),
        os.path.join('payload', 'game', 'item', 'ItemObjKouraMidori.S'),
        os.path.join('payload', 'game', 'item', 'KartItem.S'),
        os.path.join('payload', 'game', 'kart', 'KartBody.S'),
        os.path.join('payload', 'game', 'kart', 'KartMove.S'),
        os.path.join('payload', 'game', 'kart', 'KartMove.cc'),
        os.path.join('payload', 'game', 'kart', 'KartObject.S'),
        os.path.join('payload', 'game', 'kart', 'KartObjectManager.S'),
        os.path.join('payload', 'game', 'kart', 'KartObjectManager.c'),
        os.path.join('payload', 'game', 'kart', 'KartObjectManager.cc'),
        os.path.join('payload', 'game', 'kart', 'KartObjectProxy.cc'),
        os.path.join('payload', 'game', 'kart', 'KartParam.S'),
        os.path.join('payload', 'game', 'kart', 'KartState.S'),
        os.path.join('payload', 'game', 'kart', 'KartState.cc'),
        os.path.join('payload', 'game', 'kart', 'KartSub.S'),
        os.path.join('payload', 'game', 'missionrun', 'MissionRun.c'),
        os.path.join('payload', 'game', 'kart', 'VehiclePhysics.cc'),
        os.path.join('payload', 'game', 'net', 'NetManager.S'),
        os.path.join('payload', 'game', 'obj', 'ObjEffect.S'),
        os.path.join('payload', 'game', 'obj', 'ObjManager.c'),
        os.path.join('payload', 'game', 'obj', 'ObjPylon01.S'),
        os.path.join('payload', 'game', 'obj', 'ObjPylon01.c'),
        os.path.join('payload', 'game', 'race', 'Driver.S'),
        os.path.join('payload', 'game', 'race', 'DriverManager.S'),
        os.path.join('payload', 'game', 'race', 'DriverManager.c'),
        os.path.join('payload', 'game', 'race', 'Jugem.S'),
        os.path.join('payload', 'game', 'race', 'JugemManager.S'),
        os.path.join('payload', 'game', 'race', 'JugemManager.c'),
        os.path.join('payload', 'game', 'rel', 'Rel.S'),
        os.path.join('payload', 'game', 'scene', 'globe', 'GlobeManager.S'),
        os.path.join('payload', 'game', 'sound', 'DriverSound.S'),
        os.path.join('payload', 'game', 'sound', 'ItemMusicManager.S'),
        os.path.join('payload', 'game', 'sound', 'ItemMusicManager.cc'),
        os.path.join('payload', 'game', 'sound', 'KartSound.S'),
        os.path.join('payload', 'game', 'sound', 'KartSound.cc'),
        os.path.join('payload', 'game', 'sound', 'RaceSoundManager.cc'),
        os.path.join('payload', 'game', 'sound', 'SceneSoundManager.cc'),
        os.path.join('payload', 'game', 'sound', 'Snd.S'),
        os.path.join('payload', 'game', 'system', 'BugCheck.c'),
        os.path.join('payload', 'game', 'system', 'Console.c'),
        os.path.join('payload', 'game', 'system', 'CourseMap.S'),
        os.path.join('payload', 'game', 'system', 'DvdArchive.S'),
        os.path.join('payload', 'game', 'system', 'DvdArchive.c'),
        os.path.join('payload', 'game', 'system', 'FatalScene.c'),
        os.path.join('payload', 'game', 'system', 'GhostFile.c'),
        os.path.join('payload', 'game', 'system', 'GhostFile.cc'),
        os.path.join('payload', 'game', 'system', 'HomeButton.S'),
        os.path.join('payload', 'game', 'system', 'HomeButton.c'),
        os.path.join('payload', 'game', 'system', 'InputManager.S'),
        os.path.join('payload', 'game', 'system', 'InputManager.c'),
        os.path.join('payload', 'game', 'system', 'InputManager.cc'),
        os.path.join('payload', 'game', 'system', 'Mii.S'),
        os.path.join('payload', 'game', 'system', 'MultiDvdArchive.c'),
        os.path.join('payload', 'game', 'system', 'NandManager.S'),
        os.path.join('payload', 'game', 'system', 'RaceConfig.S'),
        os.path.join('payload', 'game', 'system', 'RaceConfig.c'),
        os.path.join('payload', 'game', 'system', 'RaceConfig.cc'),
        os.path.join('payload', 'game', 'system', 'RaceManager.S'),
        os.path.join('payload', 'game', 'system', 'RaceManager.c'),
        os.path.join('payload', 'game', 'system', 'RaceManager.cc'),
        os.path.join('payload', 'game', 'system', 'ResourceManager.S'),
        os.path.join('payload', 'game', 'system', 'ResourceManager.c'),
        os.path.join('payload', 'game', 'system', 'ResourceManager.cc'),
        os.path.join('payload', 'game', 'system', 'RootScene.S'),
        os.path.join('payload', 'game', 'system', 'SaveManager.cc'),
        os.path.join('payload', 'game', 'system', 'SceneCreatorDynamic.S'),
        os.path.join('payload', 'game', 'system', 'SceneCreatorDynamic.c'),
        os.path.join('payload', 'game', 'ui', 'AwardPage.S'),
        os.path.join('payload', 'game', 'ui', 'Button.cc'),
        os.path.join('payload', 'game', 'ui', 'ChannelPage.cc'),
        os.path.join('payload', 'game', 'ui', 'ConfirmPage.cc'),
        os.path.join('payload', 'game', 'ui', 'ControlGroup.cc'),
        os.path.join('payload', 'game', 'ui', 'ControlLoader.S'),
        os.path.join('payload', 'game', 'ui', 'Font.S'),
        os.path.join('payload', 'game', 'ui', 'Font.c'),
        os.path.join('payload', 'game', 'ui', 'FontManager.c'),
        os.path.join('payload', 'game', 'ui', 'GhostManagerPage.cc'),
        os.path.join('payload', 'game', 'ui', 'GhostSelectButton.cc'),
        os.path.join('payload', 'game', 'ui', 'GhostSelectControl.cc'),
        os.path.join('payload', 'game', 'ui', 'Layout.S'),
        os.path.join('payload', 'game', 'ui', 'License.S'),
        os.path.join('payload', 'game', 'ui', 'License.c'),
        os.path.join('payload', 'game', 'ui', 'LicenseSelectPage.cc'),
        os.path.join('payload', 'game', 'ui', 'Map2DRenderer.c'),
        os.path.join('payload', 'game', 'ui', 'MenuInputManager.cc'),
        os.path.join('payload', 'game', 'ui', 'MessageWindowControl.cc'),
        os.path.join('payload', 'game', 'ui', 'MiiGroup.cc'),
        os.path.join('payload', 'game', 'ui', 'Model.S'),
        os.path.join('payload', 'game', 'ui', 'ModelPage.cc'),
        os.path.join('payload', 'game', 'ui', 'Option.cc'),
        os.path.join('payload', 'game', 'ui', 'Page.cc'),
        os.path.join('payload', 'game', 'ui', 'RaceConfirmPage.cc'),
        os.path.join('payload', 'game', 'ui', 'RadioButtonControl.cc'),
        os.path.join('payload', 'game', 'ui', 'Save.S'),
        os.path.join('payload', 'game', 'ui', 'SaveManagerProxy.S'),
        os.path.join('payload', 'game', 'ui', 'Section.S'),
        os.path.join('payload', 'game', 'ui', 'Section.cc'),
        os.path.join('payload', 'game', 'ui', 'SectionManager.S'),
        os.path.join('payload', 'game', 'ui', 'SectionManager.c'),
        os.path.join('payload', 'game', 'ui', 'SectionManager.cc'),
        os.path.join('payload', 'game', 'ui', 'ServicePackTopPage.cc'),
        os.path.join('payload', 'game', 'ui', 'SettingsPage.cc'),
        os.path.join('payload', 'game', 'ui', 'SheetSelectControl.cc'),
        os.path.join('payload', 'game', 'ui', 'SingleTopPage.cc'),
        os.path.join('payload', 'game', 'ui', 'StorageBenchmarkPage.cc'),
        os.path.join('payload', 'game', 'ui', 'TabControl.cc'),
        os.path.join('payload', 'game', 'ui', 'TimeAttackGhostListPage.cc'),
        os.path.join('payload', 'game', 'ui', 'TimeAttackTopPage.S'),
        os.path.join('payload', 'game', 'ui', 'TitlePage.S'),
        os.path.join('payload', 'game', 'ui', 'TitlePage.c'),
        os.path.join('payload', 'game', 'ui', 'UIAnimator.c'),
        os.path.join('payload', 'game', 'ui', 'UIAnimator.cc'),
        os.path.join('payload', 'game', 'ui', 'UIControl.c'),
        os.path.join('payload', 'game', 'ui', 'UIControl.cc'),
        os.path.join('payload', 'game', 'ui', 'UpdatePage.cc'),
        os.path.join('payload', 'game', 'ui', 'UpDownControl.cc'),
        os.path.join('payload', 'game', 'ui', 'Wipe.S'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlMenuBackButton.cc'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlMenuInstructionText.cc'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlMenuPageTitleText.cc'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRace2DMap.S'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceBase.cc'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceInputDisplay.cc'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceLap.cc'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceNameBalloon.S'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceNameBalloon.c'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceRankNum.cc'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceSpeed.cc'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceTime.S'),
        os.path.join('payload', 'game', 'ui', 'ctrl', 'CtrlRaceTime.cc'),
        os.path.join('payload', 'game', 'ui', 'page', 'CharacterSelectPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'CourseSelectPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'CupSelectPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'DemoPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'DriftSelectPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'MachineSelectPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'RaceMenuPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'RaceMenuPage.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'RacePage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'RacePage.cc'),
        os.path.join('payload', 'game', 'ui', 'page', 'TimeAttackSplitsPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'TimeAttackSplitsPage.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'TopMenuPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'TopMenuPage.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'VsMenuPage.S'),
        os.path.join('payload', 'game', 'ui', 'page', 'VsMenuPage.c'),
        os.path.join('payload', 'game', 'ui', 'page', 'VsRulesPage.c'),
        os.path.join('payload', 'game', 'util', 'Input.S'),
        os.path.join('payload', 'nw4r', 'db', 'dbAssert.S'),
        os.path.join('payload', 'nw4r', 'db', 'dbException.S'),
        os.path.join('payload', 'nw4r', 'db', 'dbException.c'),
        os.path.join('payload', 'nw4r', 'g3d', 'g3d_resmat.c'),
        os.path.join('payload', 'nw4r', 'g3d', 'g3dResFile.S'),
        os.path.join('payload', 'nw4r', 'g3d', 'MSan.c'),
        os.path.join('payload', 'nw4r', 'lyt', 'lyt_arcResourceAccessor.S'),
        os.path.join('payload', 'nw4r', 'lyt', 'lyt_layout.S'),
        os.path.join('payload', 'nw4r', 'snd', 'DVDSoundArchive.cc'),
        os.path.join('payload', 'nw4r', 'snd', 'SoundArchive.cc'),
        os.path.join('payload', 'nw4r', 'snd', 'SoundArchiveFileReader.cc'),
        os.path.join('payload', 'nw4r', 'snd', 'SoundArchiveLoader.cc'),
        os.path.join('payload', 'nw4r', 'snd', 'SoundArchivePlayer.cc'),
        os.path.join('payload', 'nw4r', 'snd', 'SoundHandle.cc'),
        os.path.join('payload', 'nw4r', 'snd', 'StrmFileReader.cc'),
        os.path.join('payload', 'nw4r', 'ut', 'FileStream.cc'),
        os.path.join('payload', 'nw4r', 'ut', 'IOStream.cc'),
        os.path.join('payload', 'platform', 'string.c'),
        os.path.join('payload', 'platform', 'wchar.c'),
        os.path.join('payload', 'revolution', 'arc.S'),
        os.path.join('payload', 'revolution', 'arc.c'),
        os.path.join('payload', 'revolution', 'ax.c'),
        os.path.join('payload', 'revolution', 'dvd.c'),
        os.path.join('payload', 'revolution', 'es.c'),
        os.path.join('payload', 'revolution', 'exi.c'),
        os.path.join('payload', 'revolution', 'ios.S'),
        os.path.join('payload', 'revolution', 'ios.c'),
        os.path.join('payload', 'revolution', 'nand.c'),
        os.path.join('payload', 'revolution', 'OS.S'),
        os.path.join('payload', 'revolution', 'OS.c'),
        os.path.join('payload', 'revolution', 'start.S'),
        os.path.join('payload', 'revolution', 'os', 'OSContext.S'),
        os.path.join('payload', 'revolution', 'os', 'OSError.S'),
        os.path.join('payload', 'revolution', 'os', 'OSError.c'),
        os.path.join('payload', 'revolution', 'os', 'OSMemory.S'),
        os.path.join('payload', 'revolution', 'os', 'OSMemory.c'),
        os.path.join('payload', 'revolution', 'os', 'OSThread.S'),
        os.path.join('payload', 'revolution', 'os', 'OSThread.c'),
        os.path.join('payload', 'revolution', 'os', 'OSTime.cc'),
        os.path.join('payload', 'revolution', 'so', 'SOBasic.S'),
        os.path.join('payload', 'sp', 'Channel.cc'),
        os.path.join('payload', 'sp', 'Commands.c'),
        os.path.join('payload', 'sp', 'Fatal.c'),
        os.path.join('payload', 'sp', 'FormattingCodes.c'),
        os.path.join('payload', 'sp', 'FlameGraph.c'),
        os.path.join('payload', 'sp', 'Host.c'),
        os.path.join('payload', 'sp', 'IOSDolphin.c'),
        os.path.join('payload', 'sp', 'LZ77Decoder.cc'),
        os.path.join('payload', 'sp', 'LZMADecoder.cc'),
        os.path.join('payload', 'sp', 'Panic.c'),
        os.path.join('payload', 'sp', 'Patcher.c'),
        os.path.join('payload', 'sp', 'Payload.cc'),
        os.path.join('payload', 'sp', 'Rel.cc'),
        os.path.join('payload', 'sp', 'ScopeLock.cc'),
        os.path.join('payload', 'sp', 'Slab.c'),
        os.path.join('payload', 'sp', 'StackTrace.c'),
        os.path.join('payload', 'sp', 'Time.cc'),
        os.path.join('payload', 'sp', 'Update.cc'),
        os.path.join('payload', 'sp', 'Yaz.c'),
        os.path.join('payload', 'sp', 'YAZDecoder.cc'),
        os.path.join('payload', 'sp', 'keyboard', 'Keyboard.c'),
        os.path.join('payload', 'sp', 'keyboard', 'SIKeyboard.c'),
        os.path.join('payload', 'sp', 'keyboard', 'IOSKeyboard.c'),
        os.path.join('payload', 'sp', 'net', 'Net.cc'),
        os.path.join('payload', 'sp', 'net', 'Socket.cc'),
        os.path.join('payload', 'sp', 'security', 'Memory.c'),
        os.path.join('payload', 'sp', 'security', 'Stack.S'),
        os.path.join('payload', 'sp', 'security', 'Stack.c'),
        os.path.join('payload', 'sp', 'security', 'StackTrace.S'),
        os.path.join('payload', 'sp', 'settings', 'ClientSettings.cc'),
        os.path.join('payload', 'sp', 'settings', 'IniReader.cc'),
        os.path.join('payload', 'sp', 'storage', 'DecompLoader.cc'),
        os.path.join('payload', 'sp', 'storage', 'DVDStorage.cc'),
        os.path.join('payload', 'sp', 'storage', 'FATStorage.cc'),
        os.path.join('payload', 'sp', 'storage', 'LogFile.cc'),
        os.path.join('payload', 'sp', 'storage', 'NANDArchiveStorage.cc'),
        os.path.join('payload', 'sp', 'storage', 'NetStorage.cc'),
        os.path.join('payload', 'sp', 'storage', 'Sdi.c'),
        os.path.join('payload', 'sp', 'storage', 'Storage.cc'),
        os.path.join('payload', 'sp', 'storage', 'Usb.c'),
        os.path.join('payload', 'sp', 'storage', 'UsbStorage.c'),
        os.path.join('vendor', 'arith64.c'),
        os.path.join('vendor', 'ff', 'diskio.c'),
        os.path.join('vendor', 'ff', 'ff.c'),
        os.path.join('vendor', 'ff', 'fftime.c'),
        os.path.join('vendor', 'ff', 'ffunicode.c'),
        os.path.join('vendor', 'libhydrogen', 'hydrogen.c'),
        os.path.join('vendor', 'lzma', 'LzmaDec.c'),
        os.path.join('vendor', 'nanopb', 'pb_common.c'),
        os.path.join('vendor', 'nanopb', 'pb_decode.c'),
        os.path.join('vendor', 'nanopb', 'pb_encode.c'),
    ],
    'loader': [
        os.path.join('common', 'Clock.cc'),
        os.path.join('common', 'Console.cc'),
        os.path.join('common', 'DCache.cc'),
        os.path.join('common', 'Font.c'),
        os.path.join('common', 'Font.cc'),
        os.path.join('common', 'ICache.cc'),
        os.path.join('common', 'IOS.cc'),
        os.path.join('common', 'Memcpy.c'),
        os.path.join('common', 'Memset.c'),
        os.path.join('common', 'Strlen.c'),
        os.path.join('common', 'VI.cc'),
        os.path.join('loader', 'Apploader.cc'),
        os.path.join('loader', 'DI.cc'),
        os.path.join('loader', 'Loader.cc'),
        os.path.join('loader', 'Start.S'),
    ],
    'stub': [
        os.path.join('common', 'Clock.cc'),
        os.path.join('common', 'Console.cc'),
        os.path.join('common', 'DCache.cc'),
        os.path.join('common', 'Font.c'),
        os.path.join('common', 'Font.cc'),
        os.path.join('common', 'ICache.cc'),
        os.path.join('common', 'IOS.cc'),
        os.path.join('common', 'Memcpy.c'),
        os.path.join('common', 'Memset.c'),
        os.path.join('common', 'Strlen.c'),
        os.path.join('common', 'VI.cc'),
        os.path.join('stub', 'Archive.cc'),
        os.path.join('stub', 'Dolphin.cc'),
        os.path.join('stub', 'FS.cc'),
        os.path.join('stub', 'LZMA.cc'),
        os.path.join('stub', 'Memcmp.c'),
        os.path.join('stub', 'Start.S'),
        os.path.join('stub', 'Strchr.c'),
        os.path.join('stub', 'Strlcpy.c'),
        os.path.join('stub', 'Strncmp.c'),
        os.path.join('stub', 'Stub.cc'),
        os.path.join('vendor', 'lzma', 'LzmaDec.c'),
    ],
}
code_out_files = {}
for profile in ['DEBUG', 'TEST', 'RELEASE', 'CHANNEL']:
    code_out_files[profile] = {target: [] for target in code_in_files}

for target in code_in_files:
    for in_file in code_in_files[target]:
        _, ext = os.path.splitext(in_file)
        for profile in ['DEBUG', 'TEST', 'RELEASE', 'CHANNEL']:
            if target == 'stub':
                suffix = profile[0] + '.o'
            elif profile == 'TEST' or profile == 'CHANNEL':
                continue
            else:
                suffix = '.o' if profile == 'RELEASE' else 'D.o'
            out_file = os.path.join('$builddir', target, in_file + suffix)
            code_out_files[profile][target] += [out_file]
            n.build(
                out_file,
                ext[1:],
                in_file,
                variables = {
                    'Sflags': ' '.join([
                        *common_Sflags,
                        *profile_Sflags[profile],
                    ]),
                    'cflags': ' '.join([
                        *common_cflags,
                        *target_cflags[target],
                        *profile_cflags[profile],
                    ]),
                    'ccflags': ' '.join([
                        *common_ccflags,
                        *target_cflags[target],
                        *profile_cflags[profile],
                    ]),
                },
                implicit = [*protobuf_proto_files, *protobuf_h_files] if target == 'payload' else [],
            )
        n.newline()

for profile in ['DEBUG', 'RELEASE']:
    suffix = 'D' if profile == 'DEBUG' else ''
    n.build(
        os.path.join('$builddir', 'bin', f'payload{suffix}.o'),
        'ld',
        code_out_files[profile]['payload'],
        variables = {
            'ldflags': ' '.join([
                *common_ldflags,
                '-r',
            ]),
        },
    )
    n.newline()

for profile in ['DEBUG', 'RELEASE']:
    suffix = 'D' if profile == 'DEBUG' else ''
    n.build(
        [
            os.path.join('$builddir', 'bin', f'symbols{suffix}.txt'),
            os.path.join('$builddir', 'bin', f'Replacements{suffix}.c'),
        ],
        'postprocess',
        [
            os.path.join('$builddir', 'bin', f'payload{suffix}.o'),
            'symbols.txt',
        ],
        implicit = '$postprocess',
    )
    n.newline()

for region in ['P', 'E', 'J', 'K']:
    for profile in ['DEBUG', 'RELEASE']:
        suffix = 'D' if profile == 'DEBUG' else ''
        n.build(
            os.path.join('$builddir', 'scripts', f'RMC{region}{suffix}.ld'),
            'port',
            os.path.join('$builddir', 'bin', f'symbols{suffix}.txt'),
            variables = {
                'region': region,
            },
            implicit = '$port',
        )
        n.newline()

for profile in ['DEBUG', 'RELEASE']:
    suffix = 'D' if profile == 'DEBUG' else ''
    n.build(
        os.path.join('$builddir', 'bin', f'Replacements.c{suffix}.o'),
        'c',
        os.path.join('$builddir', 'bin', f'Replacements{suffix}.c'),
        variables = {
            'cflags': ' '.join([
                *common_cflags,
                *target_cflags[target],
                *profile_cflags[profile],
            ]),
        },
    )
    n.newline()

for region in ['P', 'E', 'J', 'K']:
    for fmt in ['binary', 'elf32-powerpc']:
        for profile in ['DEBUG', 'RELEASE']:
            suffix = 'D' if profile == 'DEBUG' else ''
            extension = 'bin' if fmt == 'binary' else 'elf'
            base = {
                'P': '0x8076db60' if not args.gdb_compatible else '0x809C4FA0',
                'E': '0x80769400',
                'J': '0x8076cca0',
                'K': '0x8075bfe0',
            }[region]
            n.build(
                os.path.join('$builddir', 'bin', f'payload{region}{suffix}.{extension}'),
                'ld',
                [
                    os.path.join('$builddir', 'bin', f'payload{suffix}.o'),
                    os.path.join('$builddir', 'bin', f'Replacements.c{suffix}.o'),
                ],
                variables = {
                    'ldflags' : ' '.join([
                        *common_ldflags,
                        f'-Wl,--defsym,base={base}',
                        '-Wl,--entry=Payload_Run',
                        f'-Wl,--oformat,{fmt}',
                        '-Wl,-T,' + os.path.join('$builddir', 'scripts', f'RMC{region}{suffix}.ld'),
                    ]),
                },
                implicit = os.path.join('$builddir', 'scripts', f'RMC{region}{suffix}.ld'),
            )
            n.newline()

for region in ['P', 'E', 'J', 'K']:
    for profile in ['DEBUG', 'RELEASE']:
        suffix = 'D' if profile == 'DEBUG' else ''
        out_file = os.path.join('$builddir', 'loader', f'payload{region}{suffix}.o')
        n.build(
            out_file,
            'incbin',
            os.path.join('$builddir', 'bin', f'payload{region}{suffix}.bin'),
            variables = {
                'name': f'payload{region}',
                'path': '/'.join(['$builddir', 'bin', f'payload{region}{suffix}.bin']),
            },
            implicit = 'Incbin.S',
        )
        code_out_files[profile]['loader'] += [out_file]
n.newline()

for fmt in ['binary', 'elf32-powerpc']:
    for profile in ['DEBUG', 'RELEASE']:
        suffix = 'D' if profile == 'DEBUG' else ''
        extension = 'bin' if fmt == 'binary' else 'elf'
        n.build(
            os.path.join('$builddir', 'bin', f'loader{suffix}.{extension}'),
            'ld',
            code_out_files[profile]['loader'],
            variables = {
                'ldflags' : ' '.join([
                    *common_ldflags,
                    '-Wl,--defsym,base=0x80b00000',
                    '-Wl,--entry=start',
                    f'-Wl,--oformat,{fmt}',
                    '-Wl,-T,' + os.path.join('common', 'RMC.ld'),
                ]),
            },
            implicit = os.path.join('common', 'RMC.ld'),
        )
        n.newline()

for profile in ['DEBUG', 'RELEASE']:
    suffix = 'D' if profile == 'DEBUG' else ''
    n.build(
        os.path.join('$builddir', 'contents.arc.d', 'bin', f'loader{suffix}.bin.lzma'),
        'lzmac',
        os.path.join('$builddir', 'bin', f'loader{suffix}.bin'),
    )

for profile in ['DEBUG', 'TEST', 'RELEASE']:
    suffix = profile[0]
    n.build(
        os.path.join('$builddir', 'contents.arc.d', 'bin', f'version{suffix}.bin'),
        'version',
        '$version',
        variables = {
            'type': profile.lower(),
        },
    )

n.build(
    os.path.join('$builddir', 'contents.arc.d', 'banner.bin'),
    'cp',
    'banner.bin',
)

n.build(
    os.path.join('$builddir', 'contents.arc.d', 'channel', 'opening.bnr.lzma'),
    'lzmac',
    'opening.bnr',
)

n.build(
    os.path.join('$builddir', 'bin', 'stubC.elf'),
    'ld',
    code_out_files['CHANNEL']['stub'],
    variables = {
        'ldflags' : ' '.join([
            *common_ldflags,
            '-Wl,--defsym,base=0x80100000',
            '-Wl,--entry=start',
            '-Wl,-T,' + os.path.join('common', 'RMC.ld'),
        ]),
    },
    implicit = os.path.join('common', 'RMC.ld'),
)
n.newline()

n.build(
    os.path.join('$builddir', 'bin', 'stubC.dol'),
    'elf2dol',
    os.path.join('$builddir', 'bin', 'stubC.elf'),
    implicit = '$elf2dol',
)
n.newline()

n.build(
    os.path.join('$builddir', 'contents.arc.d', 'channel', 'boot.dol.lzma'),
    'lzmac',
    os.path.join('$builddir', 'bin', 'stubC.dol'),
)
n.newline()

for profile in ['DEBUG', 'TEST', 'RELEASE']:
    in_suffix = 'D' if profile == 'DEBUG' else ''
    out_suffix = profile[0]
    in_paths = [
        *[os.path.join('$builddir', 'contents.arc.d', target) for target in asset_out_files],
        os.path.join('$builddir', 'contents.arc.d', 'bin', f'loader{in_suffix}.bin.lzma'),
        os.path.join('$builddir', 'contents.arc.d', 'bin', f'version{out_suffix}.bin'),
        os.path.join('$builddir', 'contents.arc.d', 'banner.bin'),
    ]
    if profile == 'RELEASE':
        in_paths += [
            os.path.join('$builddir', 'contents.arc.d', 'channel', 'opening.bnr.lzma'),
            os.path.join('$builddir', 'contents.arc.d', 'channel', 'boot.dol.lzma'),
        ]
    n.build(
        os.path.join('$builddir', f'contents{out_suffix}.arc'),
        'arc',
        in_paths,
        variables = {
            'arcin': os.path.join('$builddir', 'contents.arc.d'),
            'args': ' '.join([
                '--renamed loaderD.bin.lzma loader.bin.lzma',
                '--renamed versionD.bin version.bin',
                '--renamed versionT.bin version.bin',
                '--renamed versionR.bin version.bin',
            ]),
        },
    )

for profile in ['DEBUG', 'TEST', 'RELEASE']:
    suffix = profile[0]
    out_file = os.path.join('$builddir', 'stub', f'contents{suffix}.o')
    n.build(
        out_file,
        'incbin',
        os.path.join('$builddir', f'contents{suffix}.arc'),
        variables = {
            'name': 'embeddedContents',
            'path': '/'.join(['$builddir', f'contents{suffix}.arc']),
        },
        implicit = 'Incbin.S',
    )
    code_out_files[profile]['stub'] += [out_file]

for profile in ['DEBUG', 'TEST', 'RELEASE']:
    suffix = profile[0]
    n.build(
        os.path.join('$builddir', 'bin', f'stub{suffix}.elf'),
        'ld',
        code_out_files[profile]['stub'],
        variables = {
            'ldflags' : ' '.join([
                *common_ldflags,
                '-Wl,--defsym,base=0x80100000',
                '-Wl,--entry=start',
                '-Wl,-T,' + os.path.join('common', 'RMC.ld'),
            ]),
        },
        implicit = os.path.join('common', 'RMC.ld'),
    )
    n.newline()

for profile in ['DEBUG', 'TEST', 'RELEASE']:
    suffix = profile[0]
    n.build(
        os.path.join('$outdir', profile.lower(), 'boot.dol'),
        'elf2dol',
        os.path.join('$builddir', 'bin', f'stub{suffix}.elf'),
        implicit = '$elf2dol',
    )
n.newline()

for profile in ['DEBUG', 'TEST', 'RELEASE']:
    n.build(
        os.path.join('$outdir', profile.lower(), 'meta.xml'),
        'cp',
        os.path.join('meta', profile.lower() + '.xml'),
    )
n.newline()

for profile in ['DEBUG', 'TEST', 'RELEASE']:
    in_suffix = 'D' if profile == 'DEBUG' else ''
    out_suffix = profile[0]
    n.build(
        profile.lower(),
        'phony',
        [
            os.path.join('$builddir', 'bin', f'payloadP{in_suffix}.elf'),
            os.path.join('$builddir', 'bin', f'payloadE{in_suffix}.elf'),
            os.path.join('$builddir', 'bin', f'payloadJ{in_suffix}.elf'),
            os.path.join('$builddir', 'bin', f'payloadK{in_suffix}.elf'),
            os.path.join('$builddir', 'bin', f'loader{in_suffix}.elf'),
            os.path.join('$outdir', profile.lower(), 'boot.dol'),
            os.path.join('$outdir', profile.lower(), 'meta.xml'),
        ]
    )
n.newline()

n.build(
    'all',
    'phony',
    [
        'debug',
        'test',
        'release',
    ],
)
n.newline()

n.default([
    'debug',
    'test',
])
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
