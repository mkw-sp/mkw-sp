#include "CourseManager.h"

#include "sp/Storage.h"

enum {
    PREFIX_ID_NONE,
    PREFIX_ID_SNES,
    PREFIX_ID_N64,
    PREFIX_ID_GBA,
    PREFIX_ID_GCN,
    PREFIX_ID_DS,
    PREFIX_ID_WII,
};

typedef struct {
    bool isValid : 1;
    bool isVanilla : 1;
    bool isRace : 1;
    u8 prefixId : 3;
    u8 courseId : 5;
    u32 size;
} Course;

static const Course courses[] = {
    [3422] = { // Wii Bowser's Castle (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x0C,
    },
    [3424] = { // Wii Coconut Mall (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x05,
    },
    [3426] = { // Wii DK Summit (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x06,
    },
    [3427] = { // DS Delfino Square (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_DS,
        .courseId = 0x17,
    },
    [3428] = { // DS Desert Hills (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_DS,
        .courseId = 0x15,
    },
    [3429] = { // DS Peach Gardens (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_DS,
        .courseId = 0x16,
    },
    [3430] = { // DS Yoshi Falls (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_DS,
        .courseId = 0x14,
    },
    [3431] = { // Wii Daisy Circuit (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x09,
    },
    [3433] = { // Wii Dry Dry Ruins (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x0E,
    },
    [3435] = { // GBA Bowser Castle 3 (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_GBA,
        .courseId = 0x1E,
    },
    [3436] = { // GBA Shy Guy Beach (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_GBA,
        .courseId = 0x1F,
    },
    [3437] = { // GCN DK Mountain (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_GCN,
        .courseId = 0x13,
    },
    [3438] = { // GCN Mario Circuit (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_GCN,
        .courseId = 0x11,
    },
    [3439] = { // GCN Peach Beach (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x10,
    },
    [3440] = { // GCN Waluigi Stadium (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_GCN,
        .courseId = 0x12,
    },
    [3441] = { // Wii Grumble Volcano (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x03,
    },
    [3443] = { // Wii Koopa Cape (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x0F,
    },
    [3445] = { // Wii Luigi Circuit (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x08,
    },
    [3447] = { // Wii Maple Treeway (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x0B,
    },
    [3448] = { // Wii Mario Circuit (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x00,
    },
    [3450] = { // Wii Moo Moo Meadows (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x01,
    },
    [3452] = { // Wii Moonview Highway (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x0A,
    },
    [3454] = { // Wii Mushroom Gorge (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x02,
    },
    [3456] = { // N64 Bowser's Castle (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_N64,
        .courseId = 0x1C,
    },
    [3457] = { // N64 DK's Jungle Parkway (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_N64,
        .courseId = 0x1D,
    },
    [3458] = { // N64 Mario Raceway (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_N64,
        .courseId = 0x1A,
    },
    [3459] = { // N64 Sherbet Land (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_N64,
        .courseId = 0x1B,
    },
    [3460] = { // Wii Rainbow Road (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x0D,
    },
    [3462] = { // SNES Ghost Valley 2 (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_SNES,
        .courseId = 0x19,
    },
    [3463] = { // SNES Mario Circuit 3 (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_SNES,
        .courseId = 0x18,
    },
    [3464] = { // Wii Toad's Factory (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x04,
    },
    [3466] = { // Wii Wario's Gold Time (Nintendo)
        .isValid = true,
        .isVanilla = true,
        .isRace = true,
        .prefixId = PREFIX_ID_WII,
        .courseId = 0x07,
    },
    [5113] = { // New Moon Manor v1.1 (Sniki)
        .isValid = true,
        .isVanilla = false,
        .isRace = true,
        .prefixId = PREFIX_ID_NONE,
        .courseId = 0x1C,
        .size = 3943354,
    },
    [6486] = { // Honeybee Hideout v1.3.1 (TacoJosh)
        .isValid = true,
        .isVanilla = false,
        .isRace = true,
        .prefixId = PREFIX_ID_NONE,
        .courseId = 0x0B,
        .size = 6928795,
    },
    [10014] = { // GBA Snow Land v2.1.1 (cpfusion)
        .isValid = true,
        .isVanilla = false,
        .isRace = true,
        .prefixId = PREFIX_ID_GBA,
        .courseId = 0x1B,
        .size = 2163132,
    },
    [10480] = { // GBA Boo Lake v2.0 (Slimeserver)
        .isValid = true,
        .isVanilla = false,
        .isRace = true,
        .prefixId = PREFIX_ID_GBA,
        .courseId = 0x19,
        .size = 2212602,
    },
    [10487] = { // Royal Rainbow v2.2 (Brawlboxgaming)
        .isValid = true,
        .isVanilla = false,
        .isRace = true,
        .prefixId = PREFIX_ID_NONE,
        .courseId = 0x0D,
        .size = 3333081,
    },
    [10498] = { // GBA Cheese Land v3.0-beta (ZPL)
        .isValid = true,
        .isVanilla = false,
        .isRace = true,
        .prefixId = PREFIX_ID_GBA,
        .courseId = 0x15,
        .size = 4027139,
    },
    [10648] = { // GBA Broken Pier v1.04.4fix (xBlue98)
        .isValid = true,
        .isVanilla = false,
        .isRace = true,
        .prefixId = PREFIX_ID_GBA,
        .courseId = 0x19,
        .size = 4029374,
    },
};
static const u32 vanillaCourses[] = {
    3422, 3424, 3426, 3427, 3428, 3429, 3430, 3431, 3433, 3435, 3436, 3437, 3438, 3439, 3440, 3441,
    3443, 3445, 3447, 3448, 3450, 3452, 3454, 3456, 3457, 3458, 3459, 3460, 3462, 3463, 3464, 3466,
};

static u32 count;
static NodeId fileIds[4096];
static u32 dbIds[4096];
//static u32 selectedCount;
//static u32 selectedIdcs[4096];

void CourseManager_init(void) {
    for (u32 i = 0; i < ARRAY_SIZE(vanillaCourses); i++) {
        fileIds[count].storage = NULL;
        dbIds[count] = vanillaCourses[i];
        count++;
    }

    Dir dir;
    if (!Storage_openDir(&dir, L"/mkw-sp/courses")) {
        return;
    }

    NodeInfo info;
    while (Storage_readDir(&dir, &info)) {
        if (info.type == NODE_TYPE_NONE) {
            break;
        }
        if (info.type != NODE_TYPE_FILE) {
            continue;
        }
        fileIds[count] = info.id;
        dbIds[count] = UINT32_MAX;
        for (u32 i = 0; i < ARRAY_SIZE(courses); i++) {
            if (!courses[i].isValid) {
                continue;
            }
            if (courses[i].size == info.size) {
                dbIds[count] = i;
                break;
            }
        }
        count++;
    }

    Storage_closeDir(&dir);
}

u32 CourseManager_count(void) {
    return count;
}

u32 CourseManager_getDbId(u32 index) {
    assert(index < count);
    return dbIds[index];
}

u32 CourseManager_getCourseId(u32 index) {
    assert(index < count);
    return courses[dbIds[index]].courseId;
}
