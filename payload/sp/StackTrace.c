#include "StackTrace.h"

#include "sp/Dol.h"
#include "sp/Payload.h"
#include "sp/Rel.h"

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
        *addr = it->cur->LR;
    }

    ++it->depth;
    it->cur = it->cur->next;
    return true;
}

BinaryType ClassifyPointer(void *p) {
    if (p >= Payload_getTextSectionStart() && p < Payload_getTextSectionEnd()) {
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
        switch (REGION) {
        case REGION_P:
            return (void *)((u32)p - (u32)Rel_getTextSectionStart() + 0x805103B4);
        default:
            // No developers are on other regions
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

        l += snprintf(
                buf + l, capacity - l, "at %p%s %s\n", ported, pointerFlag, funcName);
    }

    return l;
}
