#include "RaceManager.hh"

extern "C" {
#include <game/system/RootScene.h>
}
#include <game/ui/SectionManager.hh>

#include "sp/cs/RaceClient.hh"

namespace SP {

RoomManager &RaceManager::roomManager() {
    return m_roomManager;
}

void RaceManager::OnCreateScene() {
    auto *sectionManager = UI::SectionManager::Instance();
    if (!sectionManager) {
        return;
    }

    if (UI::Section::HasRaceClient(sectionManager->nextSectionId())) {
        if (UI::Section::HasRaceClient(sectionManager->lastSectionId())) {
            return;
        }
    } else {
        return;
    }

    assert(!s_block);
    auto *heap = reinterpret_cast<EGG::Heap *>(s_rootScene->heapCollection.heaps[HEAP_ID_MEM2]);
    s_block = heap->alloc(sizeof(RaceClient), 0x4);
    RaceClient::CreateInstance();
}

void RaceManager::OnDestroyScene() {
    auto *sectionManager = UI::SectionManager::Instance();
    if (!sectionManager) {
        return;
    }

    if (UI::Section::HasRaceClient(sectionManager->lastSectionId())) {
        if (UI::Section::HasRaceClient(sectionManager->nextSectionId())) {
            return;
        }

        if (RaceClient::Instance()) {
            RaceClient::DestroyInstance();
        }
    } else {
        return;
    }

    assert(s_block);
    auto *heap = reinterpret_cast<EGG::Heap *>(s_rootScene->heapCollection.heaps[HEAP_ID_MEM2]);
    heap->free(s_block);
    s_block = nullptr;
}

RaceManager *RaceManager::Instance() {
    return s_instance;
}

RaceManager::RaceManager() : m_roomManager(*RoomManager::Instance()) {
    m_playerCount = m_roomManager.playerCount();
    for (u32 i = 0; i < m_playerCount; i++) {
        m_players[i] = {m_roomManager.player(i).m_clientId};
    }
}

RaceManager::~RaceManager() = default;

bool RaceManager::IsInputStateValid(const InputState &inputState) {
    if (inputState.stickX > 14) {
        return false;
    }

    if (inputState.stickY > 14) {
        return false;
    }

    if (inputState.trick > 4) {
        return false;
    }

    return true;
}

void *RaceManager::s_block = nullptr;
RaceManager *RaceManager::s_instance = nullptr;

} // namespace SP
