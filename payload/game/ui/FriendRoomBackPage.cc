#include "FriendRoomBackPage.hh"

#include "game/ui/FriendRoomPage.hh"
#include "game/ui/FriendRoomRulesPage.hh"
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

    Section *section = SectionManager::Instance()->currentSection();
    while (auto *entry = m_queue.front()) {
        if (const auto *join = std::get_if<Join>(entry)) {
            m_miiGroup.insertFromRaw(m_indices[m_playerCount], &join->mii);
            m_locations[m_playerCount] = join->location;
            m_latitudes[m_playerCount] = join->latitude;
            m_longitudes[m_playerCount] = join->longitude;
            m_playerCount++;
        } else if (const auto *settings = std::get_if<Settings>(entry)) {
            auto *friendRoomRulesPage = section->page<PageId::FriendRoomRules>();
            friendRoomRulesPage->refresh(settings->settings);
        } else {
            assert(!"Unexpected variant!");
        }
        m_queue.pop();
    }

    push(PageId::FriendRoom, Anim::Next);
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
        m_locations[m_indices[m_playerCount]] = location;
        u16 latitude = join->latitude;
        m_latitudes[m_indices[m_playerCount]] = latitude;
        u16 longitude = join->longitude;
        m_longitudes[m_indices[m_playerCount]] = longitude;
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
    } else if (const auto *comment = std::get_if<Comment>(entry)) {
        auto *mii = m_miiGroup.get(m_indices[comment->playerId]); 
        u32 location = m_locations[m_indices[comment->playerId]];
        u16 latitude = m_latitudes[m_indices[comment->playerId]];
        u16 longitude = m_longitudes[m_indices[comment->playerId]];
        u32 messageId = comment->messageId + 4500;
        auto &callback = m_players[m_indices[comment->playerId]].callback();
        globePage->requestComment(mii, latitude, longitude, location, messageId, 0, nullptr,
                callback);
        m_globePlayerId = m_playerCount;
    } else if (const auto *settings = std::get_if<Settings>(entry)) {
        assert(m_playerCount > 0);
        auto *friendRoomRulesPage = section->page<PageId::FriendRoomRules>();
        friendRoomRulesPage->refresh(settings->settings);
        auto *mii = m_miiGroup.get(m_indices[0]);
        u32 location = m_locations[m_indices[0]];
        u16 latitude = m_latitudes[m_indices[0]];
        u16 longitude = m_longitudes[m_indices[0]];
        auto &callback = m_players[m_indices[0]].callback(); 
        globePage->requestComment(mii, latitude, longitude, location, 20025, 2, nullptr, callback);
        m_globePlayerId = 0;
        m_timer = 90;
    }
    m_queue.pop();
}

void FriendRoomBackPage::onRefocus() {
    m_queue.reset();
    startReplace(Anim::Prev, 0.0f);
}

void FriendRoomBackPage::pop() {
    m_queue.reset();
    startReplace(Anim::Prev, 0.0f);

    auto *section = SectionManager::Instance()->currentSection();
    if (section->isPageActive(PageId::FriendRoom)) {
        auto friendRoomPage = section->page<PageId::FriendRoom>();
        friendRoomPage->pop();
    }
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

void FriendRoomBackPage::onReceiveComment(u32 playerId, u32 messageId) {
    assert(!m_queue.full());
    m_queue.push(Comment { playerId, messageId });
}

void FriendRoomBackPage::onSettingsChange(
        const std::array<u32, SP::RoomSettings::count> &settings) {
    assert(!m_queue.full());
    // TODO optimize
    m_queue.push(Settings { settings });
}

} // namespace UI
