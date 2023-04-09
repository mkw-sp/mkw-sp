#include "DemoPage.hh"

#include "game/kart/KartObjectManager.hh"
#include "game/system/RaceConfig.hh"
#include "game/ui/SectionManager.hh"

namespace UI {

void DemoPage::onInit() {
    m_inputManager.init(0, 0);
    setInputManager(&m_inputManager);

    auto sectionManager = SectionManager::Instance();
    auto globalContext = sectionManager->globalContext();
    auto raceConfig = System::RaceConfig::Instance();

    auto &menuScenario = raceConfig->menuScenario();
    auto &raceScenario = raceConfig->raceScenario();
    auto currentSectionId = sectionManager->currentSection()->id();

    initChildren(2);
    insertChild(0, &m_courseDisplay, 0);
    insertChild(1, &m_cupDisplay, 0);

    m_courseDisplay.load("demo", "course_name", "course_name", 0);
    m_cupDisplay.load("demo", "cup_name", "cup_name", 0);

    auto courseId = menuScenario.courseId;
    auto cupId = Registry::GetCourseCupId(courseId);

    m_courseDisplay.setMessageAll(Registry::GetCourseName(courseId), nullptr);

    u32 cupMsgId = 9999;
    MessageInfo cupInfo = {};
    const char *cupIcon = "icon_08_hatena";

    if (currentSectionId == SectionId::GPDemo || currentSectionId == SectionId::VSDemo) {
        if (raceScenario.mirror) {
            cupInfo.messageIds[0] = 1420;
        } else if (speedModIsEnabled) {
            cupInfo.messageIds[0] = 10072;
        } else {
            cupInfo.messageIds[0] = 1417 + static_cast<u32>(raceScenario.engineClass);
        }
    }

    if (currentSectionId == SectionId::GPDemo) {
        cupInfo.messageIds[1] = Registry::GetCupMessageId(cupId);
        cupMsgId = 1410 + raceScenario.raceNumber;
        cupIcon = Registry::GetCupIconName(cupId);
    } else if (currentSectionId == SectionId::VSDemo) {
        cupIcon = "icon_11_flag";
        globalContext->m_match += 1;

        if (raceScenario.teams) {
            cupMsgId = 1409;
        } else {
            cupMsgId = 1414;
            cupInfo.intVals[0] = globalContext->m_match;
            cupInfo.intVals[1] = globalContext->m_matchCount;
        }
    } else if (currentSectionId == SectionId::BTDemo) {
        if (raceScenario.battleType == 0) {
            cupIcon = "icon_09_balloon";
            cupMsgId = 1415;
        } else if (raceScenario.battleType == 1) {
            cupIcon = "icon_10_coin";
            cupMsgId = 1416;
        }
    } else {
        m_cupDisplay.setVisible(false);
    }

    m_cupDisplay.setPicture("cup_icon", cupIcon);
    m_cupDisplay.setMessageAll(cupMsgId, &cupInfo);
}

} // namespace UI
