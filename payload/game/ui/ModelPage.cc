#include "ModelPage.hh"

#include "SectionManager.hh"

namespace UI {

void ModelPage::SetModel(u32 model) {
    Section *section = SectionManager::Instance()->currentSection();
    auto *modelPage = section->page<PageId::Model>();
    modelPage->m_modelControl.setModel(model);
}

BackGroundModelControl &ModelPage::modelControl() {
    return m_modelControl;
}

} // namespace UI
