#pragma once

#include "game/ui/GlobalContext.hh"
#include "game/ui/SaveManagerProxy.hh"
#include "game/ui/Section.hh"

namespace UI {

class SectionManager {
public:
    Section *currentSection();
    SaveManagerProxy *saveManagerProxy();
    GlobalContext *globalContext();
    void setNextSection(SectionId sectionId, Page::Anim anim);
    void REPLACED(startChangeSection)(s32 delay, u32 color);
    REPLACE void startChangeSection(s32 delay, u32 color);

    static SectionManager *Instance();

private:
    Section *m_currentSection;
    u8 _04[0x90 - 0x04];
    SaveManagerProxy *m_saveManagerProxy;
    u8 _94[0x98 - 0x94];
    GlobalContext *m_globalContext;

    static SectionManager *s_instance;
};
static_assert(sizeof(SectionManager) == 0x9c);

} // namespace UI
