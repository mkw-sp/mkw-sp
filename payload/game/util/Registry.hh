#pragma once

#include <Common.hh>

#include "game/ui/SectionId.hh"

namespace Registry {

enum class Character {
    Mario = 0x00,
    BabyPeach = 0x01,
    Waluigi = 0x02,
    Bowser = 0x03,
    BabyDaisy = 0x04,
    DryBones = 0x05,
    BabyMario = 0x06,
    Luigi = 0x07,
    Toad = 0x08,
    DonkeyKong = 0x09,
    Yoshi = 0x0A,
    Wario = 0x0B,
    BabyLuigi = 0x0C,
    Toadette = 0x0D,
    KoopaTroopa = 0x0E,
    Daisy = 0x0F,
    Peach = 0x10,
    Birdo = 0x11,
    DiddyKong = 0x12,
    KingBoo = 0x13,
    BowserJr = 0x14,
    DryBowser = 0x15,
    FunkyKong = 0x16,
    Rosalina = 0x17,
    SmallMiiOutfitAMale = 0x18,
    SmallMiiOutfitAFemale = 0x19,
    SmallMiiOutfitBMale = 0x1A,
    SmallMiiOutfitBFemale = 0x1B,
    SmallMiiOutfitCMale = 0x1C,
    SmallMiiOutfitCFemale = 0x1D,
    MediumMiiOutfitAMale = 0x1E,
    MediumMiiOutfitAFemale = 0x1F,
    MediumMiiOutfitBMale = 0x20,
    MediumMiiOutfitBFemale = 0x21,
    MediumMiiOutfitCMale = 0x22,
    MediumMiiOutfitCFemale = 0x23,
    LargeMiiOutfitAMale = 0x24,
    LargeMiiOutfitAFemale = 0x25,
    LargeMiiOutfitBMale = 0x26,
    LargeMiiOutfitBFemale = 0x27,
    LargeMiiOutfitCMale = 0x28,
    LargeMiiOutfitCFemale = 0x29,
    MediumMii = 0x2A,
    SmallMii = 0x2B,
    LargeMii = 0x2C,
};

enum class Vehicle {
    StandardKartSmall = 0x00,
    StandardKartMedium = 0x01,
    StandardKartLarge = 0x02,
    BoosterSeat = 0x03,
    ClassicDragster = 0x04,
    Offroader = 0x05,
    MiniBeast = 0x06,
    WildWing = 0x07,
    FlameFlyer = 0x08,
    CheepCharger = 0x09,
    SuperBlooper = 0x0A,
    PiranhaProwler = 0x0B,
    TinyTitan = 0x0C,
    Daytripper = 0x0D,
    Jetsetter = 0x0E,
    BlueFalcon = 0x0F,
    Sprinter = 0x10,
    Honeycoupe = 0x11,
    StandardBikeSmall = 0x12,
    StandardBikeMedium = 0x13,
    StandardBikeLarge = 0x14,
    BulletBike = 0x15,
    MachBike = 0x16,
    FlameRunner = 0x17,
    BitBike = 0x18,
    Sugarscoot = 0x19,
    WarioBike = 0x1A,
    Quacker = 0x1B,
    ZipZip = 0x1C,
    ShootingStar = 0x1D,
    Magikruiser = 0x1E,
    Sneakster = 0x1F,
    Spear = 0x20,
    JetBubble = 0x21,
    DolphinDasher = 0x22,
    Phantom = 0x23,
};

enum class Controller {
    None = -0x1,
    WiiWheel = 0x0,
    WiiRemoteAndNunchuck = 0x1,
    Classic = 0x2,
    GameCube = 0x3,
};

enum class Course {
    MarioCircuit = 0x00,
    MooMooMeadows = 0x01,
    MushroomGorge = 0x02,
    GrumbleVolcano = 0x03,
    ToadsFactory = 0x04,
    CoconutMall = 0x05,
    DKSummit = 0x06,
    WarioGoldMine = 0x07,
    LuigiCircuit = 0x08,
    DaisyCircuit = 0x09,
    MoonviewHighway = 0x0A,
    MapleTreeway = 0x0B,
    BowsersCastle = 0x0C,
    RainbowRoad = 0x0D,
    DryDryRuins = 0x0E,
    KoopaCape = 0x0F,
    GCNPeachBeach = 0x10,
    GCNMarioCircuit = 0x11,
    GCNWaluigiStadium = 0x12,
    GCNDKMountain = 0x13,
    DSYoshiFalls = 0x14,
    DSDesertHills = 0x15,
    DSPeachGardens = 0x16,
    DSDelfinoSquare = 0x17,
    SNESMarioCircuit3 = 0x18,
    SNESGhostValley2 = 0x19,
    N64MarioRaceway = 0x1A,
    N64SherbetLand = 0x1B,
    N64BowsersCastle = 0x1C,
    N64DKsJungleParkway = 0x1D,
    GBABowserCastle3 = 0x1E,
    GBAShyGuyBeach = 0x1F,
    DelfinoPier = 0x20,
    BlockPlaza = 0x21,
    ChainChompRoulette = 0x22,
    FunkyStadium = 0x23,
    ThwompDesert = 0x24,
    GCNCookieLand = 0x25,
    DSTwilightHouse = 0x26,
    SNESBattleCourse4 = 0x27,
    GBABattleCourse3 = 0x28,
    N64Skyscraper = 0x29,
    GalaxyColosseum = 0x36,
    WinDemo = 0x37,
    LoseDemo = 0x38,
    DrawDemo = 0x39,
    EndingDemo = 0x3A,
};

extern const Course OrderedCourses[0x20];

bool IsCombinationValid(Character character, Vehicle vehicle);
bool IsControllerValid(Controller controller);
bool IsRaceCourse(Course course);

u32 GetCharacterMessageId(u32 characterId, bool resolveMiiNames);
REPLACE bool UseBattleRenders();

s32 GetButtonIndexFromCourse(Course course);

bool IsWiFiMenuSection(UI::SectionId sectionId);
bool IsGhostRaceSection(UI::SectionId sectionId);
bool IsCompetitionSection(UI::SectionId sectionId);

const char *GetCharacterPane(u32 characterId);
const char *GetCupIconName(u32 cupId);
u32 GetCupMessageId(u32 cupId);
u32 GetCourseCupId(Course courseId);
u32 GetCourseName(Course courseId);

const char *REPLACED(GetItemPane)(u32 itemId, u32 count);
REPLACE const char *GetItemPane(u32 itemId, u32 count);

// Returns the message ID for the given error code.
int REPLACED(WifiErrorExplain)(u32 error_code);
REPLACE u32 WifiErrorExplain(u32 error_code);

} // namespace Registry
