#include "sp/WU8Library.hh"

#include "sp/ScopeLock.hh"
#include "sp/U8Iterator.hh"
#include "sp/YAZDecoder.hh"
#include "sp/storage/Storage.hh"

#include <egg/core/eggExpHeap.hh>
#include <game/system/SaveManager.hh>

#include <algorithm>
#include <cstring>

namespace SP {

const char *s_beginner_course_files[] = {"FlagB2.brres", "MiiKanban.brres", "MiiObj01.brres",
        "MiiObj02.brres", "MiiObj03.brres", "MiiStatueL3.brres", "dokan_sfc.brres",
        "effect/Flash_L/rk_flashBeginner.breff", "effect/Flash_L/rk_flashBeginner.breft",
        "effect/Hanabi/rk_stHanabi.breff", "effect/Hanabi/rk_stHanabi.breft", "itembox.brres",
        "skyship.brres", "sun.brres"};

const char *s_farm_course_files[] = {"FlagA2.brres", "brasd/cow/eat.brasd",
        "brasd/cow/eat_ed.brasd", "brasd/cow/run.brasd", "brasd/cow/surprise.brasd",
        "brasd/cow/walk.brasd", "castletree1.brres", "castletree2.brres", "choropu.brres",
        "cow.brres", "effect/choropu/rk_choropuMoko.breff", "effect/choropu/rk_choropuMoko.breft",
        "posteffect/posteffect2.bblm", "posteffect/posteffect3.bblm", "windmill.brres"};

const char *s_kinoko_course_files[] = {"brasd/kuribo/walk_l.brasd", "brasd/kuribo/walk_r.brasd",
        "kinoko.brres", "kinokoT1.brres", "kinoko_d_g.kcl", "kinoko_d_r.kcl", "kinoko_g.kcl",
        "kinoko_r.kcl", "kuribo.brres", "posteffect/posteffect2.blight",
        "posteffect/posteffect2.blmap"};

const char *s_factory_course_files[] = {"BeltCrossing.brres", "BeltCurveA.brres", "BeltEasy.brres",
        "Block.brres", "Crane.brres", "Crane.kcl", "MiiSighKino.brres", "Piston.brres",
        "Press.brres", "bulldozer_left.brres", "bulldozer_left.kcl", "bulldozer_right.brres",
        "bulldozer_right.kcl", "effect/Alarm/rk_alarm.breff", "effect/Alarm/rk_alarm.breft",
        "effect/Block/rk_block.breff", "effect/Block/rk_block.breft", "effect/Press/rk_press.breff",
        "effect/Press/rk_press.breft", "effect/Steam/rk_steam.breff", "effect/Steam/rk_steam.breft",
        "effect/pocha/rk_pocha.breff", "effect/pocha/rk_pocha.breft",
        "effect/woodbox/rk_woodbox.breff", "effect/woodbox/rk_woodbox.breft", "group_enemy_c.brres",
        "pocha.brres", "seagull.brres", "woodbox.brres"};

const char *s_castle_course_files[] = {"Mdush.brres", "Mdush.kcl", "castleballoon1.brres",
        "castleflower1.brres", "effect/wanwan/rk_wanwan.breff", "effect/wanwan/rk_wanwan.breft",
        "group_enemy_e.brres", "group_enemy_f.brres", "wanwan.brres"};

const char *s_shopping_course_files[] = {"CarA1.brres", "CarA2.brres", "CarA3.brres", "CarB.brres",
        "PalmTree.brres", "ShMiiObj01.brres", "ShMiiObj02.brres", "ShMiiObj03.brres",
        "escalator.brres", "escalator.kcl", "group_monte_a.brres", "mii_balloon.brres",
        "miiposter.brres", "monte_a.brres", "parasol.brres"};

const char *s_boardcross_course_files[] = {"DonkyCannon_wii.brres", "K_chairlift00.brres",
        "K_sticklift00.brres", "MiiStatueM2.brres", "brasd/DonkyCannon_wii/DonkyCannon_wii.brasd",
        "dk_miiobj00.brres", "dkmonitor.brres", "effect/DonkyCannon_wii/rk_dkCannon.breff",
        "effect/DonkyCannon_wii/rk_dkCannon.breft", "effect/EnvSnow/rk_EnvSnow.breff",
        "effect/EnvSnow/rk_EnvSnow.breft", "effect/Flash_B/rk_flashBCross.breff",
        "effect/Flash_B/rk_flashBCross.breft", "heyho.brres", "heyho2.brres"};

const char *s_truck_course_files[] = {"FlagA1.brres", "FlagB1.brres", "MiiSignWario.brres",
        "TruckWagon.brres", "basabasa.brres", "effect/TruckWagon/rk_truck.breff",
        "effect/TruckWagon/rk_truck.breft", "effect/truckChimSmk/rk_truckChimSmk.breff",
        "effect/truckChimSmk/rk_truckChimSmk.breft", "truckChimSmk.brres"};

const char *s_senior_course_files[] = {"MiiObjD01.brres", "MiiObjD02.brres", "MiiObjD03.brres",
        "MiiStatueBD1.brres", "MiiStatueBL1.brres", "MiiStatueD1.brres", "MiiStatueL1.brres",
        "cruiser.brres", "effect/cruiser/rk_cruiser.breff", "effect/cruiser/rk_cruiser.breft",
        "group_enemy_a.brres", "group_enemy_b.brres", "light_house.brres", "pylon01.brres"};

const char *s_water_course_files[] = {"Epropeller.brres", "FlagA5.brres", "FlagB3.brres",
        "FlagB4.brres", "MiiSignNoko.brres", "UtsuboDokan.brres",
        "effect/Epropeller/rk_epropeller.breff", "effect/Epropeller/rk_epropeller.breft",
        "effect/FallBsA/rk_fallbasin.breff", "effect/FallBsA/rk_fallbasin.breft",
        "effect/FallBsB/rk_fallbasin.breff", "effect/FallBsB/rk_fallbasin.breft", "moray.brres",
        "pukupuku.brres"};

const char *s_treehouse_course_files[] = {"ami.brres", "brasd/hanachan/head_wait2.brasd",
        "brasd/hanachan/walk.brasd", "brasd/tree_cannon/tree_cannon.brasd",
        "effect/EnvKareha/rk_EnvKareha.breff", "effect/EnvKareha/rk_EnvKareha.breft",
        "effect/EnvKarehaUp/rk_EnvKarehaUp.breff", "effect/EnvKarehaUp/rk_EnvKarehaUp.breft",
        "effect/hanachan/rk_hana.breff", "effect/hanachan/rk_hana.breft",
        "effect/karehayama/rk_karehayama.breff", "effect/karehayama/rk_karehayama.breft",
        "effect/leaf_effect/rk_leaf_effect.breff", "effect/leaf_effect/rk_leaf_effect.breft",
        "effect/tree_cannon/rk_dkCannon.breff", "effect/tree_cannon/rk_dkCannon.breft",
        "hanachan.brres", "karehayama.brres", "tree_cannon.brres"};

const char *s_volcano_course_files[] = {"FireSnake.brres", "FlamePole_v.brres",
        "VolcanoBall1.brres", "VolcanoPiece0.kcl", "VolcanoPiece0b.kcl", "VolcanoPiece0c.kcl",
        "VolcanoPiece1.brres", "VolcanoPiece1.kcl", "VolcanoPiece11.kcl", "VolcanoPiece11b.kcl",
        "VolcanoPiece11c.kcl", "VolcanoPiece13.kcl", "VolcanoPiece13b.kcl", "VolcanoPiece13c.kcl",
        "VolcanoPiece14.kcl", "VolcanoPiece14b.kcl", "VolcanoPiece14c.kcl", "VolcanoPiece16.kcl",
        "VolcanoPiece16b.kcl", "VolcanoPiece16c.kcl", "VolcanoPiece18.kcl", "VolcanoPiece18b.kcl",
        "VolcanoPiece18c.kcl", "VolcanoPiece1b.kcl", "VolcanoPiece1c.kcl", "VolcanoPiece2.kcl",
        "VolcanoPiece2b.kcl", "VolcanoPiece2c.kcl", "VolcanoPiece3.kcl", "VolcanoPiece3b.kcl",
        "VolcanoPiece3c.kcl", "VolcanoPiece4.kcl", "VolcanoPiece4b.kcl", "VolcanoPiece4c.kcl",
        "VolcanoPiece5.kcl", "VolcanoPiece5b.kcl", "VolcanoPiece5c.kcl", "VolcanoPiece7.kcl",
        "VolcanoPiece7b.kcl", "VolcanoPiece7c.kcl", "VolcanoPiece8.kcl", "VolcanoPiece8b.kcl",
        "VolcanoPiece8c.kcl", "VolcanoPiece9.kcl", "VolcanoPiece9b.kcl", "VolcanoPiece9c.kcl",
        "VolcanoRock1.brres", "VolcanoRock1.kcl", "VolcanoRock2.kcl",
        "effect/EnvFire/rk_EnvFire.breff", "effect/EnvFire/rk_EnvFire.breft",
        "effect/FireSnake/rk_fireSnake.breff", "effect/FireSnake/rk_fireSnake.breft",
        "effect/FlamePole_v/rk_flamePole.breff", "effect/FlamePole_v/rk_flamePole.breft",
        "effect/VolcanoBall1/rk_volc.breff", "effect/VolcanoBall1/rk_volc.breft",
        "effect/pochaYogan/rk_pochaYogan.breff", "effect/pochaYogan/rk_pochaYogan.breft",
        "effect/taimatsu/rk_taimatsu.breff", "effect/taimatsu/rk_taimatsu.breft",
        "effect/volsmk/rk_volsmk.breff", "effect/volsmk/rk_volsmk.breft", "pochaYogan.brres",
        "posteffect/posteffect.bti"};

const char *s_desert_course_files[] = {"MiiSphinxY2.brres", "brasd/sanbo/revive.brasd",
        "dc_pillar.brres", "dc_pillar.kcl", "dc_pillar_base.kcl", "dc_sandcone.brres",
        "dc_sandcone.kcl", "effect/dc_pillar/rk_dc_pillar.breff",
        "effect/dc_pillar/rk_dc_pillar.breft", "effect/dc_sandcone/rk_dc_sandcone.breff",
        "effect/dc_sandcone/rk_dc_sandcone.breft", "effect/sanbo/rk_sanbo.breff",
        "effect/sanbo/rk_sanbo.breft", "sanbo.brres"};

const char *s_ridgehighway_course_files[] = {"K_bomb_car.brres", "K_car_body.brres",
        "K_truck.brres", "Ksticketc.brres", "RhMiiKanban.brres",
        "effect/Fall_MH/rk_ridgeFall.breff", "effect/Fall_MH/rk_ridgeFall.breft",
        "effect/K_bomb_car/rk_K_bomb_car.breff", "effect/K_bomb_car/rk_K_bomb_car.breft",
        "effect/pochaMori/rk_pochaLeaf.breff", "effect/pochaMori/rk_pochaLeaf.breft",
        "ridgemii00.brres"};

const char *s_koopa_course_files[] = {"FlamePole.brres", "FlamePole.kcl", "TwistedWay.brres",
        "WLfirebarGC.brres", "brasd/koopaFigure/vomit.brasd", "brasd/koopaFigure/wait.brasd",
        "course.0", "dossun.brres", "effect/FlamePole/rk_flamePole.breff",
        "effect/FlamePole/rk_flamePole.breft", "effect/dossun/rk_dossun.breff",
        "effect/dossun/rk_dossun.breft", "effect/koopaBall/rk_koopaBall.breff",
        "effect/koopaBall/rk_koopaBall.breft", "koopaBall.brres", "koopaFigure.brres",
        "koopaFirebar.brres"};

const char *s_rainbow_course_files[] = {"EarthRing.brres", "InsekiA.brres", "InsekiB.brres",
        "KmoonZ.brres", "SpaceSun.brres", "StarRing.brres", "aurora.brres",
        "effect/StarRing/rk_StarRing.breff", "effect/StarRing/rk_StarRing.breft",
        "effect/entry/rk_entry.breff", "effect/entry/rk_entry.breft"};

const char *s_old_peach_gc_files[] = {"PeachHunsuiGC.brres", "Psea.brres",
        "brasd/poihana/throw.brasd", "brasd/poihana/walk.brasd",
        "effect/PeachHunsuiGC/rk_hunsui.breff", "effect/PeachHunsuiGC/rk_hunsui.breft",
        "effect/poihana/rk_poihana.breff", "effect/poihana/rk_poihana.breft", "mare_a.brres",
        "mare_b.brres", "peachtreeGC.brres", "poihana.brres"};

const char *s_old_falls_ds_files[] = {"effect/Fall_Y/rk_yoshiFalls.breff",
        "effect/Fall_Y/rk_yoshiFalls.breft"};

const char *s_old_obake_sfc_files[] = {"BGteresaSFC.brres", "b_teresa.brres",
        "obakeblockSFC.brres"};

const char *s_old_mario_64_files[] = {"MarioGo64.brres", "brasd/puchi_pakkun/puchi_pakkun.brasd",
        "effect/Flash_M/rk_flash64mario.breff", "effect/Flash_M/rk_flash64mario.breft",
        "puchi_pakkun.brres"};

const char *s_old_sherbet_64_files[] = {"brasd/penguin_m/walk.brasd", "brasd/penguin_s/dive.brasd",
        "brasd/penguin_s/stand_up.brasd", "brasd/penguin_s/walk.brasd",
        "effect/penguin_l/rk_penguin_l.breff", "effect/penguin_l/rk_penguin_l.breft",
        "effect/penguin_m/rk_penguin_m.breff", "effect/penguin_m/rk_penguin_m.breft",
        "effect/penguin_s/rk_penguin_s.breff", "effect/penguin_s/rk_penguin_s.breft", "ice.brres",
        "penguin_l.brres", "penguin_m.brres", "penguin_s.brres"};

const char *s_old_heyho_gba_files[] = {"HeyhoBallGBA.brres", "HeyhoShipGBA.brres",
        "HeyhoTreeGBA.brres", "brasd/crab/wait.brasd", "brasd/crab/walk_l.brasd",
        "brasd/crab/walk_r.brasd", "crab.brres", "effect/HeyhoBallGBA/rk_HeyhoBallGBA.breff",
        "effect/HeyhoBallGBA/rk_HeyhoBallGBA.breft", "effect/crab/rk_crab.breff",
        "effect/crab/rk_crab.breft", "r_parasol.brres"};

const char *s_old_town_ds_files[] = {"FlagA3.brres", "SentakuDS.brres", "TownBridgeDS.brres",
        "TownBridgeDS.kcl", "TownBridgeDS2.kcl", "TownBridgeDS3.kcl", "TownTreeDS.brres"};

const char *s_old_waluigi_gc_files[] = {"WLarrowGC.brres", "WLdokanGC.brres", "WLdokanGC.kcl",
        "WLscreenGC.brres", "WLwallGC.brres", "WLwallGC.kcl",
        "effect/Flash_W/rk_flashWaluigi.breff", "effect/Flash_W/rk_flashWaluigi.breft"};

const char *s_old_desert_ds_files[] = {"sunDS.brres"};

const char *s_old_koopa_gba_files[] = {"boble.brres", "effect/boble/rk_boble.breff",
        "effect/boble/rk_boble.breft"};

const char *s_old_donkey_64_files[] = {"DKship64.brres", "DKtreeA64.brres", "DKtreeB64.brres",
        "effect/DKfalls/rk_64DKfalls.breff", "effect/DKfalls/rk_64DKfalls.breft",
        "effect/DKship64/rk_DKship64.breff", "effect/DKship64/rk_DKship64.breft"};

const char *s_old_mario_gc_files[] = {"MashBalloonGC.brres", "brasd/pakkun_f/attack.brasd",
        "brasd/pakkun_f/wait.brasd", "mariotreeGC.brres", "pakkun_dokan.brres", "pakkun_f.brres"};

const char *s_old_mario_sfc_files[] = {"oilSFC.brres"};

const char *s_old_garden_ds_files[] = {"gardentreeDS.brres"};

const char *s_old_donkey_gc_files[] = {"DKrockGC.brres", "DKturibashiGC.brres",
        "DonkyCannonGC.brres", "bird.brres", "brasd/DonkyCannonGC/DonkyCannonGC.brasd",
        "donkytree1GC.brres", "donkytree2GC.brres", "effect/DKrockGC/rk_DKrockGC.breff",
        "effect/DKrockGC/rk_DKrockGC.breft", "effect/DonkyCannonGC/rk_dkCannon.breff",
        "effect/DonkyCannonGC/rk_dkCannon.breft"};

const char *s_old_koopa_64_files[] = {"KoopaFigure64.brres",
        "effect/FlamePole_v_big/rk_flamePoleL.breff", "effect/FlamePole_v_big/rk_flamePoleL.breft",
        "effect/KoopaFigure64/rk_koopaFire.breff", "effect/KoopaFigure64/rk_koopaFire.breft"};

const char *s_block_battle_files[] = {"MiiStatueD2.brres", "MiiStatueL2.brres", "MiiStatueM1.brres",
        "MiiStatueP1.brres", "bblock1.brres", "bblock1.kcl", "bblock1b.kcl", "bblock2.kcl",
        "bblock2b.kcl", "bblock3.kcl", "bblock3b.kcl", "bblock4.kcl", "bblock4b.kcl", "bblock5.kcl",
        "bblock5b.kcl", "bblock6.kcl", "bblock6b.kcl", "bblock7.kcl", "bblock7b.kcl", "bblock8.kcl",
        "bblock8b.kcl", "bblock9.kcl", "bblock9b.kcl", "coin.brres", "effect/coin/rk_coin.breff",
        "effect/coin/rk_coin.breft"};

const char *s_venice_battle_files[] = {"effect/truckChimSmkW/rk_truckChimSmkW.breff",
        "effect/truckChimSmkW/rk_truckChimSmkW.breft", "effect/venice_hasi/rk_venice.breff",
        "effect/venice_hasi/rk_venice.breft", "truckChimSmkW.brres", "venice_hasi.brres",
        "venice_hasi.kcl", "venice_hasi_b.kcl", "venice_hasi_wall.kcl", "venice_nami.brres",
        "venice_saku.brres", "venice_saku.kcl"};

const char *s_skate_battle_files[] = {"effect/Flash_S/rk_flashSkate.breff",
        "effect/Flash_S/rk_flashSkate.breft", "fks_screen_wii.brres"};

const char *s_casino_battle_files[] = {"Twanwan.brres", "casino_roulette.brres",
        "casino_roulette.kcl", "spot.brres"};

const char *s_sand_battle_files[] = {"effect/quicksand/rk_qsandDossun.breff",
        "effect/quicksand/rk_qsandDossun.breft", "quicksand.brres"};

const char *s_old_matenro_64_files[] = {"Crescent64.brres", "Spot64.brres"};

const char *s_winningrun_demo_files[] = {"DemoJugemu.brres", "effect/DemoEf/rk_demo.breff",
        "effect/DemoEf/rk_demo.breft", "effect/DemoJugemu/rk_DemoJugemu.breff",
        "effect/DemoJugemu/rk_DemoJugemu.breft", "effect/Kamifubuki/rk_kamifubuki.breff",
        "effect/Kamifubuki/rk_kamifubuki.breft", "group_enemy_a_demo.brres",
        "group_monte_a_demo.brres", "posteffect/posteffect.blobj", "posteffect/posteffect.bltex",
        "posteffect/posteffect2.blobj"};

const char *s_loser_demo_files[] = {"DemoCol.brres", "DemoCol.kcl"};

const char *s_ring_mission_files[] = {"RM_ring1.brres", "RM_ring1.kcl", "RM_ring1b.kcl",
        "RM_ring2.kcl", "RM_ring2b.kcl", "RM_ring2c.kcl", "RM_ring3.kcl", "RM_ring3b.kcl",
        "RM_ring3c.kcl", "begoman_spike.brres", "effect/begoman_spike/rk_begoman_spike.breff",
        "effect/begoman_spike/rk_begoman_spike.breft", "ring_mission_b.kcl"};

std::pair<std::pair<const char *, u8>, std::span<const char *>> s_autoaddLibrary[] = {
        {{"beginner_course", 0x08}, s_beginner_course_files},
        {{"farm_course", 0x01}, s_farm_course_files},
        {{"kinoko_course", 0x02}, s_kinoko_course_files},
        {{"factory_course", 0x04}, s_factory_course_files},
        {{"castle_course", 0x00}, s_castle_course_files},
        {{"shopping_course", 0x05}, s_shopping_course_files},
        {{"boardcross_course", 0x06}, s_boardcross_course_files},
        {{"truck_course", 0x07}, s_truck_course_files},
        {{"senior_course", 0x09}, s_senior_course_files},
        {{"water_course", 0x0F}, s_water_course_files},
        {{"treehouse_course", 0x0B}, s_treehouse_course_files},
        {{"volcano_course", 0x03}, s_volcano_course_files},
        {{"desert_course", 0x0E}, s_desert_course_files},
        {{"ridgehighway_course", 0x0A}, s_ridgehighway_course_files},
        {{"koopa_course", 0x0C}, s_koopa_course_files},
        {{"rainbow_course", 0x0D}, s_rainbow_course_files},
        {{"old_peach_gc", 0x10}, s_old_peach_gc_files},
        {{"old_falls_ds", 0x14}, s_old_falls_ds_files},
        {{"old_obake_sfc", 0x19}, s_old_obake_sfc_files},
        {{"old_mario_64", 0x1A}, s_old_mario_64_files},
        {{"old_sherbet_64", 0x1B}, s_old_sherbet_64_files},
        {{"old_heyho_gba", 0x1F}, s_old_heyho_gba_files},
        {{"old_town_ds", 0x17}, s_old_town_ds_files},
        {{"old_waluigi_gc", 0x12}, s_old_waluigi_gc_files},
        {{"old_desert_ds", 0x15}, s_old_desert_ds_files},
        {{"old_koopa_gba", 0x1E}, s_old_koopa_gba_files},
        {{"old_donkey_64", 0x1D}, s_old_donkey_64_files},
        {{"old_mario_gc", 0x11}, s_old_mario_gc_files},
        {{"old_mario_sfc", 0x18}, s_old_mario_sfc_files},
        {{"old_garden_ds", 0x16}, s_old_garden_ds_files},
        {{"old_donkey_gc", 0x13}, s_old_donkey_gc_files},
        {{"old_koopa_64", 0x1C}, s_old_koopa_64_files},
        {{"block_battle", 0x21}, s_block_battle_files},
        {{"venice_battle", 0x20}, s_venice_battle_files},
        {{"skate_battle", 0x23}, s_skate_battle_files},
        {{"casino_battle", 0x22}, s_casino_battle_files},
        {{"sand_battle", 0x24}, s_sand_battle_files},
        {{"old_matenro_64", 0x29}, s_old_matenro_64_files},
        {{"winningrun_demo", 0x37}, s_winningrun_demo_files},
        {{"loser_demo", 0x38}, s_loser_demo_files}, {{"ring_mission", 0x36}, s_ring_mission_files}};

// WU8a
constexpr u32 WU8_MAGIC = 1465202785;

namespace WU8Library {

// Minimum required sizes for vanilla tracks to be ripped and extracted.
// This avoids using up too much memory by having to reallocate and copy
// if using a std::vector<u8>, and this lack of reallocation means it can
// run while heaps are locked (eg: while WU8LibraryPage is shown).
constexpr size_t CompressedMaxSize = 3000000;
constexpr size_t DecompressedMaxSize = 6000000;
constexpr size_t ExtractionHeapSize = CompressedMaxSize + DecompressedMaxSize + 1024;

ExtractionStage s_extractionStage = ExtractionStage::Started;
std::array<char, 64> s_currentlyExtracting;
Mutex s_extractionStateLock;

u8 s_extractionStack[1024 * 10];
OSThread s_extractionThread;

void *ExtractThread(void *param);

ExtractionState GetExtractionState() {
    ScopeLock<Mutex> g(s_extractionStateLock);

    return ExtractionState{
            .stage = s_extractionStage,
            .archive = s_currentlyExtracting,
    };
}

void SetExtractionState(ExtractionStage stage, const char *currentFile) {
    ScopeLock<Mutex> g(s_extractionStateLock);

    s_extractionStage = stage;
    if (currentFile == nullptr) {
        s_currentlyExtracting[0] = '\0';
    } else {
        snprintf(s_currentlyExtracting.data(), s_currentlyExtracting.size(), "%s", currentFile);
    }
}

bool ContainsWBZ(SP::Storage::DirHandle &dir);

bool ContainsWBZ(SP::Storage::NodeId dirId) {
    auto dir = Storage::FastOpenDir(dirId);
    if (dir) {
        return ContainsWBZ(*dir);
    } else {
        return false;
    }
}

bool ContainsWBZ(SP::Storage::DirHandle &dir) {
    while (auto node = dir.read()) {
        if (node->type == SP::Storage::NodeType::Dir) {
            if (ContainsWBZ(node->id)) {
                return true;
            }
        }

        if (std::wstring_view(node->name).ends_with(L".wbz")) {
            return true;
        }
    }

    return false;
}

bool ShouldExtract() {
    if (SP::Storage::Open(L"autoadd/.finished", "r")) {
        return false;
    }

    auto myStuff = SP::Storage::OpenDir(L"My Stuff");
    if (!myStuff) {
        return false;
    }

    return ContainsWBZ(*myStuff);
}

void StartExtraction(EGG::Heap *mem2) {
    void *buffer = new (mem2, -0x20) u8[ExtractionHeapSize];
    auto *extractionHeap = EGG::ExpHeap::Create(buffer, ExtractionHeapSize, 1);

    size_t stackSize = sizeof(s_extractionStack);
    u8 *stackTop = s_extractionStack + stackSize;

    OSCreateThread(&s_extractionThread, ExtractThread, extractionHeap, stackTop, stackSize, 25, 0);
    OSResumeThread(&s_extractionThread);
    OSDetachThread(&s_extractionThread);
}

void *ExtractThread(void *param) {
    auto *extractionHeap = reinterpret_cast<EGG::ExpHeap *>(param);
    assert(Storage::CreateDir(L"autoadd/", true));

    void *compressedBlock = extractionHeap->alloc(CompressedMaxSize, -0x20);
    void *decompressedBlock = extractionHeap->alloc(DecompressedMaxSize, -0x20);
    std::span<u8> compressedBuf(reinterpret_cast<u8 *>(compressedBlock), CompressedMaxSize);
    std::span<u8> decompressedBuf(reinterpret_cast<u8 *>(decompressedBlock), DecompressedMaxSize);

    u32 u8MagicInt = 0;
    memcpy(&u8MagicInt, U8_MAGIC, 4);

    auto *saveManager = System::SaveManager::Instance();
    auto *dvdStorage = SP::Storage::GetStorage(SP::Storage::StorageType::DVD);
    for (auto pair : s_autoaddLibrary) {
        auto archive = pair.first.first;
        auto courseId = static_cast<Registry::Course>(pair.first.second);
        auto files = pair.second;

        wchar_t sourcePath[64];
        swprintf(sourcePath, sizeof(sourcePath), L"ro:/Race/Course/%s.szs", archive);

        auto fileHandle = dvdStorage->open(sourcePath, "r");
        if (!fileHandle) {
            panic("Unable to load original game file!");
        }

        SetExtractionState(ExtractionStage::Ripping, archive);
        assert(fileHandle->read(compressedBuf.data(), fileHandle->size(), 0));

        SetExtractionState(ExtractionStage::Decompressing, archive);
        auto decompressedSize = YAZDecoder::Decode(compressedBuf.data(), fileHandle->size(),
                decompressedBuf.data(), decompressedBuf.size())
                                        .value();

        if (static_cast<u32>(courseId) < 42) {
            Sha1 courseSha;
            NETSHA1Context shaContext;
            NETSHA1Init(&shaContext);
            NETSHA1Update(&shaContext, decompressedBuf.data(), decompressedSize);
            NETSHA1GetDigest(&shaContext, &courseSha);

            auto vanillaSha = saveManager->vanillaSHA1(courseId);
            if (vanillaSha != courseSha) {
                auto courseShaStr = sha1ToHex(courseSha);
                auto vanillaShaStr = sha1ToHex(vanillaSha);
                SP_LOG("Vanilla course %s != Read course %s", vanillaShaStr.data(),
                        courseShaStr.data());

                SetExtractionState(ExtractionStage::ReplacedCourse, archive);
                break;
            }
        }

        SetExtractionState(ExtractionStage::Processing, archive);
        U8Cursor cursor{{decompressedBuf.data(), decompressedSize}};
        auto header = cursor.readU8Header().value();
        assert(header.magic == u8MagicInt);

        cursor.setPosition(header.nodeOffset);
        auto rootNode = cursor.readNode().value();
        cursor.setPosition(header.nodeOffset);

        auto nodeHeaderSize = rootNode.size * 12;
        auto stringTableStart = header.nodeOffset + nodeHeaderSize;

        U8Iterator iterator{cursor, nullptr, rootNode.size, stringTableStart};
        std::optional<U8IterItem> item;

        u8 filesToExtract = files.size();
        CircularBuffer<const char *, 3> dirStack;
        // We must set skipReadingFile to true as we aren't passing an out buffer.
        while ((item = iterator.next(/* skipReadingFile */ true))) {
            if (item->isDir) {
                auto dirPath = iterator.getPath(nullptr);
                assert(Storage::CreateDir(dirPath.data(), true));
                continue;
            } else if (item->isErr || !item->file) {
                panic("Error while iterating");
            }

            char path[64];
            std::array<wchar_t, 64> pathWide = iterator.getPath(item->file->name);
            if (snprintf(path, sizeof(path), "%ls", pathWide.data()) <= 0) {
                panic("Overflowed path");
            }

            const char *pathClean = path + strlen("autoadd/");
            auto strcmpPred = [&](const char *file) { return !strcmp(file, pathClean); };
            if (std::find_if(files.begin(), files.end(), strcmpPred) == files.end()) {
                // File is not needed for the auto-add library.
                continue;
            };

            filesToExtract -= 1;
            if (item->file->hasData) {
                continue;
            }

            SetExtractionState(ExtractionStage::Writing, pathClean);
            auto file = Storage::Open(pathWide.data(), "w").value();
            auto filePtr = &decompressedBuf[item->file->node.dataOffset];

            assert(file.write(filePtr, item->file->node.size, 0));
            SetExtractionState(ExtractionStage::Processing, archive);
        }

        // panic is skipped when the file has already been found.
        if (filesToExtract != 0) {
            panic("Unable to find all required files!\n%hhd files left", filesToExtract);
        }
    }

    if (GetExtractionState().stage != ExtractionStage::ReplacedCourse) {
        Storage::Open(L"autoadd/.finished", "w").value();
        SetExtractionState(ExtractionStage::Finished, nullptr);
    }

    extractionHeap->free(decompressedBlock);
    extractionHeap->free(compressedBlock);
    extractionHeap->destroy();
    return nullptr;
}

} // namespace WU8Library

u8 DeriveStartingKey(u32 size) {
    u8 *p = reinterpret_cast<u8 *>(&size);
    return p[0] ^ p[1] ^ p[2] ^ p[3];
}

bool DecodeWU8(std::vector<u8, HeapAllocator<u8>> &wu8Buf) {
    U8Cursor cursor({wu8Buf.begin(), wu8Buf.end()});

    auto header = cursor.readU8Header().value();
    assert(header.magic == WU8_MAGIC);

    // Used for pass one of decoding
    u8 startingKey = DeriveStartingKey(wu8Buf.size());
    // Used for pass two of decoding.
    u8 derivedKey = startingKey;

    // Initial pass, XOR all node and string table bytes with starting key
    for (u32 i = header.nodeOffset; i < (header.nodeOffset + header.metaSize); i++) {
        wu8Buf[i] ^= startingKey;
    }

    cursor.setPosition(header.nodeOffset);
    auto rootNode = cursor.readNode();
    cursor.setPosition(header.nodeOffset);

    auto nodeHeaderSize = rootNode->size * 12;
    auto stringTableStart = header.nodeOffset + nodeHeaderSize;

    std::vector<u8, HeapAllocator<u8>> originalData({wu8Buf.get_allocator()});
    U8Iterator iterator(cursor, &originalData, rootNode->size, stringTableStart);
    std::optional<U8IterItem> item;

    SP_LOG("Starting decode path 1 (XOR all object files with auto-add library)");
    while ((item = iterator.next(false))) {
        if (item->isDir) {
            continue;
        } else if (item->isErr || !item->file) {
            panic("Error while iterating");
        } else if (!item->file->hasData) {
            continue;
        }

        auto originalSize = originalData.size();
        derivedKey ^= originalData[originalSize / 2] ^ originalData[originalSize / 3] ^
                originalData[originalSize / 4];

        size_t archiveIndex = 0;
        size_t originalIndex = 0;
        while (archiveIndex != item->file->node.size) {
            if (originalIndex == originalSize) {
                originalIndex = 0;
            }

            auto archiveOffset = item->file->node.dataOffset + archiveIndex;
            wu8Buf[archiveOffset] ^= startingKey ^ originalData[originalIndex];

            originalIndex++;
            archiveIndex++;
        }
    }

    iterator.reset(header.nodeOffset);

    SP_LOG("Starting decode pass 2 (XOR all non-object files with derived key %hhu)", derivedKey);
    while ((item = iterator.next(true))) {
        if (item->isDir) {
            continue;
        } else if (item->isErr || !item->file) {
            panic("Error while iterating");
        } else if (item->file->hasData) {
            continue;
        }

        auto &node = item->file->node;
        for (auto i = node.dataOffset; i < (node.dataOffset + node.size); i++) {
            wu8Buf[i] ^= derivedKey;
        }
    }

    memcpy(wu8Buf.data(), U8_MAGIC, 4);
    return true;
}

} // namespace SP
