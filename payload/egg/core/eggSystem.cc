#include "eggSystem.hh"
extern "C" {
#include <revolution.h>
}
#include <sp/Commands.h>
#include <sp/IOSDolphin.hh>

namespace {

const char *fmtBool(bool b) {
    constexpr const char *sOnOff[2] = { "OFF", "ON" };
    return sOnOff[!!b];
}

void emuspeed(u32 *out) {
    auto emuspeed = SP::IOSDolphin::GetSpeedLimit();
    OSReport("emuspeed: percent=%u,hasValue=%s\n", emuspeed ? *emuspeed : 0,
            fmtBool(emuspeed.has_value()));

    if (out != NULL && emuspeed.has_value())
        *out = *emuspeed;
}
void set_emuspeed(u32 speed) {
    const bool res = SP::IOSDolphin::SetSpeedLimit(speed) == IPC_OK;
    OSReport("set_emuspeed: success=%s\n", fmtBool(res));
}

void DolphinTest() {
    if (!SP::IOSDolphin::Open()) {
        OSReport("Failed to open IOSDolphin.\n");
        return;
    }

    {
        auto result = SP::IOSDolphin::GetSystemTime();
        OSReport("systime: milliseconds=%u,hasValue=%s\n",
                result.has_value() ? *result : 0, fmtBool(result.has_value()));
    }
    {
        auto version = SP::IOSDolphin::GetVersion();
        OSReport("version: version=%s,hasValue=%s\n", version ? version->data() : "?",
                fmtBool(version.has_value()));
    }
    {
        auto cpuspeed = SP::IOSDolphin::GetCPUSpeed();
        OSReport("cpuspeed: ticksPerSecond=%u,hasValue=%s\n", cpuspeed ? *cpuspeed : 0,
                fmtBool(cpuspeed.has_value()));
    }
    {
        // Test GetSpeedLimit, SetSpeedLimit
        u32 old_emu_speed = 0;
        emuspeed(&old_emu_speed);
        if (old_emu_speed) {
            set_emuspeed(50);
            // Report changed speed
            emuspeed(NULL);
            // Reset speed
            set_emuspeed(old_emu_speed);
            // Report changed speed
            emuspeed(NULL);
        } else {
            OSReport("set_emuspeed: skipping\n");
        }
    }
    {
        auto prod = SP::IOSDolphin::GetRealProductCode();
        OSReport("prod: code=%s,hasValue=%s\n", prod ? prod->data() : "?",
                fmtBool(prod.has_value()));
    }
}

sp_define_command("/dolphin_test", "Test /dev/dolphin driver", const char *tmp) {
    (void)tmp;

    DolphinTest();
}
}  // namespace

namespace EGG {

void *TSystem::mem1ArenaLo() const {
    return m_mem1ArenaLo;
}

void *TSystem::mem1ArenaHi() const {
    return m_mem1ArenaHi;
}

void *TSystem::mem2ArenaLo() const {
    return m_mem2ArenaLo;
}

void *TSystem::mem2ArenaHi() const {
    return m_mem2ArenaHi;
}

XfbManager *TSystem::xfbManager() {
    return m_xfbManager;
}

TSystem *TSystem::Instance() {
    return &s_instance;
}

} // namespace EGG
