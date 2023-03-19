#include "AwaitPage.hh"

namespace UI {

void MenuAwaitPage::setReplacement(PageId replacement) {
    m_replacement = replacement;
}

SPAwaitPage::SPAwaitPage() = default;
SPAwaitPage::~SPAwaitPage() = default;

// All other AwaitPages will reset their popped flag when they are activated,
// this avoids a race between the pop and the activate calls which
// could lead to a softlock
void SPAwaitPage::onActivate() {}

} // namespace UI
