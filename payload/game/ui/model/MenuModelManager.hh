#pragma once

#include "game/util/Registry.hh"

typedef void (*GameEntryFunc)(s8 idx);

namespace UI {

class DriverModel {
public:
    void setAnim(u32 localPlayerId, u32 r5);

private:
    u8 _00[0x14 - 0x00];

public:
    Registry::Vehicle m_vehicleId;

private:
    u8 _18[0x28 - 0x18];
};
static_assert(sizeof(DriverModel) == 0x28);

struct DriverModelHandle {
    u8 _0[0x8 - 0x0];
    DriverModel *model;
};
static_assert(sizeof(DriverModelHandle) == 0xc);

class DriverModelManager {
public:
    f32 getDelay() const;
    DriverModelHandle *handle(size_t index);

private:
    u8 _00[0x0c - 0x00];
    s32 m_delay;
    DriverModelHandle m_handles[4];
    u8 _40[0x54 - 0x40];
};
static_assert(sizeof(DriverModelManager) == 0x54);

class MenuModelManager {
public:
    DriverModelManager *driverModelManager();

    void init(u32 modelCount, GameEntryFunc onDriverModelLoaded);

    static MenuModelManager *Instance();

private:
    u8 _00[0x14 - 0x00];
    DriverModelManager *m_driverModelManager;
    u8 _18[0x40 - 0x18];

    static MenuModelManager *s_instance;
};
static_assert(sizeof(MenuModelManager) == 0x40);

} // namespace UI
