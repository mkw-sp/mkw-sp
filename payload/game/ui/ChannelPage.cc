#include "ChannelPage.hh"

extern "C" {
#include "game/system/GameScene.h"
}
#include "game/ui/SectionManager.hh"
#include "sp/Channel.hh"

#include <new>

namespace UI {

ChannelPage::ChannelPage() = default;

ChannelPage::~ChannelPage() = default;

PageId ChannelPage::getReplacement() {
    return m_replacement;
}

void ChannelPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);

    initChildren(0);

    setAnimSfxIds(0, 0);

    if (versionInfo.type == BUILD_TYPE_RELEASE) {
        EGG_Heap *heap = GameScene_get()->volatileHeapCollection.heaps[HEAP_ID_MEM2];
        SP::Channel::Load(reinterpret_cast<EGG::Heap *>(heap));
    }
}

void ChannelPage::onDeinit() {
    if (versionInfo.type == BUILD_TYPE_RELEASE) {
        SP::Channel::Unload();
    }
}

void ChannelPage::onActivate() {
    m_replacement = PageId::None;

    m_state = State::Prev;
    if (versionInfo.type == BUILD_TYPE_RELEASE) {
        transition(State::Explanation);
    } else {
        transition(State::Unsupported);
    }
}

void ChannelPage::beforeInAnim() {
    skipInAnim();
}

void ChannelPage::beforeOutAnim() {
    skipOutAnim();
}

void ChannelPage::afterCalc() {
    Section *section = SectionManager::Instance()->currentSection();
    auto *optionAwaitPage = section->page<PageId::OptionAwait>();
    switch (m_state) {
    case State::Install:
    case State::Update:
        if (OSIsThreadTerminated(&m_thread)) {
            OSDetachThread(&m_thread);
            optionAwaitPage->pop();
        }
        break;
    default:
        break;
    }
}

void ChannelPage::onRefocus() {
    transition(resolve());
}

ChannelPage::State ChannelPage::resolve() {
    Section *section = SectionManager::Instance()->currentSection();
    auto *optionExplanationPage = section->page<PageId::OptionExplanation>();
    auto *optionSelect2Page = section->page<PageId::OptionSelect2>();
    auto *optionConfirmPage = section->page<PageId::OptionConfirm>();
    switch (m_state) {
    case State::Prev:
        break;
    case State::Explanation:
        switch (optionExplanationPage->choice()) {
        case 0:
            switch (SP::Channel::GetStatus()) {
            case SP::Channel::Status::None:
                return State::None;
            case SP::Channel::Status::Older:
                return State::Older;
            case SP::Channel::Status::Same:
                return State::Same;
            case SP::Channel::Status::Newer:
                return State::Newer;
            }
            break;
        case 1:
            return State::Prev;
        }
        break;
    case State::None:
        switch (optionSelect2Page->choice()) {
        case -1:
            return State::Explanation;
        case 0:
            return State::Install;
        case 1:
            return State::Next;
        }
        break;
    case State::Older:
        switch (optionSelect2Page->choice()) {
        case -1:
            return State::Explanation;
        case 0:
            return State::Update;
        case 1:
            return State::Next;
        }
        break;
    case State::Same:
        switch (optionConfirmPage->choice()) {
        case 0:
            return State::Next;
        case 1:
            return State::Explanation;
        }
        break;
    case State::Newer:
        switch (optionConfirmPage->choice()) {
        case 0:
            return State::Next;
        case 1:
            return State::Explanation;
        }
        break;
    case State::Install:
        switch (SP::Channel::GetStatus()) {
        case SP::Channel::Status::Same:
            return State::InstallOk;
        default:
            return State::InstallFail;
        }
    case State::Update:
        switch (SP::Channel::GetStatus()) {
        case SP::Channel::Status::Same:
            return State::UpdateOk;
        default:
            return State::UpdateFail;
        }
    case State::InstallOk:
    case State::InstallFail:
    case State::UpdateOk:
    case State::UpdateFail:
    case State::Unsupported:
        return State::Next;
    case State::Next:
        break;
    }

    return m_state;
}

