#include "KartRollback.hh"

#include "game/kart/KartCollide.hh"
#include "game/kart/KartMove.hh"
#include "game/kart/VehiclePhysics.hh"
#include "game/system/RaceManager.hh"

#include <sp/cs/RaceClient.hh>

namespace Kart {

KartRollback::KartRollback() = default;

Vec3 KartRollback::posDelta() const {
    return m_posDelta;
}

Quat KartRollback::mainRotDelta() const {
    return m_mainRotDelta;
}

void KartRollback::calcEarly() {
    m_posDelta = {0.0f, 0.0f, 0.0f};
    m_mainRotDelta = {0.0f, 0.0f, 0.0f, 1.0f};

    if (auto serverFrame = SP::RaceClient::Instance()->frame()) {
        u32 frameId = System::RaceManager::Instance()->frameId();
        s32 delay = static_cast<s32>(frameId) - static_cast<s32>(serverFrame->id);
        u32 playerId = getPlayerId();
        if (delay <= 0) {
            while (m_frames.front() && m_frames.front()->id < frameId) {
                m_frames.pop();
            }
            if (!m_frames.full()) {
                Vec3 pos(serverFrame->players[playerId].pos);
                Quat mainRot(serverFrame->players[playerId].mainRot);
                m_frames.push({serverFrame->id, pos, mainRot});
            }
        } else {
            while (m_frames.front() && m_frames.front()->id < serverFrame->id) {
                m_frames.pop();
            }
            auto *rollbackFrame = m_frames.front();
            if (rollbackFrame && rollbackFrame->id == serverFrame->id) {
                auto posDelta = rollbackFrame->pos - Vec3(serverFrame->players[playerId].pos);
                for (u32 i = 0; i < m_frames.count(); i++) {
                    m_frames[i]->pos -= posDelta;
                }
                Quat tmp(serverFrame->players[playerId].mainRot);
                Quat inverse;
                Quat::Inverse(rollbackFrame->mainRot, inverse);
                Quat mainRotDelta = tmp * inverse;
                for (u32 i = 0; i < m_frames.count(); i++) {
                    m_frames[i]->mainRot = mainRotDelta * m_frames[i]->mainRot;
                }
            }
        }
        for (u32 i = 0; i < m_frames.count(); i++) {
            if (m_frames[i]->id == frameId - 1) {
                auto *vehiclePhysics = getVehiclePhysics();
                f32 t = 0.2f;
                m_posDelta = m_frames[i]->pos - vehiclePhysics->m_pos;
                Vec3 proj;
                Vec3::ProjUnit(m_posDelta, getKartMove()->m_up, proj);
                f32 norm = Vec3::Norm(proj);
                if (norm < 300.0f) {
                    m_posDelta -= proj;
                }
                vehiclePhysics->m_pos += t * m_posDelta;
                Quat::Slerp(vehiclePhysics->m_mainRot, m_frames[i]->mainRot,
                        vehiclePhysics->m_mainRot, t);
                auto *kartCollide = getKartCollide();
                kartCollide->m_movement += t * m_posDelta;
                break;
            }
        }
    }
}

void KartRollback::calcLate() {
    u32 frameId = System::RaceManager::Instance()->frameId();
    if (!m_frames.back() || m_frames.back()->id < frameId) {
        if (m_frames.full()) {
            m_frames.pop();
        }
        m_frames.push(Frame{frameId, *getPos(), *getMainRot()});
    }
}

} // namespace Kart
