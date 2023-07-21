#include "WU8LibraryPage.hh"

#include "game/system/GameScene.hh"
#include "game/ui/AwaitPage.hh"
#include "game/ui/SectionManager.hh"

#include <sp/WU8Library.hh>

namespace UI {

void WU8LibraryPage::onInit() {
    m_inputManager.init(1, 0);
    setInputManager(&m_inputManager);

    auto *gameScene = System::GameScene::Instance();
    SP::WU8Library::StartExtraction(gameScene->volatileHeapCollection.mem2);
}

void WU8LibraryPage::onActivate() {
    auto *section = SectionManager::Instance()->currentSection();
    auto *awaitPage = section->page<PageId::SpinnerAwaitPage>();

    awaitPage->setTitleMessage(10440);
    awaitPage->setWindowMessage(10441);

    push(PageId::SpinnerAwaitPage, Anim::None);
}

void WU8LibraryPage::afterCalc() {
    auto *sectionManager = SectionManager::Instance();
    auto *section = sectionManager->currentSection();
    auto *awaitPage = section->page<PageId::SpinnerAwaitPage>();

    wchar_t fileWide[64];
    auto [extractionState, file] = SP::WU8Library::GetExtractionState();
    swprintf(fileWide, sizeof(fileWide), L"%s", file.data());

    MessageInfo info;
    info.strings[0] = fileWide;

    using enum SP::WU8Library::ExtractionState;
    switch (extractionState) {
    case Started:
        return;
    case Ripping:
        info.messageIds[0] = 10443;
        break;
    case Decompressing:
        info.messageIds[0] = 10444;
        break;
    case Processing:
        info.messageIds[0] = 10445;
        break;
    case Writing:
        info.messageIds[0] = 10446;
        break;
    case Finished:
        sectionManager->setNextSection(SectionId::TitleFromBoot, Anim::Next);
        sectionManager->startChangeSection(0, 0);
        return;
    case ReplacedCourse:
        awaitPage->setSpinnerVisible(false);
        awaitPage->setWindowMessage(10447);
        return;
    }

    awaitPage->setWindowMessage(10442, &info);
}

} // namespace UI
