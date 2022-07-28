---
layout: post
title:  "Announcing Mario Kart Wii - Service Pack (MKW-SP)"
date:   2022-01-30 21:00:00 +0100
author: stebler
---

Today I'm happy to announce that Mario Kart Wii - Service Pack (MKW-SP), a new MKW mod that aims to provide various features and quality of life improvements across most gamemodes, has reached version 0.1, its first public release. In this blog post I will first explain my general goals and vision with MKW-SP, and then present a few highlights of version 0.1 which focuses on time trial replays.

## Goals

MKW-SP is **fully open-source** and the code (including the whole history) and the issue tracker are available [on Github](https://github.com/stblr/mkw-sp), and so is my [Ghidra repository](https://drive.google.com/drive/folders/1I1VRfeut3NtPeddePutfAaZhduVdKhhc?usp=sharing). That means that anyone is free (and welcome) to implement new features. Feedback is also very appreciated and will be listened to.

MKW-SP is **cross-platform** and supports all variants of the Wii, the Wii U, and Dolphin, as well as all 4 versions of the game, without feature disparity. If a new platform emerges I'm also open to support it if the effort is reasonable.

MKW-SP aims to be **complete**. Adding features should generally not mean removing others. It is fully monolithic which means that potential conflicts between patches can be explicitly handled, and as such code extensions of any kind are strongly discouraged. Most existing QoL improvements are or will be integrated in it, some of them can be made optional via the in-game settings.

## Non-goals

Being as close as possible to vanilla MKW is a non-goal. If something is considered a bug, even just from a playability perspective (e.g. the thundercloud online), then it will be fixed. As such being a platform for time trials on Nintendo tracks is also a non-goal.

There are currently no plans for campaign gamemodes (grand prix and mission mode) in MKW-SP because I have little interest in writing code for them, but if you are a developer and want to contribute features related to them I'm still open to it.

## Roadmap

Version **0.1**, releasing today, focuses mainly on time trials and on the new save system, and lays the foundations such as the patching system or the porting and asset compilation tools which are needed to have a functional mod.

The main feature of **0.2** will be better support for custom tracks. Right now custom tracks are only support via My Stuff, but with 0.2 it will be possible to create track packs and switch between them via the in-game menu. A more flexible replacement for the "slots" system is also planned. An in-game updater will be added, which will be able to handle both MKW-SP and track packs.

Version **0.3** will bring online support to MKW-SP. Instead of the P2P architecture of the vanilla game, a custom client-server system will be implemented. This will allow for additional features such as cheat prevention, lag-free recordings and spectator mode, and reconnections in the middle of a race. With "open rooms", players will be able to choose between current games with their desired settings. Official servers will be provided, but any MKW-SP installation will have the ability to switch to server mode via an in-game option or a configuration file. On top of that the team mode will be enhanced to optionally have more than 2 teams, friendly fire and a way to coordinate without an external medium.

The plans for **0.4** are not set in stone but it might feature additional gamemodes and improvements to battle mode.

## Highlights of 0.1

Arguably the most exciting feature of 0.1 is the ability to have up to 11 ghosts on the track at once. In replay mode, it is possible to switch between them like in live view, and the game will wait for all ghosts to finish to end the race. The cinematic camera has been replaced by the rear view camera, which is better suited for comparisons. The UI has been accordingly adapted to display a clearer overview of which ghosts are selected, and ghosts can be sorted by finish time or creation date. The content of the ghost tags can be configured to display the finish time or the date instead of the name. They can be shown for all, focused only or no ghost. A similar setting controls the opacity of ghosts. The multi-ghost feature can be seen in [video](https://www.youtube.com/watch?v=KF3y3nQneBo).

![3 screenshots of the multi-ghost feature](/assets/0.1-mg.png)

The rksys.dat file is not used by MKW-SP. Instead, ghosts are saved as separate files on the SD card along with the custom settings. This means that more than 4 licenses can be supported, although the UI can currently only handle up to 6. The custom settings are located either in a gamemode-specific UI or in the license options. Ghosts contain additional metadata such as a hash of the track, the engine class, more precise finish times and detect glitches (partial read-only support for the CTGP format is also there). Both the save and the ghost formats have been designed to be forward and backward compatible.

Additional features include a fully native speedometer, the 200cc speed mod and an option to show Mii heads on the minimap. The full list can be found on [the dedicated page](/features).

![Screenshots of the UI for license selection, time trial rules and ghost selection](/assets/0.1-ls.png)

## Closing thoughts

Deciding when to release is always a difficult choice. Releasing late allows to work on tasks out-of-order, and to benefit from the effect of surprise, while releasing early means getting feedback and potentially help. With the relatively niche audience of version 0.1, I'm rather doing the latter choice here. Criticism and suggestions can be expressed both on GitHub and on the [Discord server](https://discord.gg/TPSKtyKgqD).

I would like to thank CLF78 for his help with the initial release.
