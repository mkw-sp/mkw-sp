#include "Update.hh"


extern "C" {
#include "sp/Host.h"
}
#include "sp/net/Net.hh"
#include "sp/net/Socket.hh"

#include <common/Bytes.hh>
extern "C" {
#include <common/Paths.h>
}

#include <protobuf/UpdateRequestMessage.pb.h>
extern "C" {
#include <revolution/nwc24/NWC24Utils.h>
}
#include <vendor/nanopb/pb_encode.h>

#include <algorithm>
#include <cstring>

namespace SP::Update {

#define TMP_CONTENTS_PATH "/tmp/contents.arc"

static const u8 serverPK[hydro_kx_PUBLICKEYBYTES] = {
    0x6c, 0xcb, 0x71, 0xe8, 0x75, 0x2f, 0x58, 0x71, 0xfc, 0x17, 0x56, 0xf2, 0xea, 0xf8, 0x90, 0x06,
    0x72, 0x83, 0xb1, 0x6e, 0x7d, 0xa9, 0xa5, 0x60, 0x42, 0xf2, 0xcd, 0xa2, 0xa5, 0x32, 0x53, 0x5c,
};
static const u8 signPK[hydro_sign_PUBLICKEYBYTES] = {
    0x47, 0x55, 0xd5, 0x50, 0x15, 0x6b, 0x18, 0xf9, 0x38, 0x04, 0xc1, 0x93, 0x59, 0x0a, 0x4a, 0xf3,
    0x14, 0x21, 0x73, 0x47, 0xb0, 0x76, 0x40, 0xcb, 0x89, 0x42, 0x96, 0xd5, 0xb6, 0x32, 0x3f, 0x75,
};
static Status status = Status::Idle;
static std::optional<Info> info;

Status GetStatus() {
    return status;
}

std::optional<Info> GetInfo() {
    return info;
}

static bool Sync(bool update) {
    status = Status::Connect;
    SP::Net::Socket socket("update.mkw-sp.com", 0x5350, serverPK, "update  ");
    if (!socket.ok()) {
        return false;
    }

    status = Status::SendInfo;
    {
        u8 buffer[UpdateRequestMessage_size];
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        UpdateRequestMessage message;
        message.wantsUpdate  = update;
        message.versionMajor = versionInfo.major;
        message.versionMinor = versionInfo.minor;
        message.versionPatch = versionInfo.patch;
        NWC24iStrLCpy(message.gameName, OSGetAppGamename(), sizeof(message.gameName));
        NWC24iStrLCpy(message.hostPlatform, Host_GetPlatformString(), sizeof(message.hostPlatform));

        assert(pb_encode(&stream, UpdateRequestMessage_fields, &message));

        if (!socket.write(buffer, stream.bytes_written)) {
             return false;
        }
    }

    status = Status::ReceiveInfo;
    {
        u8 message[78] = {};
        if (!socket.read(message, sizeof(message))) {
            return false;
        }
        Info newInfo{};
        newInfo.version.major = Bytes::Read<u16>(message, 0);
        newInfo.version.minor = Bytes::Read<u16>(message, 2);
        newInfo.version.patch = Bytes::Read<u16>(message, 4);
        newInfo.size = Bytes::Read<u32>(message, 6);
        memcpy(newInfo.signature, message + 14, sizeof(newInfo.signature));
        if (!update) {
            if (newInfo.version > versionInfo) {
                info.emplace(newInfo);
            }
            status = Status::Idle;
            return true;
        } else if (memcmp(&*info, &newInfo, sizeof(Info))) {
            info.reset();
            return false;
        }
    }

    status = Status::Download;
    {
        OSTime startTime = OSGetTime();
        hydro_sign_state state;
        if (hydro_sign_init(&state, "update  ") != 0) {
            return false;
        }
        NANDPrivateDelete(TMP_CONTENTS_PATH);
        u8 perms = NAND_PERM_OWNER_MASK | NAND_PERM_GROUP_MASK | NAND_PERM_OTHER_MASK;
        if (NANDPrivateCreate(TMP_CONTENTS_PATH, perms, 0) != NAND_RESULT_OK) {
            return false;
        }
        NANDFileInfo fileInfo;
        if (NANDPrivateOpen(TMP_CONTENTS_PATH, &fileInfo, NAND_ACCESS_WRITE) != NAND_RESULT_OK) {
            return false;
        }
        for (info->downloadedSize = 0; info->downloadedSize < info->size;) {
            alignas(0x20) u8 message[0x1000] = {};
            u16 chunkSize = std::min(info->size - info->downloadedSize, static_cast<u32>(0x1000));
            if (!socket.read(message, chunkSize)) {
                NANDClose(&fileInfo);
                return false;
            }
            if (hydro_sign_update(&state, message, chunkSize) != 0) {
                NANDClose(&fileInfo);
                return false;
            }
            if (NANDWrite(&fileInfo, message, chunkSize) != chunkSize) {
                NANDClose(&fileInfo);
                return false;
            }
            info->downloadedSize += chunkSize;
            OSTime duration = OSGetTime() - startTime;
            info->throughput = OSSecondsToTicks(static_cast<u64>(info->downloadedSize)) / duration;
        }
        if (NANDClose(&fileInfo) != NAND_RESULT_OK) {
            return false;
        }
        if (hydro_sign_final_verify(&state, info->signature, signPK) != 0) {
            return false;
        }
    }

    status = Status::Move;
    if (NANDPrivateMove(TMP_CONTENTS_PATH, UPDATE_PATH) != NAND_RESULT_OK) {
        return false;
    }

    info->updated = true;
    status = Status::Idle;
    return true;
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
