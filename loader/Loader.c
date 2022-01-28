#include <Common.h>

typedef struct {
    u8 _00[0x34 - 0x00];
    u32 length;
    u8 _38[0x3c - 0x38];
} DVDFileInfo;
static_assert(sizeof(DVDFileInfo) == 0x3c);

typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} GXColor;
static_assert(sizeof(GXColor) == 0x4);

static int (*snprintf)(char *restrict str, size_t size, const char *restrict format, ...);
static BOOL (*DVDOpen)(const char *fileName, DVDFileInfo *fileInfo);
static s32 (*DVDReadPrio)(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 prio);
#define DVDRead(fileInfo, addr, length, offset) \
    DVDReadPrio((fileInfo), (addr), (length), (offset), 2)
static BOOL (*DVDClose)(DVDFileInfo *fileInfo);
#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))
static void (*OSInit)(void);
static void *(*OSAllocFromMEM1ArenaLo)(u32 size, u32 align);
static void (*ICInvalidateRange)(void *addr, u32 nBytes);
static void (*OSFatal)(GXColor fg, GXColor bg, const char *msg);

static u32 Rel_getSize(void) {
    switch (REGION) {
    case 'P':
        return 0x3d49d0;
    case 'E':
        return 0x3d45f0;
    case 'J':
        return 0x3d4190;
    case 'K':
        return 0x3d4e30;
    default:
        while (true);
    }
}

typedef void (*PayloadEntryFunction)(void);

PayloadEntryFunction loadPayload(void) {
    char fileName[0x20];
    snprintf(fileName, sizeof(fileName), "/bin/payload%c.bin", REGION);

    DVDFileInfo fileInfo;
    if (!DVDOpen(fileName, &fileInfo)) {
        return NULL;
    }

    s32 size = OSRoundUp32B(fileInfo.length);
    void *payload = OSAllocFromMEM1ArenaLo(size, 0x20);
    s32 result = DVDRead(&fileInfo, payload, size, 0);
    DVDClose(&fileInfo);
    if (result != size) {
        return NULL;
    }

    ICInvalidateRange(payload, size);

    return payload;
}

__attribute__((section("first"))) void start(void) {
    switch (REGION) {
    case 'P':
        snprintf = (void *)0x80011938;
        DVDOpen = (void *)0x8015e2bc;
        DVDReadPrio = (void *)0x8015e834;
        DVDClose = (void *)0x8015e568;
        OSInit = (void *)0x8019fc68;
        OSAllocFromMEM1ArenaLo = (void *)0x801a1104;
        ICInvalidateRange = (void *)0x801a1710;
        OSFatal = (void *)0x801a4ec4;
        break;
    case 'E':
        snprintf = (void *)0x80010dd8;
        DVDOpen = (void *)0x8015e21c;
        DVDReadPrio = (void *)0x8015e794;
        DVDClose = (void *)0x8015e4c8;
        OSInit = (void *)0x8019fbc8;
        OSAllocFromMEM1ArenaLo = (void *)0x801a1064;
        ICInvalidateRange = (void *)0x801a1670;
        OSFatal = (void *)0x801a4e24;
        break;
    case 'J':
        snprintf = (void *)0x8001185c;
        DVDOpen = (void *)0x8015e1dc;
        DVDReadPrio = (void *)0x8015e754;
        DVDClose = (void *)0x8015e488;
        OSInit = (void *)0x8019fb88;
        OSAllocFromMEM1ArenaLo = (void *)0x801a1024;
        ICInvalidateRange = (void *)0x801a1630;
        OSFatal = (void *)0x801a4de4;
        break;
    case 'K':
        snprintf = (void *)0x800119a0;
        DVDOpen = (void *)0x8015e334;
        DVDReadPrio = (void *)0x8015e8ac;
        DVDClose = (void *)0x8015e5e0;
        OSInit = (void *)0x8019ffc4;
        OSAllocFromMEM1ArenaLo = (void *)0x801a1460;
        ICInvalidateRange = (void *)0x801a1a6c;
        OSFatal = (void *)0x801a5220;
        break;
    default:
        while (true);
    }

    OSInit();

    OSAllocFromMEM1ArenaLo(Rel_getSize(), 0x20);

    PayloadEntryFunction entry = loadPayload();
    if (!entry) {
        GXColor fg = { 255, 255, 255, 255 };
        GXColor bg = { 0, 0, 0, 255 };
        OSFatal(fg, bg, "Couldn't load mkw-sp payload!");
    }

    entry();
}
