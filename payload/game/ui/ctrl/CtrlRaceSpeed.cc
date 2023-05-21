#include "CtrlRaceSpeed.hh"

#include "game/kart/KartMove.hh"
#include "game/kart/KartObjectManager.hh"
#include "game/kart/KartState.hh"
#include "game/kart/VehiclePhysics.hh"
#include "game/system/SaveManager.hh"

extern "C" {
#include <revolution.h>
}

namespace UI {

namespace GroupId {

enum {
    Int0 = 0,
    Int1 = 1,
    Int2 = 2,
    Fract0 = 3,
    Fract1 = 4,
};

} // namespace GroupId

CtrlRaceSpeed::CtrlRaceSpeed() = default;

CtrlRaceSpeed::~CtrlRaceSpeed() = default;

void CtrlRaceSpeed::initSelf() {
    CtrlRaceBase::initSelf();

    const char *panes[] = {
            "speed_00",
            "speed_01",
            "speed_02",
            "speed_03",
            "speed_04",
            "coron",
            "speed_text",
    };
    for (u32 i = 0; i < std::size(panes); i++) {
        setPaneColor(panes[i], true);
    }
}

void CtrlRaceSpeed::calcSelf() {
    calcLabelVisibility("speed_text");

    process();

    s8 playerId = getPlayerId();
    if (playerId < 0) {
        return;
    }

    auto *saveManager = System::SaveManager::Instance();
    auto setting = saveManager->getSetting<SP::ClientSettings::Setting::Speedometer>();
    setVisible(setting != SP::ClientSettings::Speedometer::Off);

    auto *object = Kart::KartObjectManager::Instance()->object(playerId);
    if (object->getKartState()->inCannon()) {
        setting = SP::ClientSettings::Speedometer::XYZ;
    }
    f32 speed = 0.0f;
    switch (setting) {
    case SP::ClientSettings::Speedometer::InternalPlus:
    case SP::ClientSettings::Speedometer::Internal: {
        f32 internalSpeed = object->getInternalSpeed();
        speed = internalSpeed;
        if (setting == SP::ClientSettings::Speedometer::InternalPlus) {
            const auto *internalVelDir = object->getKartMove()->internalVelDir();
            const auto *movingRoadVel = object->getVehiclePhysics()->movingRoadVel();
            const auto *movingWaterVel = object->getVehiclePhysics()->movingWaterVel();
            speed += Vec3::Dot(*internalVelDir, *movingRoadVel);
            speed += Vec3::Dot(*internalVelDir, *movingWaterVel);
        }
        f32 hardSpeedLimit = object->getKartMove()->hardSpeedLimit();
        speed = std::min(speed, hardSpeedLimit);
        break;
    }
    case SP::ClientSettings::Speedometer::XYZ:
    case SP::ClientSettings::Speedometer::XZ:
    case SP::ClientSettings::Speedometer::Y: {
        const auto *pos = object->getPos();
        const auto *lastPos = object->getLastPos();
        Vec3 vel;
        if (setting == SP::ClientSettings::Speedometer::XYZ) {
            vel = {pos->x - lastPos->x, pos->y - lastPos->y, pos->z - lastPos->z};
        } else if (setting == SP::ClientSettings::Speedometer::XZ) {
            vel = {pos->x - lastPos->x, 0.0f, pos->z - lastPos->z};
        } else if (setting == SP::ClientSettings::Speedometer::Y) {
            vel = {0.0f, pos->y - lastPos->y, 0.0f};
        }
        speed = Vec3::Norm(vel);
        break;
    }
    case SP::ClientSettings::Speedometer::Off:
        break;
    }

    s32 integral = speed;
    u32 fractional = (speed >= 0.0f ? speed - integral : integral - speed) * 100.0f + 0.5f;
    if (integral > 999) {
        integral = 999;
        fractional = 99;
    } else if (integral < -99) {
        integral = -99;
        fractional = 99;
    }
    if (fractional > 99) {
        fractional = 99;
    }

    f32 int0;
    if (integral >= 100) {
        int0 = integral / 100;
    } else if (integral <= -10) {
        int0 = 10;
    } else {
        int0 = 11;
    }
    f32 int1;
    if (integral >= 10) {
        int1 = (integral / 10) % 10;
    } else if (integral <= -10) {
        int1 = -integral / 10;
    } else if (speed < 0.0f) {
        int1 = 10;
    } else {
        int1 = 11;
    }
    f32 int2;
    if (speed >= 0.0f) {
        int2 = integral % 10;
    } else {
        int2 = -integral % 10;
    }
    f32 fract0 = fractional / 10;
    f32 fract1 = fractional % 10;

    m_animator.setAnimationInactive(GroupId::Int0, 0, int0);
    m_animator.setAnimationInactive(GroupId::Int1, 0, int1);
    m_animator.setAnimationInactive(GroupId::Int2, 0, int2);
    m_animator.setAnimationInactive(GroupId::Fract0, 0, fract0);
    m_animator.setAnimationInactive(GroupId::Fract1, 0, fract1);
}

void CtrlRaceSpeed::load(u32 localPlayerCount, u32 localPlayerId) {
    m_localPlayerId = localPlayerId;
    u32 screenCount = localPlayerCount == 3 ? 4 : localPlayerCount;
    char variant[0x20];
    snprintf(variant, std::size(variant), "CtrlRaceSpeed_%u_%u", screenCount, localPlayerId);
    // clang-format off
    const char *groups[] = {
        "eAFInt0", "texture_pattern_0_9_0", nullptr,
        "eAFInt1", "texture_pattern_0_9_1", nullptr,
        "eAFInt2", "texture_pattern_0_9_2", nullptr,
        "eAFFract0", "texture_pattern_0_9_3", nullptr,
        "eAFFract1", "texture_pattern_0_9_4", nullptr,
        nullptr,
    };
    // clang-format on
    LayoutUIControl::load("game_image", "speed_number", variant, groups);
    for (u32 i = 0; i < 5; i++) {
        m_animator.setAnimationInactive(i, 0, 0.0f);
    }
}

} // namespace UI
