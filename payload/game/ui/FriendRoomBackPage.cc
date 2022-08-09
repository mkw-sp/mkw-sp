#include "FriendRoomBackPage.hh"

#include "game/ui/GlobePage.hh"
#include "game/ui/SectionManager.hh"

#include <numeric>

extern "C" {
#include <revolution.h>
}

namespace UI {

FriendRoomBackPage::FriendRoomBackPage() = default;

FriendRoomBackPage::~FriendRoomBackPage() = default;

void FriendRoomBackPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);

    initChildren(1 + std::size(m_players));
    insertChild(0, &m_pageTitleText, 0);
    for (size_t i = 0; i < std::size(m_players); i++) {
        insertChild(1 + i, &m_players[i], 0);
    }

    m_pageTitleText.load(false);
    for (size_t i = 0; i < std::size(m_players); i++) {
        m_players[i].load(&m_miiGroup, 0, i);
    }

    m_miiGroup.init(12, 0x4, nullptr);
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    if (sectionId == SectionId::OnlineServer) {
        m_pageTitleText.setMessage(20010);
    } else if (sectionId == SectionId::OnlineSingle) {
        m_pageTitleText.setMessage(20011);
    } else {
        m_pageTitleText.setMessage(20012);
    }
}

void FriendRoomBackPage::onActivate() {
    m_playerCount = 0;
    std::iota(std::begin(m_indices), std::end(m_indices), 0);
    m_globePlayerId.reset();
    m_timer = 0;

    while (auto *entry = m_queue.front()) {
        if (const auto *join = std::get_if<Join>(entry)) {
            m_miiGroup.insertFromRaw(m_indices[m_playerCount], &join->mii);
            m_playerCount++;
        } else {
            assert(!"Unexpected variant!");
        }
        m_queue.pop();
    }
}

void FriendRoomBackPage::beforeInAnim() {
    for (size_t i = 0; i < m_playerCount; i++) {
        m_players[m_indices[i]].show(i, m_playerCount);
    }
}

void FriendRoomBackPage::afterCalc() {
    if (m_timer > 0) {
        m_timer--;
        return;
    }

    auto *entry = m_queue.front();
    if (!entry) {
        return;
    }

    Section *section = SectionManager::Instance()->currentSection();
    auto *globePage = section->page<PageId::Globe>();
    if (const auto *join = std::get_if<Join>(entry)) {
        assert(m_playerCount < std::size(m_players));
        m_miiGroup.insertFromRaw(m_indices[m_playerCount], &join->mii);
        m_players[m_indices[m_playerCount]].show(m_playerCount, m_playerCount + 1);
        for (size_t i = 0; i < m_playerCount; i++) {
            m_players[m_indices[i]].move(i, m_playerCount + 1);
        }
        auto *mii = m_miiGroup.get(m_indices[m_playerCount]);
        u32 location = join->location;
        u16 latitude = join->latitude;
        u16 longitude = join->longitude;
        auto &callback = m_players[m_indices[m_playerCount]].callback(); 
        globePage->requestComment(mii, latitude, longitude, location, 4499, 0, nullptr, callback);
        m_globePlayerId = m_playerCount;
        m_playerCount++;
        m_timer = 90;
    } else if (const auto *leave = std::get_if<Leave>(entry)) {
        assert(m_playerCount > 0);
        m_playerCount--;
        if (m_globePlayerId) {
            if (*m_globePlayerId == leave->playerId) {
                m_globePlayerId.reset();
                globePage->requestSpinFar();
            } else if (*m_globePlayerId > leave->playerId) {
                (*m_globePlayerId)--;
            }
        }
        u32 index = m_indices[leave->playerId];
        for (size_t i = leave->playerId; i < m_playerCount; i++) {
            m_indices[i] = m_indices[i + 1];
        }
        m_indices[m_playerCount] = index;
        for (size_t i = 0; i < m_playerCount; i++) {
            m_players[m_indices[i]].move(i, m_playerCount);
        }
        m_players[m_indices[m_playerCount]].hide();
    }
    m_queue.pop();
}

void FriendRoomBackPage::pop() {
    m_queue.reset();
    startReplace(Anim::Prev, 0.0f);
}

void FriendRoomBackPage::onPlayerJoin(System::RawMii mii, u32 location, u16 latitude,
        u16 longitude) {
    assert(!m_queue.full());
    m_queue.push(Join { mii, location, latitude, longitude });
}

void FriendRoomBackPage::onPlayerLeave(u32 playerId) {
    assert(!m_queue.full());
    // TODO optimize
    m_queue.push(Leave { playerId });
}

} // namespace UI
