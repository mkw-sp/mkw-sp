#pragma once

#include <common/IOS.hh>

namespace IOS {

class DI final : private Resource {
public:
    DI();
    ~DI() = default;

    bool readDiskID();
    bool readUnencrypted(void *dst, u32 size, u32 offset);
    bool openPartition(u32 offset);
    bool read(void *dst, u32 size, u32 offset);
    bool isInserted();
    bool reset();

private:
    static const char s_path[];
};

} // namespace IOS
