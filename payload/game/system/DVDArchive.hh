#pragma once

#include <Common.hh>

namespace System {

class DVDArchive {
public:
    void *REPLACED(getFile)(const char *path, size_t *size);
    REPLACE void *getFile(const char *path, size_t *size);

private:
    u8 _00[0x24 - 0x00];
};
static_assert(sizeof(DVDArchive) == 0x24);

} // namespace System
