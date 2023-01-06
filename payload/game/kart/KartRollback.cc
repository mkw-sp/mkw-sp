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
    if (auto serverFrame = SP::RaceClient::Instance()->frame()) {
        u32 frameId = System::RaceManager::Instance()->frameId();
        s32 delay = static_cast<s32>(frameId) - static_cast<s32>(serverFrame->id);
        u32 playerId = getPlayerId();
        auto &player = serverFrame->players[playerId];
        auto *vehiclePhysics = getVehiclePhysics();
        auto *kartCollide = getKartCollide();
        auto *kartMove = getKartMove();
        auto *kartState = getKartState();
        if (delay <= 0) {
            while (m_frames.front() && m_frames.front()->id < frameId) {
                m_frames.pop_front();
            }
            if (!m_frames.full()) {
                s16 timeBeforeRespawn = serverFrame->players[playerId].timeBeforeRespawn;
                s16 timeInRespawn = serverFrame->players[playerId].timeInRespawn;
                Vec3 pos(serverFrame->players[playerId].pos);
                Quat mainRot(serverFrame->players[playerId].mainRot);
                f32 internalSpeed = serverFrame->players[playerId].internalSpeed;
                m_frames.push_back({serverFrame->id, timeBeforeRespawn, timeInRespawn, pos, mainRot,
                        internalSpeed});
            }
        } else {
            while (m_frames.front() && m_frames.front()->id < serverFrame->id) {
                m_frames.pop_front();
            }
            auto *rollbackFrame = m_frames.front();
            if (rollbackFrame && rollbackFrame->id == serverFrame->id) {
                s16 timeBeforeRespawn = player.timeBeforeRespawn;
                s16 timeInRespawn = player.timeInRespawn;
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
                }
                if (!!rollbackFrame->timeBeforeRespawn == !!player.timeBeforeRespawn &&
                        !!rollbackFrame->timeInRespawn == !!player.timeInRespawn) {
                    auto posDelta = rollbackFrame->pos - Vec3(serverFrame->players[playerId].pos);
                    Quat tmp(serverFrame->players[playerId].mainRot);
                    Quat inverse;
                    Quat::Inverse(rollbackFrame->mainRot, inverse);
                    Quat mainRotDelta = tmp * inverse;
                    f32 internalSpeedDelta = rollbackFrame->internalSpeed -
                            serverFrame->players[playerId].internalSpeed;
                    for (u32 i = 0; i < m_frames.count(); i++) {
                        m_frames[i]->pos -= posDelta;
                        m_frames[i]->mainRot = mainRotDelta * m_frames[i]->mainRot;
                        m_frames[i]->internalSpeed -= internalSpeedDelta;
                    }
                }
            }
        }
        for (u32 i = 0; i < m_frames.count(); i++) {
            if (m_frames[i]->id == frameId - 1) {
                if (m_frames[i]->timeBeforeRespawn) {
                    if (kartCollide->m_timeBeforeRespawn) {
                        kartCollide->m_timeBeforeRespawn = m_frames[i]->timeBeforeRespawn;
                    } else {
                        // TODO implement?
                    }
                } else if (m_frames[i]->timeInRespawn) {
                    if (kartMove->m_timeInRespawn) {
                        kartMove->m_timeInRespawn = m_frames[i]->timeInRespawn;
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
                f32 t = 0.25f;
                Vec3 posDelta = m_frames[i]->pos - vehiclePhysics->m_pos;
                Vec3 proj;
                Vec3::ProjUnit(posDelta, getKartMove()->m_up, proj);
                f32 norm = Vec3::Norm(proj);
                if (norm < 300.0f) {
                    posDelta -= proj;
                }
                m_posDelta = (1.0f - t) * m_posDelta + t * posDelta;
                Quat inverse;
                Quat::Inverse(vehiclePhysics->m_mainRot, inverse);
                Quat mainRotDelta = m_frames[i]->mainRot * inverse;
                Quat::Slerp(m_mainRotDelta, mainRotDelta, m_mainRotDelta, t);
                f32 internalSpeedDelta = m_frames[i]->internalSpeed - kartMove->m_internalSpeed;
                m_internalSpeedDelta = (1.0f - t) * m_internalSpeedDelta + t * internalSpeedDelta;
                break;
            }
        }
        vehiclePhysics->m_pos += m_posDelta;
        kartCollide->m_movement += m_posDelta;
        vehiclePhysics->m_mainRot = m_mainRotDelta * vehiclePhysics->m_mainRot;
        kartMove->m_internalSpeed += m_internalSpeedDelta;
        kartMove->m_internalSpeed = std::clamp(kartMove->m_internalSpeed, -20.0f, 120.0f);
    }
}

void KartRollback::calcLate() {
    u32 frameId = System::RaceManager::Instance()->frameId();
    if (!m_frames.back() || m_frames.back()->id < frameId) {
        if (m_frames.full()) {
            m_frames.pop_front();
        }
        m_frames.push_back(Frame{frameId, getTimeBeforeRespawn(),
                static_cast<s16>(getTimeInRespawn()), *getPos(), *getMainRot(),
                getInternalSpeed()});
    }
}

} // namespace Kart
