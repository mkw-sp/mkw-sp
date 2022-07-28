---
layout: post
title:  "MKW-SP Programming Tutorial - Part 2"
date:   2022-07-08 20:00:00 +0200
author: stebler
---

In this second part, our goal will be to add a new setting to MKW-SP to control item music. It is often frustrating as a MKW player to be enjoying a good background music for it to be interrupted by the Star or Mega one. That's why we want to give users an option to change that behavior. If you did not read the first part yet, you can find it [here]({% post_url 2022-05-26-mkw-sp-programming-tutorial-part-1 %}).

## Reverse-engineering

### Ghidra

The main tool that we are using for understanding the internals of the game is [Ghidra](https://ghidra-sre.org/) with the [GameCube loader](https://github.com/Cuyler36/Ghidra-GameCube-Loader) which most importantly supports the SIMD extensions of the Gekko and the Broadway. A repo with many MKW functions and structures is available [here](https://drive.google.com/drive/folders/1I1VRfeut3NtPeddePutfAaZhduVdKhhc?usp=sharing)[^server]. You will need a RAM dump of the PAL version of the game, which you can directly do in Dolphin. Instructions to import it are included. If you are not familiar with Ghidra, some tutorials are available on the Internet, although I don't have a specific recommendation. It's relatively straightforward to use overall.

### Finding the relevant code

We first try to find it starting from the known Mega-related symbols. By searching "mega" in the Symbol Tree pane, we find 5 functions and 1 global. The first one, `__sinit_ItemObjMega_cpp`, is a C++ [static constructor](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0421r0.html) function, so that's certainly not it. The second one, `ItemHolderPlayer_activateMega`, just calls another function. The third one, `ItemHolderPlayer_useMega`, calls the previous one and 2 other functions, one to update the inventory, and the other is only called during online gameplay. The fourth one, `ItemObjMega_makeArray`, is for the initialization of the item objects. Finally, `PlayerSub10_activateMega`, might be actually interesting:

```c
void PlayerSub10_activateMega(PlayerSub10 *playerSub10) {
    uint uVar1;
    PlayerStats *pPVar2;
    PlayerSub *pPVar3;
    PlayerSnd *pPVar4;
    PlayerSub1c *pPVar5;
    undefined4 in_GQR0;
    undefined8 in_f31;
    double dVar6;
    undefined local_8 [8];

    pPVar5 = playerSub10->playerPointers->playerSub1c;
    pPVar5->bitfield2 = pPVar5->bitfield2 | IN_A_MEGA;
    playerSub10->megaTimer = 450;
    uVar1 = PlayerPointers_getPlayerIdx(&playerSub10->playerPointers);
    FUN_806aaba8(ptr_effectInfo->players[uVar1 & 0xff]);
    FUN_805907b0(&playerSub10->playerPointers);
    pPVar2 = PlayerPointers_getPlayerStats(&playerSub10->playerPointers);
    dVar6 = (double)pPVar2->megaScale;
    pPVar3 = PlayerPointers_getPlayerSub(&playerSub10->playerPointers);
    FUN_80598338(dVar6,&pPVar3->playerPointers);
    pPVar4 = PlayerPointers_getSnd(&playerSub10->playerPointers);
    (**(code **)(*(int *)pPVar4 + 0xe0))(0,pPVar4,0x111);
    pPVar4 = PlayerPointers_getSnd(&playerSub10->playerPointers);
    if (pPVar4->isReal != '\0') {
        FUN_8070feb0(_DAT_809c27f0,0);
    }
    if ((playerSub10->playerPointers->playerSub1c->bitfield2 & SHOCKED) != 0) {
        pPVar5 = playerSub10->playerPointers->playerSub1c;
        pPVar5->bitfield2 = pPVar5->bitfield2 & ~SHOCKED;
        pPVar4 = PlayerPointers_getSnd(&playerSub10->playerPointers);
        (**(code **)(*(int *)pPVar4 + 0xe0))(0,pPVar4,0x116);
        playerSub10->shockTimer = 0;
        FUN_8056b168(playerSub10->field163_0x260,0);
    }
    if ((playerSub10->playerPointers->playerSub1c->bitfield2 & CRUSHED) != 0) {
        pPVar5 = playerSub10->playerPointers->playerSub1c;
        pPVar5->bitfield2 = pPVar5->bitfield2 & 0xfffcffff;
        pPVar4 = PlayerPointers_getSnd(&playerSub10->playerPointers);
        (**(code **)(*(int *)pPVar4 + 0xe0))(0,pPVar4,0x1a5);
        FUN_8056b094(playerSub10->field163_0x260);
    }
    FUN_805917c0(&playerSub10->playerPointers,0x14);
    FUN_8059041c(1.0,&playerSub10->playerPointers,0x1b,1);
    FUN_8056afb4(playerSub10->field163_0x260,1);
    playerSub10->field101_0x196 = 0;
}
```

Several sound-related functions are called by it. We can easily infer that the numbers passed as the third argument of the virtual function at offset 0xe0 of the vtable of `PlayerSnd` are sound ids (the full list is available [here](https://github.com/stblr/mkw-sp/blob/main/payload/game/sound/SoundId.hh). All names are stored in `revo_kart.brsar` (BRSAR meaning Binary Revolution Sound ARchive). Thus we know that `SE_ITM_BIG_KINOKO_USE` is played unconditionally, `SE_ITM_THNDR_BIG` if the player was shocked, and `SE_VCL_DMG_PRESS_REV` if the player was crushed. All of them are short state transition sounds. There is one more use of `PlayerSnd` directly in this function, to check if the player is a real player[^cpus], in which case, `FUN_8070feb0` is called. If we go to that function, we see that there is a second reference to it from `PlayerSub10_activateStar`, although with 1 instead of 0 as the second argument. That automatically makes it a very good candidate. The first argument, `_DAT_809c27f0`, is a singleton. By looking at the writes to the global, we can easily find that its instance creation function is at `FUN_8070fa08`. It only has a few member functions (until about 80710340). There are 2 numbers in `FUN_8070fd10` which if interpreted as sound ids correspond to `SE_ITM_BIG_KINOKO_STATE` and `SE_ITM_STAR_STATE` respectively, which confirms that we are indeed at the right place.

We now name this singleton `ItemMusicManager` and add a few of its fields and functions. The most important ones are `FUN_8070fd10` (now `ItemMusicManager_calc`) which is called once per frame and applies the actual sound changes, and `FUN_8070fec8` (now `ItemMusicManager_resolve`) which runs some kind of state machine to decide the next state (field at 0x10) which will then be used by the `calc` function. There are 4 possible states: `0` for the Mega music, `1` for the Star music, `3` for the idle state. The state `2` seems to apply a sine transformation to something, so we can guess that it is for the shocked/squashed state. As for the patching, we will need to set the state to idle after the `resolve` function depending on the settings. For reference, here is a partially annotated version of `ItemMusicManager_calc`:

```c
void ItemMusicManager_calc(ItemMusicManager *this) {
    float fVar1;
    ItemMusicManagerState IVar2;
    int iVar3;
    double dVar4;

    iVar3 = FUN_80713dcc();
    if ((iVar3 == 0) && (*(int *)(_DAT_809c2328 + 0x1c) != 0)) {
        ItemMusicManager_resolve(this);
        IVar2 = this->state;
        if (IVar2 == MEGA) {
            this->field27_0x20 = 0.01;
            this->field28_0x24 = 1.0;
            (**(code **)(*_DAT_809c2898 + 0xb4))(_DAT_809c2898,&this->field_0x18,0x112);
        }
        else if (IVar2 == STAR) {
            this->field27_0x20 = 0.01;
            this->field28_0x24 = 1.0;
            (**(code **)(*_DAT_809c2898 + 0xb4))(_DAT_809c2898,&this->field_0x18,0x10e);
        }
        else if (IVar2 == DAMAGE) {
          fVar1 = this->field29_0x28 + 0.418879;
          this->field29_0x28 = fVar1;
          dVar4 = SinFIdx((double)(fVar1 * 40.74366));
          if (*(int *)&PTR_809c27f8->field_0x40 == 4) {
              this->field28_0x24 = (float)(dVar4 * 0.1000000014901161) + 1.0;
          }
          else {
              this->field28_0x24 = (float)(dVar4 * 0.07000000029802322) + 1.0;
          }
          this->field27_0x20 = 1.0;
        }
        else {
            this->field28_0x24 = 1.0;
            this->field29_0x28 = 0.0;
            this->field27_0x20 = 1.0;
        }
        FUN_806f9e78((double)this->field27_0x20,_DAT_809c232c,1,0);
        FUN_806f9e78((double)this->field28_0x24,_DAT_809c232c,2,0);
    }
}
```

## Implementing the feature

### Compiling MKW-SP

You can simply follow the instructions on the README.

### Adding a new setting

We start by adding strings for the new setting. As a reminder, [here](/assets/t2-settings.png) is a screenshot of the MKW-SP settings UI. MKW uses the [BMG file format](https://wiki.tockdom.com/wiki/BMG_(File_Format)) for storing them and handling localization (one BMG file per language). Here we only need to care about NTSC-U: translations are handled thanks to the [MK Translation Project](https://wiki.tockdom.com/wiki/Translation_Project). We open `assets/MenuSP_U.bmg.json5` and add:

```diff
@@ -463,4 +463,36 @@
         "font": "regular",
         "string": "Always show the position tracker.",
     },
+    "10126": {
+        "font": "regular",
+        "string": "Sound",
+    },
+    "10127": {
+        "font": "regular",
+        "string": "Item Music",
+    },
+    "10128": {
+        "font": "regular",
+        "string": "None",
+    },
+    "10129": {
+        "font": "regular",
+        "string": "Shocked/Squashed Only",
+    },
+    "10130": {
+        "font": "regular",
+        "string": "All",
+    },
+    "10131": {
+        "font": "regular",
+        "string": "Do not let items affect the music.",
+    },
+    "10132": {
+        "font": "regular",
+        "string": "Only apply the shocked/squashed music effect.",
+    },
+    "10133": {
+        "font": "regular",
+        "string": "Let stars, mega mushrooms and lightnings affect the music.",
+    },
 }
```

Each setting is part of a category, we decide to a add a new one for sound settings. Then we add a the name of our option, the 3 options and a description for each option.

We then add the new category, setting and options to `payload/sp/settings/ClientSettings.hh`:

```diff
@@ -13,6 +13,9 @@ enum class Setting {
     InputDisplay,
     RankControl,
 
+    // Sound
+    ItemMusic,
+
     // TA
     TAClass,
     TAGhostSorting,
@@ -30,6 +33,7 @@ enum class Setting {
 
 enum class Category {
     Race,
+    Sound,
     TA,
     License,
 };
@@ -64,6 +68,12 @@ enum class RankControl {
     Always,
 };
 
+enum class ItemMusic {
+    None,
+    DamageOnly,
+    All,
+};
+
 enum class TAClass {
     CC150,
     CC200,
@@ -154,6 +164,11 @@ struct Helper<ClientSettings::Setting, ClientSettings::Setting::RankControl> {
     using type = SP::ClientSettings::RankControl;
 };
 
+template <>
+struct Helper<ClientSettings::Setting, ClientSettings::Setting::ItemMusic> {
+    using type = SP::ClientSettings::ItemMusic;
+};
+
 template <>
 struct Helper<ClientSettings::Setting, ClientSettings::Setting::TAClass> {
     using type = SP::ClientSettings::TAClass;
```

The last part is used to bind the setting enum value and the new enum containing the options for some helper functions.

We then configure the new category and setting in `payload/sp/settings/ClientSettings.cc`:

```diff
@@ -6,7 +6,7 @@ namespace SP::ClientSettings {
 
 const char name[] = "MKW-SP Settings";
 
-const u32 categoryMessageIds[] = { 10118, 10119, 10120 };
+const u32 categoryMessageIds[] = { 10118, 10126, 10119, 10120 };
 
 const Entry entries[] = {
     [static_cast<u32>(Setting::DriftMode)] = {
@@ -69,6 +69,16 @@ const Entry entries[] = {
         .valueMessageIds = (u32[]) { 10122, 10123 },
         .valueExplanationMessageIds = (u32[]) { 10124, 10125 },
     },
+    [static_cast<u32>(Setting::ItemMusic)] = {
+        .category = Category::Sound,
+        .name = magic_enum::enum_name(Setting::ItemMusic),
+        .messageId = 10127,
+        .defaultValue = static_cast<u32>(ItemMusic::All),
+        .valueCount = magic_enum::enum_count<ItemMusic>(),
+        .valueNames = magic_enum::enum_names<ItemMusic>().data(),
+        .valueMessageIds = (u32[]) { 10128, 10129, 10130 },
+        .valueExplanationMessageIds = (u32[]) { 10131, 10132, 10133 },
+    },
     [static_cast<u32>(Setting::TAClass)] = {
         .category = Category::TA,
         .name = magic_enum::enum_name(Setting::TAClass),
```

For the category, we just need to define the message id previously added to the BMG. For the setting, we notably have to provide a default value.

The settings page automatically reads all these values, so we don't have to touch UI code at all.

### Patching `ItemMusicManager`

We first put a basic class declaration in `payload/game/snd/ItemMusicManager.hh`:

```c++
#pragma once

#include <Common.hh>

namespace Sound {

class ItemMusicManager {
private:
    enum class State {
        KinokoBig = 0,
        Star = 1,
        Damage = 2,
        Idle = 3,
    };

    void REPLACED(resolve)();
    REPLACE void resolve();

    u8 _00[0x10 - 0x00];
    State m_state;
    u8 _14[0x2c - 0x14];
};
static_assert(sizeof(ItemMusicManager) == 0x2c);

} // namespace Sound
```

We only care about one field, for the rest we just need to have the correct sizes and offsets (that syntax is used all around MKW-SP). We also put a static assertion to avoid mistakes.

The `REPLACE` attribute means that it's a game function that we are going to replace with our own, and the `REPLACED` macro makes then original version of the function available.

We now add create  `payload/game/snd/ItemMusicManager.cpp` and add:

```c++
#include "ItemMusicManager.hh"

#include "game/system/SaveManager.hh"

namespace Sound {

void ItemMusicManager::resolve() {
    REPLACED(resolve)();

    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::ItemMusic>();

    switch (m_state) {
    case State::KinokoBig:
    case State::Star:
        if (setting != SP::ClientSettings::ItemMusic::All) {
            m_state = State::Idle;
        }
        break;
    case State::Damage:
        if (setting == SP::ClientSettings::ItemMusic::None) {
            m_state = State::Idle;
        }
        break;
    default:
        break;
    }
}

} // namespace Sound
```

We first call the original function, the check the value of the setting and do appropriate changes to the state.

We can now add the file to the build system via `configure.py`:

```diff
@@ -783,6 +783,7 @@ code_in_files = {
         os.path.join('payload', 'game', 'rel', 'Rel.S'),
         os.path.join('payload', 'game', 'scene', 'globe', 'GlobeManager.S'),
         os.path.join('payload', 'game', 'snd', 'DriverSound.S'),
+        os.path.join('payload', 'game', 'snd', 'ItemMusicManager.cc'),
         os.path.join('payload', 'game', 'snd', 'KartSound.S'),
         os.path.join('payload', 'game', 'snd', 'Snd.S'),
         os.path.join('payload', 'game', 'system', 'BugCheck.c'),
```

Then build MKW-SP again using `ninja`. It will complain about a missing address for `_ZN5Sound16ItemMusicManager7resolveEv` (mangled version of the function we are replacing), which we need to provide (for the PAL version only) in `symbols.txt`:

```diff
@@ -851,6 +851,7 @@
 0x80707534 KartSound_init
 0x80708b44 KartSound_80708b44
 0x8070b250 KartSound_8070b250
+0x8070fec8 _ZN5Sound16ItemMusicManager7resolveEv
 0x80716170 Snd_80716170
 0x8071e270 JugemManager_createInstance
 0x8071e480 JugemManager_createJugems
```

Finally we can [test](/assets/t2-test.png) our feature by running `out/test/boot.dol` on Dolphin or on console. It seems to work fine so far.

The feature has been added to MKW-SP in [this commit](https://github.com/stblr/mkw-sp/commit/93e5b161cd1f0e54e15f1a6df781c8d3bbefe8a2).

[^server]: The MKW-SP project hosts a shared Ghidra server which is more up to date and complete, but for legal reasons you will have to contact an administrator if you want to access it. For this tutorial, the version linked above is more than enough.
[^cpus]: The music doesn't change if a CPU or a remote player uses a Mega.
