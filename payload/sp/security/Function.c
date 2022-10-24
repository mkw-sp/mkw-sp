#include "Function.h"

#include <revolution/os.h>

#include <string.h>

typedef struct BlacklistedFunction {
    char *startAddress;
    char *endAddress;
} BlacklistedFunction;

static const BlacklistedFunction blacklistedFunctions[1] = {
        {
                (char *)BATConfig,
                (char *)__OSInitMemoryProtection,
        },
};

void Function_KillBlacklistedFunctions(void) {
    for (size_t n = 0; n < ARRAY_SIZE(blacklistedFunctions); n++) {
        const BlacklistedFunction *blacklistedFunction = &blacklistedFunctions[n];
        char *startAddress = blacklistedFunction->startAddress;
        char *endAddress = blacklistedFunction->endAddress;
        u32 functionLength = endAddress - startAddress;

        assert(((u32)startAddress & 3) == 0);
        assert(((u32)endAddress & 3) == 0);
        assert(startAddress < endAddress);

        memset(startAddress, 0, functionLength);
        DCFlushRange(startAddress, functionLength);
        ICInvalidateRange(startAddress, functionLength);
    }
}
