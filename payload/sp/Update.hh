#pragma once

extern "C" {
#include <libhydrogen/hydrogen.h>
}

#include <optional>

#include <Common.hh>

namespace SP::Update {

enum class Status {
    Connect,
    SendInfo,
    ReceiveInfo,
    Download,
    Move,
    Idle,
};

struct Info {
    VersionInfo version;
    u32 size;
    u8 signature[hydro_sign_BYTES];
    u32 downloadedSize;
    u32 throughput;
    bool updated;
};

Status GetStatus();
std::optional<Info> GetInfo();
bool Check();
bool Update();

} // namespace SP::Update
