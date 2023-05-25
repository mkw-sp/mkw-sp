extern "C" {
#include "StackTrace.h"

#include "sp/Dol.h"
#include "sp/Payload.h"
#include "sp/Rel.h"
#include "sp/security/StackCanary.h"
}
#include "MapFile.hh"

extern "C" {
#include <revolution.h>
}

#include <stdio.h>
#include <stdlib.h>

static bool CheckPointer(u32 addr) {
    const u32 OFFSET_MASK = 0x0FFFFFFF;
    if (addr == 0 || (u32)addr == ~0u) {
        return false;
    }

    if (OSIsMEM1Region(addr)) {
        return (addr & OFFSET_MASK) < OSGetPhysicalMem1Size();
    }
    if (OSIsMEM2Region(addr)) {
        return (addr & OFFSET_MASK) < OSGetPhysicalMem2Size();
    }
    return false;
}

void StackTraceIterator_create(StackTraceIterator *it, void *begin) {
    it->cur = (StackTraceEntry *)begin;
    it->depth = 0;
    it->maxDepth = 15;
}

bool StackTraceIterator_read(StackTraceIterator *it, void **addr) {
    if (it->depth >= it->maxDepth) {
        return false;
    }
    if (!CheckPointer((u32)it->cur)) {
        return false;
    }

    if (addr != NULL) {
        void *lr = it->cur->LR;
        *addr = StackCanary_IsLinkRegisterEncrypted((u32)lr) ?
                (void *)StackCanary_XORLinkRegister((u32)lr) :
                lr;
    }

    ++it->depth;
    it->cur = it->cur->next;
    return true;
}

BinaryType ClassifyPointer(void *p) {
    if (p >= Payload_getTextSectionStart() && p < Payload_getReplacementsSectionEnd()) {
        return BINARY_SP;
    }
    if (p >= Rel_getTextSectionStart() && p < Rel_getTextSectionEnd()) {
        return BINARY_REL;
    }
    if (p >= Dol_getInitSectionStart() && p < Dol_getTextSectionEnd()) {
        return BINARY_DOL;
    }

    return BINARY_ILLEGAL;
}

void *PortPointer(void *p) {
    if (ClassifyPointer(p) == BINARY_REL) {
        u32 portedPointer = (u32)p - (u32)Rel_getTextSectionStart();

        switch (REGION) {
        case REGION_E:
            return (void *)(portedPointer + 0x8050C034);
        case REGION_P:
            return (void *)(portedPointer + 0x805103B4);
        case REGION_J:
            return (void *)(portedPointer + 0x8050FD34);
        case REGION_K:
            return (void *)(portedPointer + 0x804FE3D4);
        }
    }

    return p;
}

size_t WriteStackTraceShort(char *buf, int capacity, void *sp) {
    StackTraceIterator it;
    StackTraceIterator_create(&it, sp);

    size_t l = 0;
    void *lr = NULL;
    while (StackTraceIterator_read(&it, &lr)) {
        void *ported = PortPointer(lr);

        const char *pointerFlag = "";
        if (ClassifyPointer(lr) == BINARY_REL) {
            pointerFlag = ported == lr ? ":SP" : "*";
        }

        const char *funcName = "?";
        switch (ClassifyPointer(lr)) {
        case BINARY_DOL:
            funcName = "DOL";
            break;
        case BINARY_REL:
            funcName = "REL";
            break;
        case BINARY_SP:
            funcName = "SP";
            break;
        case BINARY_ILLEGAL:
            funcName = "ILLEGAL";
            break;
        }

        auto sym = SP::MapFile::SymbolLowerBound(reinterpret_cast<uintptr_t>(lr));
        if (sym && !SP::MapFile::ScoreMatch(sym->address, reinterpret_cast<uintptr_t>(lr))) {
            sym = std::nullopt;
        }
        char fmtSym[256] = "<- ";
        if (sym) {
            intptr_t delta = reinterpret_cast<intptr_t>(lr) - static_cast<intptr_t>(sym->address);
            auto name = sym->name;
            snprintf(fmtSym, sizeof(fmtSym), "%.*s [%c0x%X]\n", name.size(), name.data(),
                    "+-"[delta < 0], std::abs(delta));
            if (fmtSym[sizeof(fmtSym) - 2] != '\0') {
                fmtSym[sizeof(fmtSym) - 2] = '\n';
            }
        }

        l += snprintf(buf + l, capacity - l, "%p%s %s %s", ported, pointerFlag, funcName, fmtSym);
    }

    return l;
}
