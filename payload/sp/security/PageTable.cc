extern "C" {
#include "PageTable.h"
}

extern "C" {
#include "sp/Dol.h"
#include "sp/Payload.h"
#include "sp/Rel.h"
}

#include <algorithm>

#include <Common.hh>

extern "C" void InitSRs();
extern "C" void SetSDR1(u32 sdr1);

namespace SP::PageTable {

#define PAGE_TABLE_MEMORY_POWER_OF_2 16 // 64 Kibibytes
static_assert(PAGE_TABLE_MEMORY_POWER_OF_2 > 15 && PAGE_TABLE_MEMORY_POWER_OF_2 < 26);
#define PAGE_TABLE_SIZE (1 << PAGE_TABLE_MEMORY_POWER_OF_2)
#define PAGE_SIZE (1 << 12)

#define HTABMASK ~(~0 << (PAGE_TABLE_MEMORY_POWER_OF_2 - 16))
#define HTABORG_MASK 0xFFFF0000

enum WIMG {
    // clang-format off
    WIMG_NONE              = 0 << 0,
    WIMG_GUARDED           = 1 << 0,
    WIMG_MEMORY_COHERENCE  = 1 << 1,
    WIMG_CACHING_INHIBITED = 1 << 2,
    WIMG_WRITE_THROUGH     = 1 << 3,
    // clang-format on
};

enum PP {
    // clang-format off
    PP_NONE           = 0b00,
    PP_READ_ONLY      = 0b01,
    PP_READ_WRITE     = 0b10,
    PP_FULL_READ_ONLY = 0b11,
    // clang-format on
};

struct PageTableEntryInfo {
    const void *physicalAddress;
    const void *effectiveAddress;
    u32 size;
    WIMG wimg;
    PP pp;
    u32 sr;
};

union PageTableEntry {
    struct {
        // clang-format off
        u32 V    :  1;
        u32 VSID : 24;
        u32 H    :  1;
        u32 API  :  6;

        u32 RPN  : 20;
        u32      :  3;
        u32 R    :  1;
        u32 C    :  1;
        u32 WIMG :  4;
        u32      :  1;
        u32 PP   :  2;
        // clang-format on
    };

    u32 hilo[2];
};

alignas(PAGE_TABLE_SIZE) static char pageTable[PAGE_TABLE_SIZE];

template <u32 n>
static u32 GetSR() {
    u32 SR;
    asm("mfsr %0, %1" : "=r"(SR) : "n"(n));
    return SR;
}

static void InvalidateAllTLBEntries() {
    for (u32 entry = 0; entry < PAGE_SIZE * 64; entry += PAGE_SIZE) {
        asm("tlbie %0, 0" : : "r"(entry));
    }
    asm("tlbsync");
}

static void InitPageTable() {
    u32 HTABORG = (u32)pageTable & HTABORG_MASK;

    InvalidateAllTLBEntries();
    SetSDR1(VIRTUAL_TO_PHYSICAL(HTABORG) | HTABMASK);
}

static bool AddEntryToPageTable(const void *physicalAddress, const void *effectiveAddress,
        WIMG wimg, PP pp, u32 sr) {
    u32 VSID = sr & 0x7FFFF;
    u32 pageIndex = ((u32)effectiveAddress >> 12) & 0xFFFF;

    u32 primaryHash = VSID ^ pageIndex;
    u32 secondaryHash = ~primaryHash;
    const std::array<u32, 2> hashArray = {
            primaryHash,
            secondaryHash,
    };

    u32 hash;
    PageTableEntry *pte = nullptr;
    for (size_t hashIndex = 0; hashIndex < hashArray.size(); hashIndex++) {
        u32 offset = ((hashArray[hashIndex] & (HTABMASK << 10)) | (hashArray[hashIndex] & 0x3FF)) << 6;
        PageTableEntry *pteg = (PageTableEntry *)(pageTable + offset);

        PageTableEntry *it = std::find_if(pteg, pteg + 8, [](PageTableEntry &pte) { return !pte.V; });
        if (it != pteg + 8) {
            hash = hashIndex;
            pte = it;
            break;
        }
    }

    if (!pte) {
        return false;
    }

    const PageTableEntry entry = {
            .V = 1,
            .VSID = VSID,
            .H = hash,
            .API = (u32)effectiveAddress >> 22,

            .RPN = (u32)physicalAddress >> 12,
            .R = 0,
            .C = 0,
            .WIMG = wimg,
            .PP = pp,
    };

    pte->hilo[1] = entry.hilo[1];
    asm volatile("eieio");
    pte->hilo[0] = entry.hilo[0];
    asm volatile("sync");

    return true;
}

static const std::array<PageTableEntryInfo, 3> pteInfoArray = {
        PageTableEntryInfo{
                (const void *)VIRTUAL_TO_PHYSICAL(Dol_getInitSectionStart()),
                Dol_getInitSectionStart(),
                (u32)Dol_getTextSectionEnd() - (u32)Dol_getInitSectionStart(),
                WIMG_NONE,
                PP_FULL_READ_ONLY,
                GetSR<8>(),
        },
        PageTableEntryInfo{
                (const void *)VIRTUAL_TO_PHYSICAL(Rel_getTextSectionStart()),
                Rel_getTextSectionStart(),
                Rel_getTextSectionSize(),
                WIMG_NONE,
                PP_FULL_READ_ONLY,
                GetSR<8>(),
        },
        PageTableEntryInfo{
                (const void *)VIRTUAL_TO_PHYSICAL(Payload_getTextSectionStart()),
                Payload_getTextSectionStart(),
                (u32)Payload_getReplacementsSectionEnd() - (u32)Payload_getTextSectionStart(),
                WIMG_NONE,
                PP_FULL_READ_ONLY,
                GetSR<8>(),
        },
};

static void Init() {
    InitSRs();
    InitPageTable();

    for (const PageTableEntryInfo &pteInfo : pteInfoArray) {
        char *physicalStartAddress = (char *)ROUND_DOWN(pteInfo.physicalAddress, PAGE_SIZE);
        char *effectiveStartAddress = (char *)ROUND_DOWN(pteInfo.effectiveAddress, PAGE_SIZE);
        char *effectiveEndAddress =
                (char *)ROUND_UP((char *)pteInfo.effectiveAddress + pteInfo.size, PAGE_SIZE);
        u32 size = effectiveEndAddress - effectiveStartAddress;

        assert(!(size % PAGE_SIZE));

        for (u32 offset = 0; offset != size; offset += PAGE_SIZE) {
            const void *physicalAddress = physicalStartAddress + offset;
            const void *effectiveAddress = effectiveStartAddress + offset;

            assert(AddEntryToPageTable(physicalAddress, effectiveAddress, pteInfo.wimg, pteInfo.pp,
                    pteInfo.sr));
        }
    }
}

} // namespace SP::PageTable

extern "C" void PageTable_Init() {
    SP::PageTable::Init();
}
