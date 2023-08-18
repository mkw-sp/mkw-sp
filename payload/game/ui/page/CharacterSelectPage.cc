#include "CharacterSelectPage.hh"

#include "game/ui/ModelRenderPage.hh"
#include "game/ui/SectionManager.hh"
#include "game/ui/model/MenuModelManager.hh"

namespace UI {

void CharacterSelectPage::onBack(u32 localPlayerId) {
    auto *inputManager = reinterpret_cast<MultiControlInputManager *>(getInputManager());
    auto *sectionManager = SectionManager::Instance();
    auto *modelRenderPage = sectionManager->currentSection()->page<PageId::ModelRender>();

    if (!inputManager->isPerControl(localPlayerId) && !modelRenderPage->player_09(localPlayerId)) {
        trySetAnim(localPlayerId);

        if (!m_titleMessage.miis[localPlayerId]) {
            u32 characterId = static_cast<u32>(
                    sectionManager->globalContext()->m_localCharacterIds[localPlayerId]);
            u32 characterMessageId = Registry::GetCharacterMessageId(characterId, false);

            m_names[localPlayerId].setMessageAll(characterMessageId);
        }

        inputManager->setPerControl(localPlayerId, true);
        playSound(Sound::SoundId::SE_UI_PAGE_PREV, -1);
        return;
    }

    if (localPlayerId == 0) {
        goBack();
    }
}

void CharacterSelectPage::trySetAnim(u32 localPlayerId) {
    MenuModelManager *menuModelManager = MenuModelManager::Instance();
    if (!menuModelManager) {
        return;
    }

    DriverModelManager *driverModelManager = menuModelManager->driverModelManager();
    if (!driverModelManager) {
        return;
    }

    DriverModel *driverModel = driverModelManager->handle(localPlayerId)->model;
    if (!driverModel) {
        return;
    }

    driverModel->setAnim(localPlayerId, 0);
}

void CharacterSelectPage::goBack() {
    auto *sectionManager = SectionManager::Instance();
    SectionId sectionId = sectionManager->currentSection()->id();

    if (Registry::IsWiFiMenuSection(sectionId)) {
        if (m_wifiCountDownTimer && m_wifiCountDownTimer->isRunning()) {
            return;
        }

        sectionManager->globalContext()->m_selectPlayer[0].m_characterId = -1;
        sectionManager->globalContext()->m_selectPlayer[1].m_characterId = -1;

        m_replacement = PageId::None;
        Page::startReplace(Anim::Prev, 0.0f);
        return;
    }

    if (Registry::IsGhostRaceSection(sectionId)) {
        m_replacement = PageId::None;
        Page::startReplace(Anim::Prev, 0.0f);
        return;
    }

    if (Registry::IsCompetitionSection(sectionId)) {
        m_replacement = PageId::None;
        Page::startReplace(Anim::Prev, 0.0f);
        return;
    }

    switch (sectionId) {
    case SectionId::SingleChannelLeaderboardChallenge:
        changeSection(SectionId::ServicePackRankings, Anim::Prev, 0.0f);
        break;
    case SectionId::SingleGhostListChallenge:
        changeSection(SectionId::Unknown82, Anim::Prev, 0.0f);
        break;
    default:
        startReplacePreviousPage(0.0f);
        break;
    }
}

} // namespace UI
