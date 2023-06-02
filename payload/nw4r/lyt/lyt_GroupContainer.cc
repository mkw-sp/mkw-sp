#include "Common.hh"

extern "C" void *REPLACED(GroupContainer_FindGroupByName)(void *a, const char *b);
extern "C" REPLACE void *GroupContainer_FindGroupByName(void *a, const char *b) {
    void *r = REPLACED(GroupContainer_FindGroupByName)(a, b);
    if (r == nullptr) {
        panic("Failed to find group %s", b);
    }
    return r;
}
