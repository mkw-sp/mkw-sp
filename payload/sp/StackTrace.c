#include "StackTrace.h"

#include "sp/Dol.h"
#include "sp/Payload.h"
#include "sp/Rel.h"
#include "sp/security/Stack.h"

#include <revolution.h>
#include <stdio.h>

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
        *addr = Stack_IsLinkRegisterEncrypted((u32 *)lr) ? Stack_XORLinkRegister((u32 *)lr) : lr;
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
        default:
            return p;
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

        l += snprintf(buf + l, capacity - l, "@ %p%s %s\n", ported, pointerFlag, funcName);
    }

    return l;
}
