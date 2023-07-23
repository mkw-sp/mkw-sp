#include "WifiDisconnectedPage.hh"

#include "game/ui/SectionManager.hh"

namespace UI {

// Only called by this page's vanilla onActivate, now incorrect.
REPLACE void WifiResetDisconnectReason(GlobalContext::OnlineDisconnectInfo *) {
    panic("WifiResetDisconnectReason stubbed");
}

u32 WifiErrorExplain(u32 error);

void WifiDisconnectedPage::onActivate() {
    m_okayButton.setVisible(true);

    auto *sectionManager = SectionManager::Instance();
    auto *globalContext = sectionManager->globalContext();

    auto *currentSection = sectionManager->currentSection();
    auto currentSectionId = currentSection->id();
    if (currentSectionId == SectionId::OnlineDisconnectedGeneric) {
        currentSection->shutdownNet();
        m_messageBox.setMessageAll(4018);
        return;
    }

    auto disconnectCategory = globalContext->m_onlineDisconnectInfo.m_category;
    u32 errorCode = globalContext->m_onlineDisconnectInfo.m_errorCode;

    if (disconnectCategory == OnlineErrorCategory::ErrorCode) {
        // SP error codes start at 30000 and are simply message IDs
        if (errorCode >= 30000) {
            MessageInfo *info = nullptr;
            if (globalContext->m_onlineDisconnectInfoInfo.has_value()) {
                info = &*globalContext->m_onlineDisconnectInfoInfo;
            }

            m_messageBox.setMessageAll(errorCode, info);
        } else {
            MessageInfo info;
            info.intVals[0] = errorCode;
            m_messageBox.setMessageAll(WifiErrorExplain(errorCode), &info);
        }

        currentSection->shutdownNet();
    } else if (disconnectCategory == OnlineErrorCategory::MiiInvalid) {
        m_messageBox.setMessageAll(4016);
        currentSection->shutdownNet();
    } else if (disconnectCategory == OnlineErrorCategory::GeneralDisconnect) {
        m_messageBox.setMessageAll(2072);
        currentSection->shutdownNet();
    } else if (disconnectCategory == OnlineErrorCategory::UnrecoverableDisconnect) {
        m_messageBox.setMessageAll(2052);
        m_okayButton.setVisible(false);
    }

    globalContext->m_onlineDisconnectInfo.m_category = OnlineErrorCategory::None;
    globalContext->m_onlineDisconnectInfo.m_errorCode = 0;
    globalContext->m_onlineDisconnectInfoInfo.reset();
}

} // namespace UI
