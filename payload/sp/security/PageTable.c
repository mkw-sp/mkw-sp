#include "PageTable.h"

#include <Common.h>
#include <string.h>
#include "sp/Dol.h"
#include "sp/Rel.h"
#include "sp/Payload.h"

#define PAGE_TABLE_MEMORY_POWER_OF_2 16 // 64 Kibibytes
static_assert(PAGE_TABLE_MEMORY_POWER_OF_2 > 15 && PAGE_TABLE_MEMORY_POWER_OF_2 < 26);
#define PAGE_TABLE_SIZE (1 << PAGE_TABLE_MEMORY_POWER_OF_2)

#define HTABORG_MASK 0xFFFF0000
#define HTABMASK ~(~0U << (PAGE_TABLE_MEMORY_POWER_OF_2 - 16))

#define GetSR(n) \
    ({ \
        u32 SR; \
        asm("mfsr %0, " SP_TOSTRING(n) : "=r"(SR)); \
        SR; \
    })

typedef enum WIMG {
    // clang-format off
    WIMG_GUARDED           = 1 << 0,
    WIMG_MEMORY_COHERENCE  = 1 << 1,
    WIMG_CACHING_INHIBITED = 1 << 2,
    WIMG_WRITE_THROUGH     = 1 << 3,
    // clang-format on
} WIMG;

typedef enum PP {
    // clang-format off
    PP_NONE           = 0b00,
    PP_READ_ONLY      = 0b01,
    PP_READ_WRITE     = 0b10,
    PP_FULL_READ_ONLY = 0b11,
    // clang-format on
} PP;

typedef union PageTableEntry {
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
} PageTableEntry;

typedef struct PageTableEntryInfo {
    void *physicalAddress;
    void *effectiveAddress;
    s32 size;
    WIMG wimg;
    PP pp;
    u32 sr;
} PageTableEntryInfo;

__attribute__((aligned(PAGE_TABLE_SIZE))) static char pageTable[PAGE_TABLE_SIZE];

static bool AddEntryToPageTable(PageTableEntryInfo *pteInfo);

static u32 GetSDR1(void) {
    u32 sdr1;
    asm("mfsdr1 %0" : "=r"(sdr1));
    return sdr1;
}

static void SetSDR1(u32 sdr1) {
    asm("mtsdr1 %0" : : "r"(sdr1));
}

static void SetUpPageTable(void) {
    memset(pageTable, 0, sizeof(pageTable));

    PageTableEntryInfo Dol[1] = {{
        .physicalAddress = (void*)((u32)Dol_getInitSectionStart() & ~0x80000000),
        .effectiveAddress = Dol_getInitSectionStart(),
        .size = ((s32)Dol_getTextSectionEnd())-((s32)Dol_getInitSectionStart()&~0xFFF),
        .wimg = 0,
        .pp = PP_FULL_READ_ONLY,
        .sr = GetSR(0x8)
    }};
    PageTableEntryInfo Rel[1] = {{
        .physicalAddress = (void*)((u32)Rel_getTextSectionStart() & ~0x80000000),
        .effectiveAddress = Rel_getTextSectionStart(),
        .size = ((s32)Rel_getTextSectionEnd())-((s32)Rel_getTextSectionStart()&~0xFFF),
        .wimg = 0,
        .pp = PP_FULL_READ_ONLY,
        .sr = GetSR(0x8)
    }};
    PageTableEntryInfo PayloadText[1] = {{
        .physicalAddress = (void*)((u32)Payload_getTextSectionStart() & ~0x80000000),
        .effectiveAddress = Payload_getTextSectionStart(),
        .size = ((s32)Payload_getTextSectionEnd())-((s32)Payload_getTextSectionStart()&~0xFFF),
        .wimg = 0,
        .pp = PP_FULL_READ_ONLY,
        .sr = GetSR(0x8)
    }};
     PageTableEntryInfo PayloadReplacements[1] = {{
        .physicalAddress = (void*)((u32)Payload_getReplacementsStart() & ~0x80000000),
        .effectiveAddress = Payload_getReplacementsStart(),
        .size = ((s32)Payload_getReplacementsEnd())-((s32)Payload_getReplacementsStart()&~0xFFF),
        .wimg = 0,
        .pp = PP_FULL_READ_ONLY,
        .sr = GetSR(0x8)
    }};
    PageTableEntryInfo *pageTableMaps[] = {
        Dol,
        Rel,
        PayloadText,
        PayloadReplacements,
        0
    };

    u32 HTABORG = (u32)pageTable & HTABORG_MASK;
    SetSDR1(HTABORG | HTABMASK);
        for (size_t index = 0;pageTableMaps[index];index++) {
            for (;pageTableMaps[index]->size > 0;pageTableMaps[index]->size -= 0x1000) {
                assert(AddEntryToPageTable(pageTableMaps[index]));
                pageTableMaps[index]->physicalAddress+=0x1000;
                pageTableMaps[index]->effectiveAddress+=0x1000;
            }
        }
}

static bool AddEntryToPageTable(PageTableEntryInfo *pteInfo) {
    u32 VSID = pteInfo->sr & 0x7FFFF;
    u32 pageIndex = ((u32)pteInfo->effectiveAddress >> 12) & 0xFFFF;

    u32 primaryHash = VSID ^ pageIndex;
    u32 secondaryHash = ~primaryHash;
    u32 hashArray[] = {
            primaryHash,
            secondaryHash,
    };
    static_assert((sizeof(hashArray) / sizeof(u32)) == 2);

    char *pageTableBase = (char *)(GetSDR1() & HTABORG_MASK);
    for (size_t hashIndex = 0; hashIndex < (sizeof(hashArray) / sizeof(u32)); hashIndex++) {
        u32 index = ((hashArray[hashIndex] & (HTABMASK << 10)) | (hashArray[hashIndex] & 0x3FF)) << 6;
        PageTableEntry *pteg = (PageTableEntry *)(pageTableBase + index);

        for (int entryIndex = 0; entryIndex < 8; entryIndex++) {
            PageTableEntry *pte = pteg + entryIndex;
            if (pte->V) {
                continue;
            }

            PageTableEntry entry = {
                    // clang-format off
                    .V = 1,
                    .VSID = VSID,
                    .H = hashIndex,
                    .API = (u32)pteInfo->effectiveAddress >> 22,

                    .RPN = (u32)pteInfo->physicalAddress >> 12,
                    .R = 0,
                    .C = 0,
                    .WIMG = pteInfo->wimg,
                    .PP = pteInfo->pp,
                    // clang-format on
            };

            pte->hilo[1] = entry.hilo[1];
            asm volatile("eieio");
            pte->hilo[0] = entry.hilo[0];
            asm volatile("sync");

            return true;
        }
    }

    return false;
}

void PageTable_Init(void) {
    SetUpPageTable();
}
