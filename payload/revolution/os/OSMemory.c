#include <revolution/os.h>
#include <sp/Dol.h>
#include <sp/Payload.h>
#include <sp/Rel.h>

extern void EnableInstsOnExecSections(void);

void RealMode(void (*function)(void));

static const u32 k128KBs = 0x20000;
static const u32 kBATAreas = 12;
static const u32 kIBATUVS = (1 << 1);
static const u32 kIBATUVP = (1 << 0);

u32 newIBAT0U;
u32 newIBAT1U;
u32 newIBAT2U;

static u32 MakeBATBEPI(const void *address) {
    return (u32)address & 0xFFFE0000;
}

static u32 MakeBATBLEncoding(u32 shiftAmount) {
    assert(shiftAmount < kBATAreas);

    return ((1 << shiftAmount) - 1) << 2;
}

static u32 MakeBATBLEncodingFromSize(u32 size) {
    u32 batBLEncoding;
    u32 batAreaSize = k128KBs;
    for (size_t n = 0; n < kBATAreas; n++) {
        batBLEncoding = MakeBATBLEncoding(n);
        if (size <= batAreaSize) {
            return batBLEncoding;
        }
        batAreaSize <<= 1;
    }
    return batBLEncoding;
}

static void SetNewIBAT0UValue(void) {
    char const *startAddress = (char *)OS_CACHED_BASE;
    char const *endAddress = (char *)Dol_getTextSectionEnd();

    const u32 IBAT0UBEPI = MakeBATBEPI(startAddress);
    const u32 IBAT0UBLEncoding = MakeBATBLEncodingFromSize(endAddress - startAddress);

    newIBAT0U = IBAT0UBEPI | IBAT0UBLEncoding | kIBATUVS | kIBATUVP;
}

static void SetNewIBAT1UValue(void) {
    char const *startAddress = (char *)ROUND_DOWN(Rel_getTextSectionStart(), 0x20000);
    char const *endAddress = (char *)Rel_getTextSectionEnd();

    const u32 IBAT1UBEPI = MakeBATBEPI(startAddress);
    const u32 IBAT1UBLEncoding = MakeBATBLEncodingFromSize(endAddress - startAddress);

    newIBAT1U = IBAT1UBEPI | IBAT1UBLEncoding | kIBATUVS | kIBATUVP;
}

static void SetNewIBAT2UValue(void) {
    char const *startAddress = (char *)ROUND_DOWN(Payload_getTextSectionStart(), 0x20000);
    char const *endAddress = (char *)Payload_getTextSectionEnd();

    const u32 IBAT2UBEPI = MakeBATBEPI(startAddress);
    const u32 IBAT2UBLEncoding = MakeBATBLEncodingFromSize(endAddress - startAddress);

    newIBAT2U = IBAT2UBEPI | IBAT2UBLEncoding | kIBATUVS | kIBATUVP;
}

void OSEnableCodeExecOnExecSections(void) {
    SetNewIBAT0UValue();
    SetNewIBAT1UValue();
    SetNewIBAT2UValue();

    // clang-format off
    BOOL enabled = OSDisableInterrupts();
    {
        RealMode(EnableInstsOnExecSections);
    }
    OSRestoreInterrupts(enabled);
    // clang-format on
}
