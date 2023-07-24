#include "Update.hh"

extern "C" {
#include "sp/Host.h"
}
#include "sp/net/Net.hh"
#include "sp/net/SyncSocket.hh"

#include <common/Bytes.hh>
#include <common/Paths.hh>

#include <protobuf/Update.pb.h>
extern "C" {
#include <revolution.h>
#include <revolution/nwc24/NWC24Utils.h>
}
#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

#include <algorithm>
#include <cstring>

namespace SP::Update {

#define TMP_CONTENTS_PATH "/tmp/contents.arc"

// clang-format off
static const u8 serverPK[hydro_kx_PUBLICKEYBYTES] = {
    0x6c, 0xcb, 0x71, 0xe8, 0x75, 0x2f, 0x58, 0x71, 0xfc, 0x17, 0x56, 0xf2, 0xea, 0xf8, 0x90, 0x06,
    0x72, 0x83, 0xb1, 0x6e, 0x7d, 0xa9, 0xa5, 0x60, 0x42, 0xf2, 0xcd, 0xa2, 0xa5, 0x32, 0x53, 0x5c,
};
static const u8 signPK[hydro_sign_PUBLICKEYBYTES] = {
    0x47, 0x55, 0xd5, 0x50, 0x15, 0x6b, 0x18, 0xf9, 0x38, 0x04, 0xc1, 0x93, 0x59, 0x0a, 0x4a, 0xf3,
    0x14, 0x21, 0x73, 0x47, 0xb0, 0x76, 0x40, 0xcb, 0x89, 0x42, 0x96, 0xd5, 0xb6, 0x32, 0x3f, 0x75,
};
// clang-format on
static Status status = Status::Idle;
static std::optional<Info> info;

Status GetStatus() {
    return status;
}

std::optional<Info> GetInfo() {
    return info;
}

static std::expected<void, const wchar_t *> Sync(bool update) {
    assert(versionInfo.type == BUILD_TYPE_RELEASE);

    status = Status::Connect;
    SP::Net::SyncSocket socket("update.mkw-sp.com", 21328, serverPK, "update  ");
    if (!socket.ok()) {
        return std::unexpected(L"Unable to connect to update.mkw-sp.com!");
    }

    status = Status::SendInfo;
    {
        u8 buffer[UpdateRequest_size];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        UpdateRequest request;
        request.wantsUpdate = update;
        request.versionMajor = versionInfo.major;
        request.versionMinor = versionInfo.minor;
        request.versionPatch = versionInfo.patch;
        NWC24iStrLCpy(request.gameName, OSGetAppGamename(), sizeof(request.gameName));
        NWC24iStrLCpy(request.hostPlatform, Host_GetPlatformString(), sizeof(request.hostPlatform));

        assert(pb_encode(&stream, UpdateRequest_fields, &request));
        TRY(socket.write(buffer, stream.bytes_written));
    }

    status = Status::ReceiveInfo;
    {
        u8 buffer[UpdateResponse_size];
        std::optional<u16> size = TRY(socket.read(buffer, sizeof(buffer)));
        if (!size) {
            return std::unexpected(L"Unable to recieve update information");
        }

        pb_istream_t stream = pb_istream_from_buffer(buffer, *size);

        UpdateResponse response;
        if (!pb_decode(&stream, UpdateResponse_fields, &response)) {
            return std::unexpected(L"Unable to decode update information");
        }

        Info newInfo{};
        newInfo.version.major = response.versionMajor;
        newInfo.version.minor = response.versionMinor;
        newInfo.version.patch = response.versionPatch;
        newInfo.size = response.size;
        assert(response.signature.size == sizeof(newInfo.signature));
        memcpy(newInfo.signature, response.signature.bytes, sizeof(newInfo.signature));
        if (!update) {
            if (newInfo.version > versionInfo) {
                info.emplace(newInfo);
            }
            status = Status::Idle;
            return {};
        } else if (memcmp(&*info, &newInfo, sizeof(Info))) {
            info.reset();
            return std::unexpected(L"Update information has changed since last checked!");
        }
    }

    status = Status::Download;
    {
        OSTime startTime = OSGetTime();
        hydro_sign_state state;
        if (hydro_sign_init(&state, "update  ") != 0) {
            return std::unexpected(L"Failed to initialise encryption");
        }
        NANDPrivateDelete(TMP_CONTENTS_PATH);
        u8 perms = NAND_PERM_OWNER_MASK | NAND_PERM_GROUP_MASK | NAND_PERM_OTHER_MASK;
        if (NANDPrivateCreate(TMP_CONTENTS_PATH, perms, 0) != NAND_RESULT_OK) {
            return std::unexpected(L"Failed to create nand file");
        }
        NANDFileInfo fileInfo;
        if (NANDPrivateOpen(TMP_CONTENTS_PATH, &fileInfo, NAND_ACCESS_WRITE) != NAND_RESULT_OK) {
            return std::unexpected(L"Failed to open nand file");
        }
        for (info->downloadedSize = 0; info->downloadedSize < info->size;) {
            alignas(0x20) u8 message[0x1000] = {};
            u16 chunkSize = std::min(info->size - info->downloadedSize, static_cast<u32>(0x1000));
            auto res = socket.read(message, chunkSize);
            if (!res) {
                NANDClose(&fileInfo);
                return std::unexpected(res.error());
            }
            if (hydro_sign_update(&state, message, chunkSize) != 0) {
                NANDClose(&fileInfo);
                return std::unexpected(L"Failed to verify update file part");
            }
            if (NANDWrite(&fileInfo, message, chunkSize) != chunkSize) {
                NANDClose(&fileInfo);
                return std::unexpected(L"Failed to write update file to nand");
            }
            info->downloadedSize += chunkSize;
            OSTime duration = OSGetTime() - startTime;
            info->throughput = OSSecondsToTicks(static_cast<u64>(info->downloadedSize)) / duration;
        }
        if (NANDClose(&fileInfo) != NAND_RESULT_OK) {
            return std::unexpected(L"Failed to close nand file");
        }
        if (hydro_sign_final_verify(&state, info->signature, signPK) != 0) {
            return std::unexpected(L"Failed to verify downloaded update file");
        }
    }

    status = Status::Move;
    if (NANDPrivateMove(TMP_CONTENTS_PATH, UPDATE_PATH) != NAND_RESULT_OK) {
        return std::unexpected(L"Failed to finalize update");
    }

    info->updated = true;
    status = Status::Idle;
    return {};
}

bool Check() {
    if (info) {
        status = Status::Idle;
        return true;
    }
    if (!Sync(false)) {
        SP::Net::Restart();
        return false;
    }
    return true;
}

bool Update() {
    assert(info);
    if (!Sync(true)) {
        info.reset();
        SP::Net::Restart();
        return false;
    }
    return true;
}

} // namespace SP::Update
