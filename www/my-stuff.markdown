---
layout: default
title: My Stuff
permalink: /docs/my-stuff/
---

# My Stuff

## Sound Archive Subfile Replacement (SASR)

### Usage

On MKW-SP v0.1.9 and later, it is possible to replace any sound from `revo_kart.brsar` with an external BRSTM. BRSTMs must be named after the [sound id](https://github.com/mkw-sp/mkw-sp/blob/main/payload/game/sound/SoundId.hh) and put in the `sasr` (or `sound/sasr`) folder in one of your `My Stuff` folders (for instance, `/mkw-sp/My Stuff/sasr/270.brstm` to replace the Star music).

The number of channels (mono or stereo) is determined at runtime and can be different from the original sound.

To convert a BRSAR sound to a BRSTM, you can use the [BRSAR tool](/tools/brsar) to extract the relevant BRWAR, and then the [BRWAR tool](/tools/brwar) to export BRWAVs as BRSTMs.

### Limitations

- Only the folder from the highest priority My Stuff folder will be picked.
- The random BRSTM feature cannot be used for these files.
- Advanced control features, such as multiple sounds with a single id, may not work.
