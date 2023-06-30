#!/usr/bin/env python3


import argparse
import glob
import io
import os
import platform
import subprocess
import sys
import tempfile

from vendor.ninja_syntax import Writer


try:
    import json5
    del json5
except ModuleNotFoundError:
    raise SystemExit("Error: pyjson5 not found. Please install it with `python -m pip install json5`")

if sys.version_info < (3, 10):
    raise SystemExit("Error: Python 3.10 or newer is required")

if platform.python_implementation() == "PyPy":
    print("Warning: PyPy may be slower, due to spawning many Python processes")

features = [
    'mission-mode',
    'online',
    'save-states',
]

our_argv = []
ninja_argv = []
found_seperator = False
for arg in sys.argv[1:]:
    if found_seperator:
        ninja_argv.append(arg)
    elif arg == "--":
        found_seperator = True
    else:
        our_argv.append(arg)

parser = argparse.ArgumentParser()
parser.add_argument('--gdb_compatible', action='store_true')
parser.add_argument("--dry", action="store_true")
parser.add_argument("--ci", action="store_true")
for feature in features:
    parser.add_argument(f'--{feature}', default=True, action=argparse.BooleanOptionalAction)
args = parser.parse_args(our_argv)

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

thumbnail_in_files = sorted(glob.glob("thumbnails/*.jpg"))
for in_file in thumbnail_in_files:
    out_file = os.path.join('$builddir', 'contents.arc.d', in_file)
    n.build(
        out_file,
        'cp',
        in_file,
    )

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

pack_select_assets = [
    os.path.join('bg', 'blyt', 'obi_bottom.brlyt.json5'),
    os.path.join('bg', 'blyt', 'obi_bottom_message.brlyt.json5'),
    os.path.join('button', 'anim', 'course_select_button_free.brlan.json5'),
    os.path.join('button', 'anim', 'course_select_button_free_to_select.brlan.json5'),
    os.path.join('button', 'anim', 'course_select_button_select.brlan.json5'),
    os.path.join('button', 'anim', 'course_select_button_select_to_free.brlan.json5'),
    os.path.join('button', 'blyt', 'course_select_button.brlyt.json5'),
    os.path.join('button', 'blyt', 'course_select_scroll_bar.brlyt.json5'),
    os.path.join('button', 'ctrl', 'CourseSelectArrowLeft.brctr.json5'),
    os.path.join('button', 'ctrl', 'CourseSelectArrowRight.brctr.json5'),
    os.path.join('button', 'ctrl', 'CourseSelectButton.brctr.json5'),
    os.path.join('button', 'ctrl', 'CourseSelectScrollBar.brctr.json5'),
    os.path.join('button', 'blyt', 'pack_select_button.brlyt.json5'),
    os.path.join('button', 'ctrl', 'PackSelectButton.brctr.json5'),
    os.path.join('control', 'ctrl', 'CourseSelectPageNum.brctr.json5'),
    os.path.join('control', 'ctrl', 'TimeAttackGhostListPageNum.brctr.json5'),
]

