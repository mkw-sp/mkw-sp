#include "LicenseDisplayPage.hh"

#include "game/system/SaveManager.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

void LicenseDisplayPage::onActivate() {
    if (anim() != Anim::Next) {
        return;
    }

    auto *saveManager = System::SaveManager::Instance();
    auto *sectionManager = SectionManager::Instance();
    auto *globalContext = sectionManager->globalContext();

    auto sectionId = sectionManager->currentSection()->id();
    if (sectionId == SectionId::MiiSelectCreate) {
        auto *mii = m_miiGroup.get(0);
        saveManager->createLicense(0, mii->id(), mii->name());
        saveManager->selectLicense(0);
        globalContext->m_localPlayerMiis.insertFromId(0, mii->id());
        saveManager->createSPLicense(mii->id());
    } else if (sectionId == SectionId::MiiSelectChange || sectionId == SectionId::LicenseSettings) {
        auto *mii = m_miiGroup.get(0);
        saveManager->createLicense(0, mii->id(), mii->name());
        saveManager->selectLicense(0);
        globalContext->m_localPlayerMiis.insertFromId(0, mii->id());
        saveManager->setMiiId(*mii->id());
    }

    LicenseControl::Refresh(nullptr, &m_licenseControl, 0, &m_miiGroup, 0);
}

} // namespace UI
