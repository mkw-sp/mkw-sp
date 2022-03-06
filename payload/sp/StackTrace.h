#pragma once

#include <Common.h>

typedef struct StackTraceEntry StackTraceEntry;

struct StackTraceEntry {
    StackTraceEntry *next;
    void *LR;
};

typedef struct {
    StackTraceEntry *cur;
    int depth;
    int maxDepth;
} StackTraceIterator;

void StackTraceIterator_create(StackTraceIterator *it, void *begin);
bool StackTraceIterator_read(StackTraceIterator *it, void **addr);

typedef enum {
    BINARY_SP,
    BINARY_REL,
    BINARY_DOL,
    BINARY_ILLEGAL,
} BinaryType;

//! Determine which executable a pointer belongs to
BinaryType ClassifyPointer(void *p);

//! Try to convert a MKW-SP address-space REL pointer to vanilla address-space.
void *PortPointer(void *p);

//! For OSFatal asserts, which has a small text buffer, and we have the source line number
//! already.
size_t WriteStackTraceShort(char *buf, int capacity, void *sp);
