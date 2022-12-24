#include "KartRollback.hh"

#include "game/kart/VehiclePhysics.hh"
#include "game/system/RaceManager.hh"

#include <sp/cs/RaceClient.hh>

namespace Kart {

KartRollback::KartRollback() = default;

void KartRollback::calcEarly() {
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
                vehiclePhysics->m_pos = m_frames[i]->pos;
                vehiclePhysics->m_mainRot = m_frames[i]->mainRot;
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
