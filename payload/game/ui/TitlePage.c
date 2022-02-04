#include "TitlePage.h"

#include "MessageWindowPage.h"
#include "SectionManager.h"

static bool geckoWarningWasShown = false;

void TitlePage_processGeckoWarning(TitlePage *this) {
    if (*(u8 *)0x800018A8 != 0x94) {
        return;
    }

    if (geckoWarningWasShown) {
        return;
    }

    geckoWarningWasShown = true;

    Section *currentSection = s_sectionManager->currentSection;
    MessageWindowPopupPage *messageWindowPopupPage =
            (MessageWindowPopupPage *)currentSection->pages[PAGE_ID_MESSAGE_WINDOW_POPUP];
    MessageWindowPopupPage_reset(messageWindowPopupPage);
    MessageWindowPopupPage_setMessage(messageWindowPopupPage, 0x3037, NULL);
    Page_push(this, PAGE_ID_MESSAGE_WINDOW_POPUP, PAGE_ANIMATION_NEXT);
}
