#include "CourseSelectButton.hh"

#include "game/ui/SectionManager.hh"

#include <game/system/RaceConfig.hh>

#include <cstdio>

namespace UI {

CourseSelectButton::CourseSelectButton() = default;

CourseSelectButton::~CourseSelectButton() = default;

void CourseSelectButton::calcSelf() {
    auto *section = SectionManager::Instance()->currentSection();
    auto *pane = m_mainLayout.findPaneByName("picture_base");
    assert(pane);
    auto *material = pane->getMaterial();
    assert(material);
    assert(material->getTexSRTNum() == 1);
    auto *texSRTs = material->getTexSRTAry();
    assert(texSRTs);
    texSRTs[0].scale.x = section->locationAdjustScale().y;
    texSRTs[0].scale.y = section->locationAdjustScale().x;
}

void CourseSelectButton::load(u32 i) {
    char variant[0x20];
    snprintf(variant, std::size(variant), "Button%zu", i);
    PushButton::load("button", "CourseSelectButton", variant, 0x1, false, false);
    m_index = i;

    const char *paneNames[4] = {"text_shadow", "text", "text_light_01", "text_light_02"};
    assert(std::size(paneNames) == std::size(m_panes));
    assert(std::size(paneNames) == std::size(m_sizes));
    for (size_t i = 0; i < std::size(paneNames); i++) {
        m_panes[i] = m_mainLayout.findPaneByName(paneNames[i]);
        assert(m_panes[i]);
        m_sizes[i].x = m_panes[i]->m_width;
        m_sizes[i].y = m_panes[i]->m_height;
    }
}

void CourseSelectButton::refresh(u32 wiimmId) {
    auto *section = SectionManager::Instance()->currentSection();
    for (size_t i = 0; i < std::size(m_panes); i++) {
        m_panes[i]->m_width = m_sizes[i].x / section->locationAdjustScale().x;
        m_panes[i]->m_height = m_sizes[i].y / section->locationAdjustScale().y;
    }

    auto *raceConfig = System::RaceConfig::Instance();
    auto &trackPackManager = SP::TrackPackManager::Instance();
    auto &track = trackPackManager.getTrack(wiimmId);

    raceConfig->m_packInfo.setTrackMessage(this, track.name.c_str(), track.getCourseId());
}

void CourseSelectButton::setTex(u8 c, const GXTexObj &texObj) {
    auto *pane = m_mainLayout.findPaneByName("picture_base");
    assert(pane);
    auto *material = pane->getMaterial();
    assert(material);
    assert(c < material->getTextureNum());
    auto *texMaps = material->getTexMapAry();
    assert(texMaps);
    texMaps[c] = nw4r::lyt::TexMap(texObj);
}

} // namespace UI