asset_in_files = {
    os.path.join('Scene', 'UI', 'AwardSP.arc.lzma'): sorted(glob.glob("award/**/*.json5", root_dir="assets")),
    os.path.join('Scene', 'UI', 'CrashSP.arc.lzma'): sorted(glob.glob("fatal/**/*.*", root_dir="assets", recursive=True)),
    os.path.join('Race', 'CommonSP.arc.lzma'): [
        # Thumbnails
        os.path.join('kartCameraParamThumbnails.bin'),
        # Mega TC
        os.path.join('MegaTC.brres'),
        # Battle mode
        os.path.join('balloon.brres'),
        os.path.join('bikePartsDispParam.bin'),
        os.path.join('kartPartsDispParam.bin'),
        os.path.join('RKRace_SP.breff'),
        os.path.join('RKRace_SP.breft'),
        # Online mode
        os.path.join('ItemSlotOnline.bin'),
    ],
    os.path.join('Scene', 'UI', 'ChannelSP.arc.lzma'): [
        # Explanation text with 2 lines
        os.path.join('bg', 'blyt', 'obi_bottom.brlyt.json5'),
        os.path.join('bg', 'blyt', 'obi_bottom_message.brlyt.json5'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_curve_000.tpl'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_right_000.tpl'),
        #
        os.path.join('bg', 'ctrl', 'MenuObiTitleTextOption.brctr.json5'),
        # Channel
        os.path.join('button', 'ctrl', 'ServicePackChannelButton.brctr.json5'),
        # Tools
        os.path.join('button', 'ctrl', 'ServicePackToolsButton.brctr.json5'),
        os.path.join('button', 'ctrl', 'ServicePackTopButton.brctr.json5'),
        os.path.join('button', 'ctrl', 'ToolsButton.brctr.json5'),
        # Course/Pack Select
        *pack_select_assets,
        os.path.join('vanillaTracks.pb.bin'),
        *[os.path.normpath(i) for i in sorted(glob.glob("vanillaTracks/*.pb.bin", root_dir="assets", recursive=True))],
    ],
    os.path.join('Scene', 'UI', 'FontSP_K.arc.lzma'): [
        os.path.join('kart_font_korea.brfnt'),
        os.path.join('tt_kart_font_rodan_ntlg_pro_b_K.brfnt'),
        os.path.join('mario_font_number_nocolor.brfnt'),
        os.path.join('mario_font_number_outline.brfnt'),
    ],
    os.path.join('Scene', 'UI', 'FontSP_R.arc.lzma'): [
        os.path.join('kart_kanji_font.brfnt'),
        os.path.join('tt_kart_font_rodan_ntlg_pro_b_R.brfnt'),
        os.path.join('mario_font_number_nocolor.brfnt'),
        os.path.join('mario_font_number_outline.brfnt'),
    ],
    os.path.join('Scene', 'UI', 'GlobeSP.arc.lzma'): [
        # Explanation text with 2 lines
        os.path.join('bg', 'blyt', 'obi_bottom.brlyt.json5'),
        os.path.join('bg', 'blyt', 'obi_bottom_message.brlyt.json5'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_curve_000.tpl'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_right_000.tpl'),
        #
        os.path.join('button', 'blyt', 'common_w205_room_id.brlyt.json5'),
        os.path.join('button', 'blyt', 'common_w206_room_id_text.brlyt.json5'),
        os.path.join('button', 'ctrl', 'FriendRoom.brctr.json5'),
        os.path.join('button', 'ctrl', 'OnlineTopButton.brctr.json5'),
        os.path.join('button', 'ctrl', 'RoomCodeEditBox.brctr.json5'),
        os.path.join('button', 'ctrl', 'RoomCodeEditBoxLetter.brctr.json5'),
        os.path.join('button', 'ctrl', 'RoomCodeKeyboard.brctr.json5'),
        os.path.join('button', 'ctrl', 'SettingsButton.brctr.json5'),
        os.path.join('message_window', 'blyt', 'room_rules.brlyt.json5'),
        os.path.join('message_window', 'ctrl', 'RoomRules.brctr.json5'),
        # Settings
        os.path.join('bg', 'blyt', 'race_obi_bottom_message.brlyt.json5'),
        os.path.join('bg', 'ctrl', 'ObiInstructionTextPopup.brctr.json5'),
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
        os.path.join('control', 'ctrl', 'SettingUpDownButtonL.brctr.json5'),
        os.path.join('control', 'ctrl', 'SettingUpDownButtonR.brctr.json5'),
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
        # Teams
        os.path.join('button', 'anim', 'online_team_select_light_01_ok.brlan.json5'),
        os.path.join('button', 'anim', 'online_team_select_light_01_stop.brlan.json5'),
        os.path.join('button', 'anim', 'online_team_select_light_02_select.brlan.json5'),
        os.path.join('button', 'anim', 'online_team_select_light_02_stop.brlan.json5'),
        os.path.join('button', 'blyt', 'online_team_select.brlyt.json5'),
        os.path.join('button', 'ctrl', 'OnlineTeamSelect.brctr.json5'),
        os.path.join('button', 'timg', 'ht_squareWhite_00.tpl'),
        # Multiplayer Character Select
        os.path.join('button', 'ctrl', 'CharacterSelect4_0_Multi.brctr.json5'),
        os.path.join('button', 'ctrl', 'CharacterSelect4_1_Multi.brctr.json5'),
        os.path.join('button', 'ctrl', 'CharacterSelect4_2_Multi.brctr.json5'),
        os.path.join('button', 'ctrl', 'CharacterSelect4_3_Multi.brctr.json5'),
        os.path.join('button', 'blyt', 'common_w117_mii_suit.brlyt.json5'),
        # Vanilla track manifests
        os.path.join('vanillaTracks.pb.bin'),
        *[os.path.normpath(i) for i in sorted(glob.glob("vanillaTracks/*.pb.bin", root_dir="assets", recursive=True))],
        # Pack select assets
        os.path.join('button', 'anim', 'common_w010_cup_fuchi_check_loop.brlan.json5'),
        os.path.join('button', 'anim', 'common_w010_cup_text_light_01_ok.brlan.json5'),
        os.path.join('button', 'anim', 'common_w010_cup_text_light_01_stop.brlan.json5'),
        os.path.join('button', 'anim', 'common_w010_cup_text_light_02_select.brlan.json5'),
        os.path.join('button', 'anim', 'common_w010_cup_text_light_02_stop.brlan.json5'),
        *pack_select_assets,
    ],
    os.path.join('Scene', 'UI', 'MenuMultiSP.arc.lzma'): [
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_fade_in.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_fade_in_after.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_fade_in_before.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_fade_out.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_free.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_free_to_select.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_fuchi_check_loop.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_select.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_select_to_free.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_text_light_01_ok.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_text_light_01_stop.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_text_light_02_select.brlan.json5'),
        os.path.join('button', 'anim', 'common_w083_earth_chat_message_text_light_02_stop.brlan.json5'),
        os.path.join('button', 'blyt', 'common_w083_earth_chat_message.brlyt.json5'),
        os.path.join('button', 'blyt', 'common_w132_movie_button_multi.brlyt.json5'),
        os.path.join('button', 'ctrl', 'SettingsButton.brctr.json5'),
        os.path.join('control', 'anim', 'common_w207_multi_team_rap_text_text_center_to_right.brlan.json5'),
        os.path.join('control', 'anim', 'common_w207_multi_team_rap_text_text_hide.brlan.json5'),
        os.path.join('control', 'anim', 'common_w207_multi_team_rap_text_text_left_to_center.brlan.json5'),
        os.path.join('control', 'anim', 'common_w207_multi_team_rap_text_text_stop.brlan.json5'),
        os.path.join('control', 'blyt', 'common_w207_multi_team_rap.brlyt.json5'),
        os.path.join('control', 'blyt', 'common_w207_multi_team_rap_text.brlyt.json5'),
        os.path.join('control', 'blyt', 'team_color_select_menu_common.brlyt.json5'),
        os.path.join('control', 'ctrl', 'TeamArrowLeft.brctr.json5'),
        os.path.join('control', 'ctrl', 'TeamArrowRight.brctr.json5'),
        os.path.join('control', 'ctrl', 'TeamUpDownText.brctr.json5'),
        os.path.join('control', 'ctrl', 'TeamUpDownValue.brctr.json5'),
        os.path.join('control', 'timg', 'fm_team_color_flag_0.tpl'),
        os.path.join('control', 'timg', 'fm_team_color_flag_1.tpl'),
        os.path.join('control', 'timg', 'fm_team_color_flag_2.tpl'),
        os.path.join('control', 'timg', 'fm_team_color_flag_3.tpl'),
        os.path.join('control', 'timg', 'fm_team_color_flag_4.tpl'),
        os.path.join('control', 'timg', 'fm_team_color_flag_5.tpl'),
        os.path.join('control', 'timg', 'fm_team_color_flag_6.tpl'),
        os.path.join('control', 'timg', 'fm_team_color_flag_7.tpl'),
        os.path.join('control', 'timg', 'fm_team_color_flag_8.tpl'),
        os.path.join('control', 'timg', 'fm_team_color_flag_9.tpl'),
        os.path.join('control', 'timg', 'fm_team_color_flag_10.tpl'),
        # Explanation text with 2 lines
        os.path.join('bg', 'blyt', 'obi_bottom.brlyt.json5'),
        os.path.join('bg', 'blyt', 'obi_bottom_message.brlyt.json5'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_curve_000.tpl'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_right_000.tpl'),
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
        os.path.join('control', 'ctrl', 'SettingUpDownButtonL.brctr.json5'),
        os.path.join('control', 'ctrl', 'SettingUpDownButtonR.brctr.json5'),
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
        # Teams
        os.path.join('button', 'ctrl', 'TeamConfirmOK.brctr.json5'),
        os.path.join('control', 'anim', 'common_w204_team_position.brlan.json5'),
        os.path.join('control', 'blyt', 'common_w204_team.brlyt.json5'),
        os.path.join('control', 'ctrl', 'TeamConfirm.brctr.json5'),
        # Multiplayer Character Select
        os.path.join('button', 'ctrl', 'CharacterSelect4_0_Multi.brctr.json5'),
        os.path.join('button', 'ctrl', 'CharacterSelect4_1_Multi.brctr.json5'),
        os.path.join('button', 'ctrl', 'CharacterSelect4_2_Multi.brctr.json5'),
        os.path.join('button', 'ctrl', 'CharacterSelect4_3_Multi.brctr.json5'),
        os.path.join('button', 'blyt', 'common_w117_mii_suit.brlyt.json5'),
        *pack_select_assets,
    ],
    os.path.join('Scene', 'UI', 'MenuOtherSP.arc.lzma'): [
        # Explanation text with 2 lines
        os.path.join('bg', 'blyt', 'obi_bottom.brlyt.json5'),
        os.path.join('bg', 'blyt', 'obi_bottom_message.brlyt.json5'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_curve_000.tpl'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_right_000.tpl'),
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
        os.path.join('control', 'ctrl', 'SettingUpDownButtonL.brctr.json5'),
        os.path.join('control', 'ctrl', 'SettingUpDownButtonR.brctr.json5'),
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
        os.path.join('button', 'ctrl', 'SettingsButton.brctr.json5'),
        # Explanation text with 2 lines
        os.path.join('bg', 'blyt', 'obi_bottom.brlyt.json5'),
        os.path.join('bg', 'blyt', 'obi_bottom_message.brlyt.json5'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_curve_000.tpl'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_right_000.tpl'),
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
        os.path.join('control', 'ctrl', 'SettingUpDownButtonL.brctr.json5'),
        os.path.join('control', 'ctrl', 'SettingUpDownButtonR.brctr.json5'),
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
        # Teams
        os.path.join('button', 'ctrl', 'TeamConfirmOK.brctr.json5'),
        os.path.join('control', 'anim', 'common_w204_team_position.brlan.json5'),
        os.path.join('control', 'blyt', 'common_w204_team.brlyt.json5'),
        os.path.join('control', 'ctrl', 'TeamConfirm.brctr.json5'),
        # Multiplayer Character Select
        os.path.join('button', 'ctrl', 'CharacterSelect4_0_Multi.brctr.json5'),
        os.path.join('button', 'ctrl', 'CharacterSelect4_1_Multi.brctr.json5'),
        os.path.join('button', 'ctrl', 'CharacterSelect4_2_Multi.brctr.json5'),
        os.path.join('button', 'ctrl', 'CharacterSelect4_3_Multi.brctr.json5'),
        os.path.join('button', 'blyt', 'common_w117_mii_suit.brlyt.json5'),
        # CourseDebug
        os.path.join('control', 'anim', 'CourseDebug.brlan.json5'),
        os.path.join('control', 'blyt', 'CourseDebug.brlyt.json5'),
        os.path.join('control', 'ctrl', 'CourseDebug.brctr.json5'),
        os.path.join('control', 'timg', 'download.tpl'),
        os.path.join('control', 'timg', 'ef_wave_indMip.tpl'),
        # Vanilla track manifests
        *pack_select_assets,
        os.path.join('vanillaTracks.pb.bin'),
        *[os.path.normpath(i) for i in sorted(glob.glob("vanillaTracks/*.pb.bin", root_dir="assets", recursive=True))],
        # Flags
        os.path.join('control', 'blyt', 'chara_flag_machine_picture_common.brlyt.json5'),
        *[os.path.normpath(i) for i in sorted(glob.glob("control/timg/[0-9][0-9][0-9].tpl", root_dir="assets", recursive=True))],
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
        os.path.join('button', 'ctrl', 'PauseMenuTimeAttackNoSS.brctr.json5'),
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
        os.path.join('control', 'ctrl', 'SettingUpDownButtonL.brctr.json5'),
        os.path.join('control', 'ctrl', 'SettingUpDownButtonR.brctr.json5'),
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
        os.path.join('game_image', 'anim', 'battle_total_point_off.brlan.json5'),
        os.path.join('game_image', 'anim', 'battle_total_point_on.brlan.json5'),
        os.path.join('game_image', 'anim', 'battle_total_point_stop.brlan.json5'),
        os.path.join('game_image', 'anim', 'chara_name_text_color_dummy.brlan.json5'),
        os.path.join('game_image', 'anim', 'common_w039_map_chara_icon_dummy.brlan.json5'),
        os.path.join('game_image', 'anim', 'game_image_lap_position.brlan.json5'),
        os.path.join('game_image', 'anim', 'game_image_speed_texture_pattern_0_9.brlan.json5'),
        os.path.join('game_image', 'blyt', 'battle_point_set_position.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'battle_total_point.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'coin.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'common_w039_map_chara_icon.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'debug_panel.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'game_image_lap.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'game_image_speed.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'InputDisplay.brlyt.json5'),
        os.path.join('game_image', 'blyt', 'race_message_half.brlyt.json5'),
        os.path.join('game_image', 'ctrl', 'balloon.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'BattleAddPoint.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'battle_total_point.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'DebugPanel.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'InputDisplay.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'item_window_new.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'lap_number.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'map_chara.brctr.json5'),
        os.path.join('game_image', 'ctrl', 'point.brctr.json5'),
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
        #MegaTC
        os.path.join('game_image', 'timg', 'MegaTC_icon.tpl'),
        os.path.join('game_image', 'blyt', 'item.brlyt.json5'),
        # Results
        os.path.join('result', 'anim', 'common_w204_result_team_position.brlan.json5'),
        os.path.join('result', 'anim', 'position_12players_color_off.brlan.json5'),
        os.path.join('result', 'anim', 'position_12players_c_off.brlan.json5'),
        os.path.join('result', 'anim', 'team_point_position.brlan.json5'),
        os.path.join('result', 'blyt', 'common_w204_team.brlyt.json5'),
        os.path.join('result', 'blyt', 'team_point.brlyt.json5'),
        os.path.join('result', 'ctrl', 'ResultTeam.brctr.json5'),
        os.path.join('result', 'ctrl', 'ResultTeamPoint.brctr.json5'),
    ],
    os.path.join('Scene', 'UI', 'TitleSP.arc.lzma'): [
        # Explanation text with 2 lines
        os.path.join('bg', 'blyt', 'obi_bottom.brlyt.json5'),
        os.path.join('bg', 'blyt', 'obi_bottom_message.brlyt.json5'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_curve_000.tpl'),
        os.path.join('bg', 'timg', 'tt_obi_bottom_right_000.tpl'),
        # License selection
        os.path.join('button', 'blyt', 'common_w076_license_icon_center.brlyt.json5'),
        os.path.join('button', 'ctrl', 'LicenseSelect.brctr.json5'),
        os.path.join('button', 'timg', 'tt_license_icon_004.tpl'),
        # Title page without online
        os.path.join('button', 'ctrl', 'TopMenuChannelWakuNoOnline.brctr.json5'),
        # Online renaming
        os.path.join('button', 'ctrl', 'TopMenuWifiWaku.brctr.json5'),
        # New title screen images
        os.path.join('title', 'timg', 'tt_title_screen_koopa.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_koopa_bokeboke.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_luigi.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_luigi_bokeboke.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_mario.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_mario_bokeboke.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_mario0.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_mario0_bokeboke.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_mario2.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_mario2_bokeboke.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_peachi.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_peachi_bokeboke.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_title_rogo_r_only.tpl'),
        os.path.join('title', 'timg', 'tt_title_screen_title_rogo_bokeboke.tpl'),
        os.path.join('title', 'blyt', 'title.brlyt.json5'), #Need these for consistency across all regions
        os.path.join('title', 'blyt', 'title_boke.brlyt.json5'),
        os.path.join('title', 'blyt', 'title_image_common.brlyt.json5'),
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
    asset_in_files[os.path.join('Scene', 'UI', f'GlobeSP_{language}.arc.lzma')] = [
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
            '.bin': '.bin',
            '.breff': '.breff',
            '.breft': '.breft',
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
                '.bin': 'cp',
                '.breff': 'cp',
                '.breft': 'cp',
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
if not devkitppc:
    raise SystemExit("DEVKITPPC environment variable not set")

n.variable('write', os.path.join('tools', 'write.py'))
n.variable('nanopb', os.path.join('vendor', 'nanopb', 'generator', 'nanopb_generator.py'))
n.variable('gcc', os.path.join(devkitppc, 'bin', 'powerpc-eabi-gcc'))
n.variable('compiler', os.path.join(devkitppc, 'bin', 'powerpc-eabi-gcc'))
n.variable('postprocess', 'postprocess.py')
n.variable('port', 'port.py')
n.variable('generate_symbol_map', 'generate_symbol_map.py')
n.variable('lzmac', 'lzmac.py')
n.variable('version', 'version.py')
n.variable('elf2dol', 'elf2dol.py')
n.newline()

n.rule(
    'write',
    command = f'{sys.executable} $write "$content" $out',
    description = 'WRITE $out',
)

n.rule(
    'nanopb',
    command = f'{sys.executable} $nanopb $in -I protobuf -L "#include <vendor/nanopb/%s>" -D build/protobuf -q',
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
    '-fshort-wchar',
    '-isystem', '.',
    '-isystem', 'include',
    '-isystem', 'payload',
    '-isystem', 'vendor',
    '-isystem', 'build',
    '-msdata=none',
    '-Wall',
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
    '-std=c++23',
    '-Wall',
    '-Wextra',
    '-Wno-delete-non-virtual-dtor',
    '-Wno-packed-bitfield-compat',
    '-Wsuggest-override',
]
if args.gdb_compatible:
    common_cflags += ['-DGDB_COMPATIBLE=1']
    common_ccflags += ['-DGDB_COMPATIBLE=1']

if args.ci:
    common_ccflags.append("-Werror")
    common_cflags.append("-Werror")
else:
    common_ccflags.append("-Werror=vla")
    common_cflags.extend((
        '-Werror=implicit-function-declaration',
        '-Werror=incompatible-pointer-types',
        '-Werror=vla',
    ))

target_cflags = {
    'stub': [
        '-DSP_STUB',
    ],
    'loader': [
        '-DSP_LOADER',
    ],
    'payload': [
        '-DSP_PAYLOAD',
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
if args.ci:
    common_ldflags.append('-Wl,--fatal-warnings')

n.rule(
    'S',
    command = '$gcc -MD -MT $out -MF $out.d $Sflags -c $in -o $out',
    depfile = '$out.d',
    deps = 'gcc',
    description = 'S $out',
)
n.newline()

n.rule(
    'incbin',
    command = '$gcc -DNAME=$name -DPATH=$path -c Incbin.S -o $out',
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
    command = '$gcc $ldflags $in -o $out',
    description = 'LD $out',
)
n.newline()

n.rule(
    'generate_symbol_map',
    command = f'{sys.executable} $generate_symbol_map $in $out',
    description = 'SMAP $out',
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

feature_dirs = []
feature_hh_files = []
for feature in features:
    content = '#pragma once\n'
    content += '\n'
    content += '#define ENABLE_'
    content += feature.upper().replace('-', '_')
    content += ' '
    content += str(vars(args)[feature.replace('-', '_')]).lower()
    content += '\n'
    feature_dir = os.path.join('$builddir', 'features', feature.replace('-', '_'))
    feature_dirs += [feature_dir]
    feature_hh_file = os.path.join(feature_dir, feature.title().replace('-', '') + '.hh')
    feature_hh_files += [feature_hh_file]
    n.build(
        feature_hh_file,
        'write',
        variables = {
            'content': repr(content)[1:-1],
        },
        implicit = '$write',
    )
n.newline()

protobuf_proto_files = [
    os.path.join('protobuf', 'Login.proto'),
    os.path.join('protobuf', 'Matchmaking.proto'),
    os.path.join('protobuf', 'TrackPacks.proto'),
    os.path.join('protobuf', 'NetStorage.proto'),
    os.path.join('protobuf', 'Room.proto'),
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
        os.path.join('common', 'TQuat.S'),
        os.path.join('common', 'TQuat.cc'),
        os.path.join('common', 'TVec3.S'),
        os.path.join('common', 'TVec3.cc'),
        os.path.join('common', 'VI.cc'),
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
        os.path.join('vendor', 'tjpgd', 'tjpgd.c'),
        *sorted(glob.glob("vendor/bzip2/*.c")),
        *sorted(glob.glob("payload/**/*.cc", recursive=True)),
        *sorted(glob.glob("payload/**/*.c", recursive=True)),
        *sorted(glob.glob("payload/**/*.S", recursive=True)),
    ],
    'loader': [
        os.path.join('common', 'Clock.cc'),
        os.path.join('common', 'Console.cc'),
        os.path.join('common', 'DCache.cc'),
        os.path.join('common', 'Font.c'),
        os.path.join('common', 'Font.cc'),
        os.path.join('common', 'FS.cc'),
        os.path.join('common', 'ICache.cc'),
        os.path.join('common', 'IOS.cc'),
        os.path.join('common', 'Memcmp.c'),
        os.path.join('common', 'Memcpy.c'),
        os.path.join('common', 'Memset.c'),
        os.path.join('common', 'Strlcpy.c'),
        os.path.join('common', 'Strlen.c'),
        os.path.join('common', 'VI.cc'),
        os.path.join('vendor', 'sha1', 'sha1.c'),
        *sorted(glob.glob("loader/**/*.cc", recursive=True)),
        *sorted(glob.glob("loader/**/*.c", recursive=True)),
        *sorted(glob.glob("loader/**/*.S", recursive=True)),
    ],
    'stub': [
        os.path.join('common', 'Clock.cc'),
        os.path.join('common', 'Console.cc'),
        os.path.join('common', 'DCache.cc'),
        os.path.join('common', 'Font.c'),
        os.path.join('common', 'Font.cc'),
        os.path.join('common', 'FS.cc'),
        os.path.join('common', 'ICache.cc'),
        os.path.join('common', 'IOS.cc'),
        os.path.join('common', 'Memcpy.c'),
        os.path.join('common', 'Memset.c'),
        os.path.join('common', 'Strlcpy.c'),
        os.path.join('common', 'Strlen.c'),
        os.path.join('common', 'VI.cc'),
        os.path.join('vendor', 'lzma', 'LzmaDec.c'),
        *sorted(glob.glob("stub/**/*.cc", recursive=True)),
        *sorted(glob.glob("stub/**/*.c", recursive=True)),
        *sorted(glob.glob("stub/**/*.S", recursive=True)),
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
                        *['-isystem ' + feature_dir for feature_dir in feature_dirs],
                    ]),
                },
                order_only = [
                    *(feature_hh_files if target == 'payload' and ext[1:] == 'cc' else []),
                    *(protobuf_h_files if target == 'payload' else []),
                ],
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
                'P': '0x8076F000' if not args.gdb_compatible else '0x809C4FA0',
                'E': '0x8076A000',
                'J': '0x8076E000',
                'K': '0x8075D000',
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

loader_base = 0x80e00000 if args.gdb_compatible else 0x80b00000

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
                    f'-Wl,--defsym,base={loader_base}',
                    '-Wl,--entry=start',
                    f'-Wl,--oformat,{fmt}',
                    '-Wl,-T,' + os.path.join('common', 'RMC.ld'),
                ]),
            },
            implicit = os.path.join('common', 'RMC.ld'),
        )
        n.newline()

for region in ['P', 'E', 'J', 'K']:
    for profile in ['DEBUG', 'RELEASE']:
        suffix = 'D' if profile == 'DEBUG' else ''
        in_file = os.path.join('$builddir', 'bin', f'payload{region}{suffix}.elf')
        out_file = os.path.join('$builddir', 'bin', f'payload{region}{suffix}.SMAP')
        n.build(
            out_file,
            'generate_symbol_map',
            in_file,
        )
        n.newline()

for region in ['P', 'E', 'J', 'K']:
    for profile in ['DEBUG', 'RELEASE']:
        suffix = 'D' if profile == 'DEBUG' else ''
        in_file = os.path.join('$builddir', 'bin', f'payload{region}{suffix}.SMAP')
        out_file = os.path.join(
            '$builddir',
            'contents.arc.d',
            'bin',
            f'payload{region}{suffix}.SMAP.lzma'
        )
        n.build(
            out_file,
            'lzmac',
            in_file,
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
        *[os.path.join('$builddir', 'contents.arc.d', target) for target in thumbnail_in_files],
        *[os.path.join('$builddir', 'contents.arc.d', target) for target in asset_out_files],
        os.path.join('$builddir', 'contents.arc.d', 'bin', f'payloadP{in_suffix}.SMAP.lzma'),
        os.path.join('$builddir', 'contents.arc.d', 'bin', f'payloadE{in_suffix}.SMAP.lzma'),
        os.path.join('$builddir', 'contents.arc.d', 'bin', f'payloadJ{in_suffix}.SMAP.lzma'),
        os.path.join('$builddir', 'contents.arc.d', 'bin', f'payloadK{in_suffix}.SMAP.lzma'),
        os.path.join('$builddir', 'contents.arc.d', 'bin', f'loader{in_suffix}.bin.lzma'),
        os.path.join('$builddir', 'contents.arc.d', 'bin', f'version{out_suffix}.bin'),
        os.path.join('$builddir', 'contents.arc.d', 'banner.bin'),
    ]
    for region in ['P', 'E', 'J', 'K']:
        in_paths += [os.path.join(
            '$builddir',
            'contents.arc.d',
            'bin',
            f'payload{region}{in_suffix}.SMAP.lzma',
        )]
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
                '--renamed payloadPD.SMAP.lzma payloadP.SMAP.lzma',
                '--renamed payloadED.SMAP.lzma payloadE.SMAP.lzma',
                '--renamed payloadJD.SMAP.lzma payloadJ.SMAP.lzma',
                '--renamed payloadKD.SMAP.lzma payloadK.SMAP.lzma',
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
n.default(["test"])

if args.dry:
    with open('build.ninja', 'w') as out_file:
        out_file.write(out_buf.getvalue())

    raise SystemExit

out_file = tempfile.NamedTemporaryFile("w+", delete=False)
out_file.write(out_buf.getvalue())
out_file.close()
n.close()

try:
    proc = subprocess.run(("ninja", "-f", out_file.name, *ninja_argv))
except KeyboardInterrupt:
    returncode = 130 # 128 + SIGINT
else:
    returncode = proc.returncode

os.remove(out_file.name)
sys.exit(returncode)
