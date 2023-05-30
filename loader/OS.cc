#include "OS.hh"

namespace OS {

#define RETAIL_PHYSICAL_MEM2_SIZE 0x04000000

static u32 GetPhysicalMEM2Size() {
    return *reinterpret_cast<u32 *>(0x80003118);
}

bool IsDevelopmentConsole() {
    return GetPhysicalMEM2Size() > RETAIL_PHYSICAL_MEM2_SIZE;
}

} // namespace OS
