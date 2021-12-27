#include <revolution.h>

#include <Rel.h>

typedef void (*PayloadEntryFunction)(void);

PayloadEntryFunction loadPayload(void) {
    DVDFileInfo fileInfo;
    if (!DVDOpen("/bin/payload.bin", &fileInfo)) {
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
