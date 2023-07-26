#pragma once

#include <optional>

#include <Common.hh>

namespace IOS {

void Init();
u16 GetNumber();
bool EscalatePrivileges(bool again);
#ifdef SP_LOADER
void DeescalatePrivileges(std::optional<u32> newKeyAddress);
std::optional<std::optional<u32>> ImportNewCommonKey();
#else
void DeescalatePrivileges();
#endif
bool IsDolphin();

enum class Mode : u32 {
    None = 0,
    Read = 1 << 0,
    Write = 1 << 1,
    Both = Write | Read,
};

class Resource {
public:
    struct IoctlvPair {
        void *data;
        u32 size;
    };

    Resource(const Resource &) = delete;
    Resource &operator=(const Resource &) = delete;
    Resource(s32 fd);
    Resource(const char *path, Mode mode);
    ~Resource();
    s32 ioctl(u32 ioctl, const void *input, u32 inputSize, void *output, u32 outputSize);
    s32 ioctlv(u32 ioctlv, u32 inputCount, u32 outputCount, IoctlvPair *pairs);
    bool ioctlvReboot(u32 ioctlv, u32 inputCount, IoctlvPair *pairs);
    bool ok() const;

private:
    s32 open(const char *path, Mode mode);
    s32 close();

protected:
    s32 m_fd = -1;
};

class File : protected Resource {
public:
    struct Stats {
        u32 size;
        u32 offset;
    };

    File(s32 fd);
    File(const char *path, Mode mode);
    ~File();
    s32 read(void *output, u32 outputSize);
    s32 write(const void *input, u32 inputSize);
    s32 getStats(Stats *stats);
    using Resource::ok;
};

} // namespace IOS
