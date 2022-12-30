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
                Vec3 pos;
                pos.x = serverFrame->players[playerId].pos.x;
                pos.y = serverFrame->players[playerId].pos.y;
                pos.z = serverFrame->players[playerId].pos.z;
                Quat mainRot;
                mainRot.x = serverFrame->players[playerId].mainRot.x;
                mainRot.y = serverFrame->players[playerId].mainRot.y;
                mainRot.z = serverFrame->players[playerId].mainRot.z;
                mainRot.w = serverFrame->players[playerId].mainRot.w;
                m_frames.push({serverFrame->id, pos, mainRot});
            }
        } else {
            while (m_frames.front() && m_frames.front()->id < serverFrame->id) {
                m_frames.pop();
            }
            auto *rollbackFrame = m_frames.front();
            if (rollbackFrame && rollbackFrame->id == serverFrame->id) {
                Vec3 posDelta;
                posDelta.x = rollbackFrame->pos.x - serverFrame->players[playerId].pos.x;
                posDelta.y = rollbackFrame->pos.y - serverFrame->players[playerId].pos.y;
                posDelta.z = rollbackFrame->pos.z - serverFrame->players[playerId].pos.z;
                for (u32 i = 0; i < m_frames.count(); i++) {
                    m_frames[i]->pos.x -= posDelta.x;
                    m_frames[i]->pos.y -= posDelta.y;
                    m_frames[i]->pos.z -= posDelta.z;
                }
                Quat tmp;
                tmp.x = serverFrame->players[playerId].mainRot.x;
                tmp.y = serverFrame->players[playerId].mainRot.y;
                tmp.z = serverFrame->players[playerId].mainRot.z;
                tmp.w = serverFrame->players[playerId].mainRot.w;
                Quat inverse;
                PSQUATInverse(&rollbackFrame->mainRot, &inverse);
                Quat mainRotDelta;
                PSQUATMultiply(&tmp, &inverse, &mainRotDelta);
                for (u32 i = 0; i < m_frames.count(); i++) {
                    PSQUATMultiply(&mainRotDelta, &m_frames[i]->mainRot, &m_frames[i]->mainRot);
                }
            }
        }
        for (u32 i = 0; i < m_frames.count(); i++) {
            if (m_frames[i]->id == frameId - 1) {
                auto *vehiclePhysics = getVehiclePhysics();
                f32 t = 0.2f;
                m_posDelta.x = m_frames[i]->pos.x - vehiclePhysics->m_pos.x;
                m_posDelta.y = m_frames[i]->pos.y - vehiclePhysics->m_pos.y;
                m_posDelta.z = m_frames[i]->pos.z - vehiclePhysics->m_pos.z;
                Vec3 proj;
                Vec3::projUnit(proj, m_posDelta, getKartMove()->m_up);
                f32 norm = Vec3::norm(proj);
                if (norm < 300.0f) {
                    m_posDelta.x -= proj.x;
                    m_posDelta.y -= proj.y;
                    m_posDelta.z -= proj.z;
                }
                vehiclePhysics->m_pos.x += t * m_posDelta.x;
                vehiclePhysics->m_pos.y += t * m_posDelta.y;
                vehiclePhysics->m_pos.z += t * m_posDelta.z;
                C_QUATSlerp(&vehiclePhysics->m_mainRot, &m_frames[i]->mainRot,
                        &vehiclePhysics->m_mainRot, t);
                auto *kartCollide = getKartCollide();
                kartCollide->m_movement.x += t * m_posDelta.x;
                kartCollide->m_movement.y += t * m_posDelta.y;
                kartCollide->m_movement.z += t * m_posDelta.z;
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
