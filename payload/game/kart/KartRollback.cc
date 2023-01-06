#include "KartRollback.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/KartState.hh"
#include "game/kart/VehiclePhysics.hh"
#include "game/system/RaceManager.hh"

#include <sp/cs/RaceClient.hh>

#include <algorithm>

namespace Kart {

KartRollback::KartRollback() = default;

Vec3 KartRollback::posDelta() const {
    return m_posDelta;
}

Quat KartRollback::mainRotDelta() const {
    return m_mainRotDelta;
}

f32 KartRollback::internalSpeedDelta() const {
    return m_internalSpeedDelta;
}

void KartRollback::calcEarly() {
    u32 playerId = getPlayerId();
    if (auto frame = serverFrame(playerId)) {
        u32 time = System::RaceManager::Instance()->time();
        s32 delay = static_cast<s32>(time) - static_cast<s32>(frame->time);
        if (delay <= 0) {
            handleFutureFrame(*frame);
        } else {
            handlePastFrame(*frame);
        }
        for (u32 i = 0; i < m_frames.count(); i++) {
            if (m_frames[i]->time == time - 1) {
                applyFrame(*m_frames[i]);
                break;
            }
        }
        auto *vehiclePhysics = getVehiclePhysics();
        auto *kartCollide = getKartCollide();
        auto *kartMove = getKartMove();
        vehiclePhysics->m_pos += m_posDelta;
        kartCollide->m_movement += m_posDelta;
        vehiclePhysics->m_mainRot = m_mainRotDelta * vehiclePhysics->m_mainRot;
        kartMove->m_internalSpeed += m_internalSpeedDelta;
        kartMove->m_internalSpeed = std::clamp(kartMove->m_internalSpeed, -20.0f, 120.0f);
    }
}

void KartRollback::calcLate() {
    u32 time = System::RaceManager::Instance()->time();
    if (!m_frames.back() || m_frames.back()->time < time) {
        if (m_frames.full()) {
            m_frames.pop_front();
        }
        s16 timeBeforeRespawn = getTimeBeforeRespawn();
        s16 timeInRespawn = getTimeInRespawn();
        std::array<s16, 3> timesBeforeBoostEnd;
        for (u32 i = 0; i < 3; i++) {
            timesBeforeBoostEnd[i] = getTimeBeforeBoostEnd(i * 2);
        }
        Vec3 pos = *getPos();
        Quat mainRot = *getMainRot();
        f32 internalSpeed = getInternalSpeed();
        Frame frame{time, timeBeforeRespawn, timeInRespawn, timesBeforeBoostEnd, pos, mainRot,
                internalSpeed};
        m_frames.push_back(std::move(frame));
    }
}

std::optional<KartRollback::Frame> KartRollback::serverFrame(u32 playerId) const {
    auto serverFrame = SP::RaceClient::Instance()->frame();
    if (!serverFrame) {
        return {};
    }

    u32 time = serverFrame->time;
    const auto &player = serverFrame->players[playerId];
    s16 timeBeforeRespawn = player.timeBeforeRespawn;
    s16 timeInRespawn = player.timeInRespawn;
    std::array<s16, 3> timesBeforeBoostEnd;
    for (u32 i = 0; i < 3; i++) {
        timesBeforeBoostEnd[i] = player.timesBeforeBoostEnd[i];
    }
    Vec3 pos(player.pos);
    Quat mainRot(player.mainRot);
    f32 internalSpeed = player.internalSpeed;
    return {{time, timeBeforeRespawn, timeInRespawn, timesBeforeBoostEnd, pos, mainRot,
            internalSpeed}};
}

void KartRollback::handleFutureFrame(const Frame &frame) {
    u32 time = System::RaceManager::Instance()->time();
    while (m_frames.front() && m_frames.front()->time < time) {
        m_frames.pop_front();
    }
    if (!m_frames.full()) {
        m_frames.push_back(std::move(frame));
    }
}

void KartRollback::handlePastFrame(const Frame &frame) {
    while (m_frames.front() && m_frames.front()->time < frame.time) {
        m_frames.pop_front();
    }
    auto *rollbackFrame = m_frames.front();
    if (rollbackFrame && rollbackFrame->time == frame.time) {
        s16 timeBeforeRespawn = frame.timeBeforeRespawn;
        s16 timeInRespawn = frame.timeInRespawn;
        auto timesBeforeBoostEnd = frame.timesBeforeBoostEnd;
        for (u32 i = 0; i < m_frames.count(); i++) {
            if (timeBeforeRespawn) {
                if (m_frames[i]->timeInRespawn) {
                    m_frames[i]->timeInRespawn = 1;
                } else {
                    m_frames[i]->timeBeforeRespawn = timeBeforeRespawn;
                }
                timeBeforeRespawn--;
                if (!timeBeforeRespawn) {
                    timeInRespawn = 1;
                }
            } else if (timeInRespawn) {
                if (m_frames[i]->timeBeforeRespawn) {
                    m_frames[i]->timeBeforeRespawn = 1;
                } else {
                    m_frames[i]->timeInRespawn = timeInRespawn;
                }
                timeInRespawn++;
                if (timeInRespawn > 110) {
                    timeInRespawn = 0;
                }
            } else {
                m_frames[i]->timeBeforeRespawn = 0;
                m_frames[i]->timeInRespawn = 0;
            }
            for (u32 j = 0; j < 3; j++) {
                m_frames[i]->timesBeforeBoostEnd[j] = timesBeforeBoostEnd[j];
                if (timesBeforeBoostEnd[j]) {
                    timesBeforeBoostEnd[j]--;
                }
            }
        }
        if (!!rollbackFrame->timeBeforeRespawn == !!frame.timeBeforeRespawn &&
                !!rollbackFrame->timeInRespawn == !!frame.timeInRespawn) {
            auto posDelta = rollbackFrame->pos - frame.pos;
            Quat inverse;
            Quat::Inverse(rollbackFrame->mainRot, inverse);
            Quat mainRotDelta = frame.mainRot * inverse;
            f32 internalSpeedDelta = rollbackFrame->internalSpeed - frame.internalSpeed;
            for (u32 i = 0; i < m_frames.count(); i++) {
                m_frames[i]->pos -= posDelta;
                m_frames[i]->mainRot = mainRotDelta * m_frames[i]->mainRot;
                m_frames[i]->internalSpeed -= internalSpeedDelta;
            }
        }
    }
}

void KartRollback::applyFrame(const Frame &frame) {
    auto *vehiclePhysics = getVehiclePhysics();
    auto *kartCollide = getKartCollide();
    auto *kartMove = getKartMove();
    auto *kartState = getKartState();
    if (frame.timeBeforeRespawn) {
        if (kartCollide->m_timeBeforeRespawn) {
            kartCollide->m_timeBeforeRespawn = frame.timeBeforeRespawn;
        } else {
            // TODO implement?
        }
    } else if (frame.timeInRespawn) {
        if (kartMove->m_timeInRespawn) {
            kartMove->m_timeInRespawn = frame.timeInRespawn;
        } else {
            // TODO implement?
        }
    } else {
        if (kartCollide->m_timeBeforeRespawn) {
            kartState->m_beforeRespawn = false;
            kartCollide->m_timeBeforeRespawn = 0;
            // TODO WipeControl
        } else if (kartMove->m_timeInRespawn) {
            kartMove->m_timeInRespawn = 140;
        }
    }
    for (u32 i = 0; i < 3; i++) {
        if (frame.timesBeforeBoostEnd[i]) {
            if (kartMove->m_boost.m_timesBeforeEnd[i * 2]) {
                kartMove->m_boost.m_timesBeforeEnd[i * 2] = frame.timesBeforeBoostEnd[i];
            } else {
                kartMove->activateBoost(i * 2, frame.timesBeforeBoostEnd[i]);
            }
        } else {
            kartMove->m_boost.m_timesBeforeEnd[i * 2] = 0;
            kartMove->m_boost.m_types &= ~(1 << (i * 2));
        }
    }
    f32 t = 0.25f;
    Vec3 posDelta = frame.pos - vehiclePhysics->m_pos;
    Vec3 proj;
    Vec3::ProjUnit(posDelta, getKartMove()->m_up, proj);
    f32 norm = Vec3::Norm(proj);
    if (norm < 300.0f) {
        posDelta -= proj;
    }
    m_posDelta = (1.0f - t) * m_posDelta + t * posDelta;
    Quat inverse;
    Quat::Inverse(vehiclePhysics->m_mainRot, inverse);
    Quat mainRotDelta = frame.mainRot * inverse;
    Quat::Slerp(m_mainRotDelta, mainRotDelta, m_mainRotDelta, t);
    f32 internalSpeedDelta = frame.internalSpeed - kartMove->m_internalSpeed;
    m_internalSpeedDelta = (1.0f - t) * m_internalSpeedDelta + t * internalSpeedDelta;
}

} // namespace Kart
