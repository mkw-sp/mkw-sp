#include "Patcher.h"

__attribute__((section("first"))) void start(void) {
    Patcher_patch(PATCHER_BINARY_DOL);
}
