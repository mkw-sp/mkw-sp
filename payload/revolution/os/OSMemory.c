#include <revolution/os.h>
#include <sp/Payload.h>

extern void DisableInstsOnMEM1Hi16MB(void);
extern void DisableInstsOnMEM2(void);
extern void EnableInstsOnPayload(void);

void RealMode(void (*function)(void));

static const u32 k128KBs = 0x20000;
static const u32 kBATAreas = 12;

u32 newIBAT1U;

static u32 GetIBAT0U(void) {
    u32 IBAT0U;
    __asm__ volatile("mfspr %0, 0x210" : "=r"(IBAT0U));
    return IBAT0U;
}

static u32 GetBATBEPI(u32 upperBAT) {
    return upperBAT & 0xFFFE0000;
}

static u32 GetBATBLEncoding(u32 upperBAT) {
    return upperBAT & 0x00001FFC;
}

static u32 GetBATAreaSize(u32 batBLEncoding) {
    batBLEncoding >>= 2;
    batBLEncoding = ~batBLEncoding;
    batBLEncoding |= 1 << (kBATAreas - 1);

    return k128KBs << __builtin_ctz(batBLEncoding);
}

static void *GetBATEndAddress(u32 upperBAT) {
    char *const batStartAddress = (char *)GetBATBEPI(upperBAT);
    const u32 batAreaSize = GetBATAreaSize(GetBATBLEncoding(upperBAT));

    return batStartAddress + batAreaSize;
}

static bool ShouldUpdateIBAT1(void) {
    return (char *)payload_replacements_end > (char *)GetBATEndAddress(GetIBAT0U());
}

static u32 MakeBATBLEncoding(u32 shiftAmount) {
    assert(shiftAmount < kBATAreas);

    return ((1 << shiftAmount) - 1) << 2;
}

static u32 MakePayloadBATBLEncoding(void) {
    assert((char *)payload_replacements_end > (char *)GetBATEndAddress(GetIBAT0U()));
    const u32 payloadExecutableAreaSize =
            (char *)payload_replacements_end - (char *)GetBATEndAddress(GetIBAT0U());

    u32 batBLEncoding;
    u32 batAreaSize = k128KBs;
    for (size_t n = 0; n < kBATAreas; n++) {
        batBLEncoding = MakeBATBLEncoding(n);
        if (payloadExecutableAreaSize <= batAreaSize) {
            return batBLEncoding;
        }
        batAreaSize <<= 1;
    }
    return batBLEncoding;
}

static void SetNewIBAT1UValue(void) {
    const u32 IBAT1UBEPI = (u32)GetBATEndAddress(GetIBAT0U());
    const u32 IBAT1UBLEncoding = MakePayloadBATBLEncoding();
    const u32 IBAT1UVS = (1 << 1);
    const u32 IBAT1UVP = (1 << 0);

    newIBAT1U = IBAT1UBEPI | IBAT1UBLEncoding | IBAT1UVS | IBAT1UVP;
}

void OSDisableCodeExecOnMEM1Hi16MB(void) {
    // clang-format off
    BOOL enabled = OSDisableInterrupts();
    {
        RealMode(DisableInstsOnMEM1Hi16MB);
    }
    OSRestoreInterrupts(enabled);
    // clang-format on
}

void OSDisableCodeExecOnMEM2(void) {
    // clang-format off
    BOOL enabled = OSDisableInterrupts();
    {
        RealMode(DisableInstsOnMEM2);
    }
    OSRestoreInterrupts(enabled);
    // clang-format on
}

void OSEnableCodeExecOnPayload(void) {
    if (!ShouldUpdateIBAT1()) {
        return;
    }

    SetNewIBAT1UValue();

    // clang-format off
    BOOL enabled = OSDisableInterrupts();
    {
        RealMode(EnableInstsOnPayload);
    }
    OSRestoreInterrupts(enabled);
    // clang-format on
}
