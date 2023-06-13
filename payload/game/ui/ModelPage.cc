#include "ModelPage.hh"

#include "SectionManager.hh"

namespace UI {

BackGroundModelControl &ModelPage::modelControl() {
    return m_modelControl;
}

void ModelPage::SetModel(u32 model) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *modelPage = section->page<PageId::Model>();
    modelPage->m_modelControl.setModel(model);
}

} // namespace UI
