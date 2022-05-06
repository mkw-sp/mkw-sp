#pragma once

extern "C" {
#include <revolution.h>
}

namespace SP {

class DVDFile {
public:
    DVDFile(const char *path);
    ~DVDFile();
    u32 size() const;
    u32 alignedSize() const;
    s32 read(void *dst, s32 size, s32 offset);
    bool ok() const;

private:
    DVDFileInfo m_info;
    bool m_ok;
};

} // namespace SP
