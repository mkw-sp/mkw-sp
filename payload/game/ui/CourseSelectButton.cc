#include "CourseSelectButton.hh"

#include "game/ui/SectionManager.hh"

#include <sp/TrackPackManager.hh>

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

void CourseSelectButton::refresh(u32 courseId) {
    auto *section = SectionManager::Instance()->currentSection();
    for (size_t i = 0; i < std::size(m_panes); i++) {
        m_panes[i]->m_width = m_sizes[i].x / section->locationAdjustScale().x;
        m_panes[i]->m_height = m_sizes[i].y / section->locationAdjustScale().y;
    }

    // TODO(GnomedDev): Handle showing track names via wiimm's DB.
    auto *trackPackManager = SP::TrackPackManager::Instance();
    if (trackPackManager->isVanilla()) {
        auto vanillaPack = trackPackManager->getSelectedTrackPack();
        auto slotId = vanillaPack->getSlotId(courseId);
        setMessageAll(9360 + slotId);
    } else {
        setMessageAll(10394);
    }
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
