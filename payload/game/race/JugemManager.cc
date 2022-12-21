#include "JugemManager.hh"

#include "game/ui/SectionManager.hh"
#include "game/ui/page/RacePage.hh"

namespace Race {

void JugemManager::calc() {
    for (u32 i = 0; i < m_count; i++) {
        m_jugems[i]->m_visible = true;
        auto sectionId = UI::SectionManager::Instance()->currentSection()->id();
        if (sectionId == UI::SectionId::GhostReplay) {
            if (m_jugems[i]->getPlayerId() != UI::RacePage::Instance()->watchedPlayerId()) {
                m_jugems[i]->m_visible = false;
            }
        }
        if (sectionId == UI::SectionId::Thumbnails) {
            m_jugems[i]->m_visible = false;
        }

        m_jugems[i]->calc();
    }
}

void JugemManager::setVisible(u32 playerId, bool visible) {
    for (u32 i = 0; i < m_count; i++) {
        if (m_jugems[i]->getPlayerId() == playerId) {
            if (!m_jugems[i]->m_visible) {
                visible = false;
            }
            m_jugems[i]->setVisible(visible);
        }
    }
}

JugemManager *JugemManager::Instance() {
    return s_instance;
}

} // namespace Race
