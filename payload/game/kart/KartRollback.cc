#include "KartRollback.hh"

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
                pos.x = serverFrame->players[playerId].posX;
                pos.y = serverFrame->players[playerId].posY;
                pos.z = serverFrame->players[playerId].posZ;
                m_frames.push({serverFrame->id, pos});
            }
        } else {
            while (m_frames.front() && m_frames.front()->id < serverFrame->id) {
                m_frames.pop();
            }
            auto *rollbackFrame = m_frames.front();
            if (rollbackFrame && rollbackFrame->id == serverFrame->id) {
                Vec3 delta;
                delta.x = rollbackFrame->pos.x - serverFrame->players[playerId].posX;
                delta.y = rollbackFrame->pos.y - serverFrame->players[playerId].posY;
                delta.z = rollbackFrame->pos.z - serverFrame->players[playerId].posZ;
                for (u32 i = 0; i < m_frames.count(); i++) {
                    m_frames[i]->pos.x -= delta.x;
                    m_frames[i]->pos.y -= delta.y;
                    m_frames[i]->pos.z -= delta.z;
                }
            }
        }
        for (u32 i = 0; i < m_frames.count(); i++) {
            if (m_frames[i]->id == frameId - 1) {
                setPos(&m_frames[i]->pos);
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
        m_frames.push(Frame{frameId, *getPos()});
    }
}

} // namespace Kart
