#include "DriftSelectPage.hh"

#include "game/ui/SectionManager.hh"
#include <sp/cs/RoomClient.hh>

namespace UI {

void DriftSelectPage::onButtonFront(PushButton *button) {
    auto context = SectionManager::Instance()->globalContext();
    context->m_driftIsAuto[0] = button->m_index == 1;
    REPLACED(onButtonFront)(button);
}

} // namespace UI
