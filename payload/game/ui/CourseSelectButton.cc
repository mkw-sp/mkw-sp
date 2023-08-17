#include "CourseSelectButton.hh"

#include "game/ui/SectionManager.hh"

#include <cstdio>

namespace UI {

CourseSelectButton::CourseSelectButton() = default;

CourseSelectButton::~CourseSelectButton() = default;

void CourseSelectButton::load(u32 i) {
    char variant[0x20];
    if (SectionManager::Instance()->currentSection()->id() == SectionId::ServicePackRankings) {
        snprintf(variant, std::size(variant), "RankingButton%zu", i);
    } else {
        snprintf(variant, std::size(variant), "Button%zu", i);
    }
    PushButton::load("button", "CourseSelectButton", variant, 0x1, false, false);
    m_index = i;

    auto *section = SectionManager::Instance()->currentSection();
    const char *paneNames[4] = {"text_shadow", "text", "text_light_01", "text_light_02"};
    for (size_t i = 0; i < std::size(paneNames); i++) {
        auto *pane = m_mainLayout.findPaneByName(paneNames[i]);
        assert(pane);
        pane->m_width /= section->scaleFor().x;
    }

    auto *pane = m_mainLayout.findPaneByName("picture_base");
    assert(pane);
    auto *material = pane->getMaterial();
    assert(material);
    assert(material->getTexSRTNum() == 1);
    auto *texSRTs = material->getTexSRTAry();
    assert(texSRTs);
    texSRTs[0].scale.y = section->scaleFor().x;
}

void CourseSelectButton::refresh(u32 messageId) {
    setMessageAll(messageId);
}

void CourseSelectButton::refresh(u8 c, const GXTexObj &texObj) {
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