void ChannelPage::transition(State state) {
    if (state == m_state) {
        return;
    }

    u8 *stackTop = m_stack + sizeof(m_stack);
    Section *section = SectionManager::Instance()->currentSection();
    auto *optionExplanationPage = section->page<PageId::OptionExplanation>();
    auto *optionSelect2Page = section->page<PageId::OptionSelect2>();
    auto *optionAwaitPage = section->page<PageId::OptionAwait>();
    auto *optionMessagePage = section->page<PageId::OptionMessage>();
    auto *optionConfirmPage = section->page<PageId::OptionConfirm>();
    switch (state) {
    case State::Prev:
        m_replacement = PageId::ServicePackTop;
        startReplace(Anim::Prev, 0.0f);
        break;
    case State::Explanation:
        optionExplanationPage->setTitleMessage(10088);
        optionExplanationPage->setWindowMessage(10090);
        push(PageId::OptionExplanation, m_state == State::Prev ? Anim::Next : Anim::Prev);
        break;
    case State::None:
        optionSelect2Page->setTitleMessage(10088);
        optionSelect2Page->setWindowMessage(10091);
        optionSelect2Page->setButtonMessages(0, 2002);
        optionSelect2Page->setButtonMessages(1, 2003);
        push(PageId::OptionSelect2, Anim::Next);
        break;
    case State::Older:
        optionSelect2Page->setTitleMessage(10088);
        optionSelect2Page->setWindowMessage(10092);
        optionSelect2Page->setButtonMessages(0, 2002);
        optionSelect2Page->setButtonMessages(1, 2003);
        push(PageId::OptionSelect2, Anim::Next);
        break;
    case State::Same:
        optionConfirmPage->reset();
        optionConfirmPage->setTitleMessage(10088);
        optionConfirmPage->setWindowMessage(10093);
        push(PageId::OptionConfirm, Anim::Next);
        break;
    case State::Newer:
        optionConfirmPage->reset();
        optionConfirmPage->setTitleMessage(10088);
        optionConfirmPage->setWindowMessage(10094);
        push(PageId::OptionConfirm, Anim::Next);
        break;
    case State::Install:
        OSCreateThread(&m_thread, Install, nullptr, stackTop, sizeof(m_stack), 24, 0);
        OSResumeThread(&m_thread);
        optionAwaitPage->reset();
        optionAwaitPage->setTitleMessage(10088);
        optionAwaitPage->setWindowMessage(10095);
        push(PageId::OptionAwait, Anim::Next);
        break;
    case State::Update:
        OSCreateThread(&m_thread, Install, nullptr, stackTop, sizeof(m_stack), 24, 0);
        OSResumeThread(&m_thread);
        optionAwaitPage->reset();
        optionAwaitPage->setTitleMessage(10088);
        optionAwaitPage->setWindowMessage(10096);
        push(PageId::OptionAwait, Anim::Next);
        break;
    case State::InstallOk:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10088);
        optionMessagePage->setWindowMessage(10097);
        push(PageId::OptionMessage, Anim::Next);
        break;
    case State::InstallFail:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10088);
        optionMessagePage->setWindowMessage(10098);
        push(PageId::OptionMessage, Anim::Next);
        break;
    case State::UpdateOk:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10088);
        optionMessagePage->setWindowMessage(10099);
        push(PageId::OptionMessage, Anim::Next);
        break;
    case State::UpdateFail:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10088);
        optionMessagePage->setWindowMessage(10100);
        push(PageId::OptionMessage, Anim::Next);
        break;
    case State::Unsupported:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10088);
        optionMessagePage->setWindowMessage(10101);
        push(PageId::OptionMessage, Anim::Next);
        break;
    case State::Next:
        m_replacement = PageId::ServicePackTop;
        startReplace(Anim::Next, 0.0f);
        break;
    }
    m_state = state;
}

void *ChannelPage::Install(void *UNUSED(arg)) {
    SP::Channel::Install();
    return nullptr;
}

} // namespace UI

extern "C" void *ChannelPage_ct(void *self) {
    return new (self) UI::ChannelPage();
}

static_assert(sizeof_ChannelPage == sizeof(UI::ChannelPage));
