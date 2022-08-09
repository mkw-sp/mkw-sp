---
layout: default
title: Features
permalink: /features/
---

# Features

## Time Trials

**11-Way Multi-Ghost** [stebler]<br />
It is possible to race against or watch up to [11 ghosts at once](/assets/11-ghosts.png). One can switch between ghosts during a replay (similar to live view). The ghost selection UI has been [accordingly remade from scratch](/assets/ghost-selection.png).

**Vanilla Mode** [stebler]<br />
With competitive time trials in mind, most MKW-SP enhancements can be disabled.

**Rear View In Replay** [stebler]<br />
In ghost replay mode, cinematic view is [replaced by rear view](/assets/rear-view.png).

**Position Tracker** [stebler]<br />
The position tracker can be enabled in time trials mode.

**Ghost Settings** [stebler, riidefi]<br />
Tag count, tag content, opacity and sound can be configured.

**Additional Ghost Metadata** [stebler, Chadderz, MrBean35000vr, CLF78]<br />
The sha-1 of the track, whether 200cc was enabled, more precise finish times and detected glitches are saved at the end of the ghost file. The first two properties are used to only show ghosts for the correct track and engine class. The equivalent fields can be read from CTGP ghosts for the same purpose. The sha-1 computation doesn't take \_Dif files into account, allowing for e.g. custom textures.

## Race

**200cc** [MrBean35000vr]<br />
A speed class with a 1.5x speed multiplier, brake drifting and fast falling is supported in time trials and VS.

**Tag Limit Increase** [stebler]<br />
Up to 12 tags can be shown at once (from 3 in the vanilla game). The limit can be configured in the settings.

**Pause Menu Improvements** [riidefi, stebler]<br />
Additional buttons have been added to the pause menus, notably one to change the settings and another to go back directly to the ghost selection.

**4:3 FOV In 16:9** [Brandondorf9999]<br />
The wider 4:3 field-of-view can be used in 16:9 mode.

## HUD

**Speedometer** [stebler, Syara]<br />
The speed of the player is shown in the HUD, including [moving road](/assets/som-moving-road.png) and [moving water](/assets/som-moving-water.png). Split-screen [is supported](/assets/som-split-screen.png). The "SPEED" label has been translated into 5 languages.

**Input Display** [riidefi, stebler, LydeumZ]<br />
_License setting._ The controller inputs can be shown in the HUD. Split-screen and 200cc are supported.

**Hidden HUD Labels** [stebler]<br />
The TIME, LAP and SPEED HUB labels can be hidden.

**Mii Heads On Minimap** [stebler]<br />
For local players and ghosts, Miis can replace characters as map icons.

## Sound

**Item Music Setting** [stebler]<br />
The Mega Mushroom/Star music as well as the effect that's applied when shocked or crushed can be disabled.

**Final Lap Settings** [stebler, Melg, CLF78]<br />
The final lap jingle and speedup can be configured.

## My Stuff

**Layered Archives** [stebler]<br />
For tracks and UI files, it is possible to replace only certain subfiles by adding the \_Dif suffix to the archive name (e.g. `koopa_course_Dif.szs` or `MenuSingle_E_Dif.szs`).

**LZMA Compression** [stebler]<br />
The standalone LZMA format can be used in place of YAZ0, allowing for smaller file sizes.

**Random BRSTM** [stebler]<br />
Multiple BRSTMs can be placed in a folder with the name of the replaced files and will be picked at random.

**BRSTM Fixup** [stebler]<br />
BRSTMs that are too long or have the wrong number of tracks will not cut off.

**Sound Archive Subfile Replacement** [stebler]<br />
Individual sounds from `revo_kart.brsar` can be replaced by BRSTMs.

## Misc

**In-Game Updater** [stebler, Star, Palapeli, Zach]<br />
MKW-SP can be updated in-game.

**Wii Menu Channel** [stebler, Palapeli, Zach]<br />
An MKW-SP channel can now be installed to the Wii Menu. It functions as a shortcut.

**Custom Save System** [stebler, riidefi]<br />
The rksys.dat format is replaced with per-license ini files. More than 4 licenses (for now the UI can [handle up to 6](/assets/6-licenses.png)) and [additional settings](/assets/license-settings.png) are supported. Ghosts are moved to separate files and everything is always unlocked. All the files are stored on the SD or USB device.

**Translations** [MK Translation Project, stebler]<br />
All languages (plus Dutch) are supported in all versions of the game. Some of the MKW-SP-specific strings are also translated.

**Storage Benchmark** [stebler]<br />
The speed of SD/USB devices can be tested in various workloads.

**Instant Menu Transitions** [riidefi]<br />
Menu transitions can be disabled. This allows for faster navigation.

**Security Mitigations** [Star]<br />
Several security mitigations such as DEP, stack canaries or ASLR are implemented to make the game harder to exploit by malicious custom files or in case of a vulnerability in the networking code.

**No NTSC-K Region-Lock** [stebler, Palapeli]<br />
The NTSC-K game can be player on global consoles.

**Performance Overlay** [stebler]<br />
The CPU, GPU and RAM usage can be displayed as bars on the screen.

**Improved Logging And Crash Reports** [stebler, riidefi, Star]<br />
A log is stored to a file on the SD/USB and crash reports contain more details. Some bugs in custom tracks have specific checks with associated explanation messages.

## Limitations and known bugs

Ghost solidity is off by one frame when switching. See [issue 44](https://github.com/stblr/mkw-sp/issues/44).

VS and battle settings aren't saved. See [issue 50](https://github.com/stblr/mkw-sp/issues/50).

On courses with the `pylon01` object, time trials with more than 4 players will desync. See [issue 94](https://github.com/stblr/mkw-sp/issues/94).

Most gecko codes won't work because of the different memory map. A warning is shown on the title screen if they are enabled. When memory protection is implemented, support will be dropped altogether.
