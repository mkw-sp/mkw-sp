#pragma once

struct TypeInfo;

typedef struct TypeInfo {
    struct TypeInfo *base;
} TypeInfo;

inline bool TypeInfo_isDerived(TypeInfo *base, TypeInfo *derived) {
    while (derived != NULL) {
        if (derived == base) {
            return true;
        }

        derived = derived->base;
    }
    return false;
}
