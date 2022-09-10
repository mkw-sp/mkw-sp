#pragma once

#include "game/ui/Page.hh"

namespace UI {

class ModelRenderPage : public Page {
public:
    // ModelRenderPage();

    void REPLACE onInit() override;

    static u8 REPLACED(determineModelCount)(SectionId sectionId);
    static REPLACE u8 determineModelCount(SectionId sectionId);

    static void onDriverModelLoaded(s8 idx);

private:
    struct Something {
        u32 _00;
        u32 _04;
        u8 _08;
        u8 _09;
    };

    MenuInputManager m_inputManager;
    Something _54[4];
    u8 _82[0x84 - 0x82];
    u8 m_modelCount;
    u32 _88;
    u8 _8c[0x90 - 0x8c];
    u8 _90;
    u8 _91[0x1a14 - 0x91];
};
static_assert(sizeof(ModelRenderPage) == 0x1a14);

} // namespace UI
