#include "StackCanary.hh"

extern "C" {
#include <revolution/os/OSCache.h>
}

#include <algorithm>
#include <array>

typedef bool (*Find)(u32 *startAddress);
typedef void (*LinkRegisterFunction)();

typedef struct LinkRegisterPatch {
    Find findPrologue;
    Find findLRSave;
    Find findLRRestore;
    Find findEpilogue;

    u32 prologueInstCount;
    u32 epilogueInstCount;

    LinkRegisterFunction newLRSaveFunc;
    LinkRegisterFunction newLRRestoreFunc;
} LinkRegisterPatch;

extern "C" void StackCanary_SaveAlignedLinkRegister();
extern "C" void StackCanary_RestoreAlignedLinkRegister();
extern "C" void StackCanary_SaveLinkRegister();
extern "C" void StackCanary_RestoreLinkRegister();

namespace SP::StackCanary {

static const u32 blr = 0x4E800020;
static const u32 mflr = 0x7C0802A6;
static const u32 mtlr = 0x7C0803A6;

static bool IsAlignedPrologue(u32 *startAddress) {
    const u32 stwux = 0x7C21596E;

    return startAddress[0] == stwux && startAddress[1] == mflr;
}

static bool IsAlignedLinkRegisterSave(u32 *startAddress) {
    const u32 stw = 0x900C0004;

    return startAddress[0] == stw;
}

static bool IsAlignedLinkRegisterRestore(u32 *startAddress) {
    const u32 lwz = 0x800A0004;

    return startAddress[0] == lwz;
}

static bool IsAlignedEpilogue(u32 *startAddress) {
    const u32 mr = 0x7D415378;

    return startAddress[0] == mtlr && startAddress[1] == mr && startAddress[2] == blr;
}

static bool IsPrologue(u32 *startAddress) {
    const u16 stwu = 0x9421;

    return (startAddress[0] >> 16) == stwu && startAddress[1] == mflr;
}

static bool IsLinkRegisterSave(u32 *startAddress) {
    const u16 stw = 0x9001;

    return (startAddress[0] >> 16) == stw;
}

static bool IsLinkRegisterRestore(u32 *startAddress) {
    const u16 lwz = 0x8001;

    return (startAddress[0] >> 16) == lwz;
}

static bool IsEpilogue(u32 *startAddress) {
    const u16 addi = 0x3821;

    return startAddress[0] == mtlr && (startAddress[1] >> 16) == addi && startAddress[2] == blr;
}

static const std::array<LinkRegisterPatch, 2> lrPatches = {
        // clang-format off
        LinkRegisterPatch
        {
            .findPrologue = IsPrologue,
            .findLRSave = IsLinkRegisterSave,
            .findLRRestore = IsLinkRegisterRestore,
            .findEpilogue = IsEpilogue,

            .prologueInstCount = 2,
            .epilogueInstCount = 3,

            .newLRSaveFunc = StackCanary_SaveLinkRegister,
            .newLRRestoreFunc = StackCanary_RestoreLinkRegister,
        },
        LinkRegisterPatch
        {
            .findPrologue = IsAlignedPrologue,
            .findLRSave = IsAlignedLinkRegisterSave,
            .findLRRestore = IsAlignedLinkRegisterRestore,
            .findEpilogue = IsAlignedEpilogue,

            .prologueInstCount = 2,
            .epilogueInstCount = 3,

            .newLRSaveFunc = StackCanary_SaveAlignedLinkRegister,
            .newLRRestoreFunc = StackCanary_RestoreAlignedLinkRegister,
        },
        // clang-format on
};

static u32 *FindFirst(u32 *startAddress, u32 *endAddress, Find find, u32 instCount) {
    while (startAddress + instCount <= endAddress) {
        if (find(startAddress)) {
            return startAddress;
        }
        startAddress++;
    }

    return NULL;
}

static u32 *FindLast(u32 *startAddress, u32 *endAddress, Find find, u32 instCount) {
    while (endAddress - instCount >= startAddress) {
        if (find(endAddress - instCount)) {
            return endAddress - instCount;
        }
        endAddress--;
    }

    return NULL;
}

static u32 CreateBranchLinkInstruction(u32 *source, u32 *destination) {
    char *sourceAddress = reinterpret_cast<char *>(source);
    char *destinationAddress = reinterpret_cast<char *>(destination);

    return (18 << 26) | ((destinationAddress - sourceAddress) & 0x3FFFFFC) | (1 << 0);
}

void AddLinkRegisterPatches(u32 *start, u32 *end) {
    assert((reinterpret_cast<u32>(start) & 3) == 0);
    assert((reinterpret_cast<u32>(end) & 3) == 0);

    for (const LinkRegisterPatch &lrPatch : lrPatches) {
        u32 *startAddress = start;
        u32 *endAddress = end;

        while (startAddress < endAddress) {
            u32 *prologue = FindFirst(startAddress, endAddress, lrPatch.findPrologue,
                    lrPatch.prologueInstCount);
            if (!prologue) {
                break;
            }

            u32 *epilogue = FindFirst(prologue + lrPatch.prologueInstCount, endAddress,
                    lrPatch.findEpilogue, lrPatch.epilogueInstCount);
            if (!epilogue) {
                break;
            }

            u32 *lrSave = FindFirst(prologue + lrPatch.prologueInstCount, epilogue,
                    lrPatch.findLRSave, 1);
            if (!lrSave) {
                startAddress = epilogue + lrPatch.epilogueInstCount;
                continue;
            }

            u32 *lrRestore = FindLast(lrSave + 1, epilogue, lrPatch.findLRRestore, 1);
            if (!lrRestore) {
                startAddress = epilogue + lrPatch.epilogueInstCount;
                continue;
            }

            // Skip over functions that do not return
            u32 *nextPrologue = FindFirst(prologue + lrPatch.prologueInstCount, endAddress,
                    lrPatch.findPrologue, lrPatch.prologueInstCount);
            if (nextPrologue && nextPrologue < epilogue) {
                startAddress = nextPrologue;
                continue;
            }

            *lrSave = CreateBranchLinkInstruction(lrSave,
                    reinterpret_cast<u32 *>(lrPatch.newLRSaveFunc));
            *lrRestore = CreateBranchLinkInstruction(lrRestore,
                    reinterpret_cast<u32 *>(lrPatch.newLRRestoreFunc));

            DCFlushRange(lrSave, sizeof(*lrSave));
            DCFlushRange(lrRestore, sizeof(*lrRestore));
            ICInvalidateRange(lrSave, sizeof(*lrSave));
            ICInvalidateRange(lrRestore, sizeof(*lrRestore));

            startAddress = epilogue + lrPatch.epilogueInstCount;
        }
    }
}

} // namespace SP::StackCanary
