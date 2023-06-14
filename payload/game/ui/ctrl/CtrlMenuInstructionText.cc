#include "CtrlMenuInstructionText.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

CtrlMenuInstructionText::CtrlMenuInstructionText() = default;

CtrlMenuInstructionText::~CtrlMenuInstructionText() = default;

void CtrlMenuInstructionText::load() {
    auto sectionId = SectionManager::Instance()->currentSection()->id();
    switch (sectionId) {
    case SectionId::Unlock0:
    case SectionId::Unlock1:
    case SectionId::Unlock2:
    case SectionId::Unlock3:
        LayoutUIControl::load("common", "MenuObiInstructionText", "MenuObiInstructionText",
                nullptr);
        break;
    default:
        if (Section::GetSceneId(sectionId) == System::SceneId::Race) {
            LayoutUIControl::load("bg", "RaceObiInstructionText", "RaceObiInstructionText",
                    nullptr);
        } else {
            LayoutUIControl::load("bg", "MenuObiInstructionText", "MenuObiInstructionText",
                    nullptr);
        }
    }
}

} // namespace UI
