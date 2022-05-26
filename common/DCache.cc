#include "DCache.hh"

namespace DCache {

void Store(const void *start, size_t size) {
    if (size == 0) {
        return;
    }
    size_t address = reinterpret_cast<size_t>(start);
    size = AlignUp(size, 0x20);
    do {
        asm("dcbst %y0" : : "Z"(*reinterpret_cast<u8 *>(address)));
        address += 0x20;
        size -= 0x20;
    } while (size > 0);
    asm("sync");
}

void Flush(const void *start, size_t size) {
    if (size == 0) {
        return;
    }
    size_t address = reinterpret_cast<size_t>(start);
    size = AlignUp(size, 0x20);
    do {
        asm("dcbf %y0" : : "Z"(*reinterpret_cast<u8 *>(address)));
        address += 0x20;
        size -= 0x20;
    } while (size > 0);
    asm("sync");
}

void Invalidate(void *start, size_t size) {
    if (size == 0) {
        return;
    }
    size_t address = reinterpret_cast<size_t>(start);
    size = AlignUp(size, 0x20);
    do {
        asm("dcbi %y0" : : "Z"(*reinterpret_cast<u8 *>(address)));
        address += 0x20;
        size -= 0x20;
    } while (size > 0);
}

} // namespace DCache
