#include "UpdatePage.hh"

#include "game/ui/AwaitPage.hh"
#include "game/ui/Option.hh"
#include "game/ui/SectionManager.hh"

#include <sp/Update.hh>

#include <cwchar>

namespace UI {

UpdatePage::UpdatePage() = default;

UpdatePage::~UpdatePage() = default;

PageId UpdatePage::getReplacement() {
    return m_replacement;
}

void UpdatePage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);

    initChildren(0);

    setAnimSfxIds(Sound::SoundId::SE_DUMMY, Sound::SoundId::SE_DUMMY);

    u8 *stackTop = m_stack + sizeof(m_stack);
    OSCreateThread(&m_thread, Check, nullptr, stackTop, sizeof(m_stack), 24, 0);
    OSDetachThread(&m_thread);
    OSResumeThread(&m_thread);
}

void UpdatePage::onActivate() {
    m_replacement = PageId::None;

    m_state = State::Prev;
    if (versionInfo.type == BUILD_TYPE_RELEASE) {
        if (OSIsThreadTerminated(&m_thread)) {
            if (!reinterpret_cast<u32>(m_thread.val)) {
                u8 *stackTop = m_stack + sizeof(m_stack);
                OSCreateThread(&m_thread, Check, nullptr, stackTop, sizeof(m_stack), 24, 0);
                OSResumeThread(&m_thread);
                transition(State::Check);
            } else if (!SP::Update::GetInfo() || !SP::Update::GetInfo()->updated) {
                transition(State::CheckOk);
            } else {
                transition(State::UpdateOk);
            }
        } else {
            transition(State::Check);
        }
    } else {
        transition(State::Unsupported);
    }
}

void UpdatePage::beforeInAnim() {
    skipInAnim();
}

void UpdatePage::beforeOutAnim() {
    skipOutAnim();
}

void UpdatePage::afterCalc() {
    Section *section = SectionManager::Instance()->currentSection();
    auto *optionAwaitPage = section->page<PageId::OptionAwait>();
    u32 messageId;
    MessageInfo info{};
    wchar_t downloadedSize[0x10];
    wchar_t size[0x10];
    wchar_t throughput[0x10];
    auto printMebibytes = [](u32 val, wchar_t(&mebibytes)[0x10]) {
        swprintf(mebibytes, std::size(mebibytes), L"%.2f", (f32)val / (1024 * 1024));
    };
    switch (m_state) {
    case State::Check:
    case State::Update:
        messageId = m_state == State::Check ? 10102 : 10107;
        switch (SP::Update::GetStatus()) {
        case SP::Update::Status::Connect:
            info.messageIds[0] = 10112;
            break;
        case SP::Update::Status::SendInfo:
            info.messageIds[0] = 10113;
            break;
        case SP::Update::Status::ReceiveInfo:
            info.messageIds[0] = 10114;
            break;
        case SP::Update::Status::Download:
            printMebibytes(SP::Update::GetInfo()->downloadedSize, downloadedSize);
            printMebibytes(SP::Update::GetInfo()->size, size);
            printMebibytes(SP::Update::GetInfo()->throughput, throughput);
            info.messageIds[0] = 10115;
            info.strings[0] = downloadedSize;
            info.strings[1] = size;
            info.strings[2] = throughput;
            break;
        case SP::Update::Status::Move:
            info.messageIds[0] = 10116;
            break;
        case SP::Update::Status::Idle:
            info.messageIds[0] = 10117;
            break;
        }
        optionAwaitPage->setWindowMessage(messageId, &info);
        if (OSIsThreadTerminated(&m_thread)) {
            optionAwaitPage->pop();
        }
        break;
    default:
        break;
    }
}

void UpdatePage::onRefocus() {
    transition(resolve());
}

void UpdatePage::onCheckOkFront(MessagePage *messagePage) {
    messagePage->setAnim(Anim::Next);
}

UpdatePage::State UpdatePage::resolve() {
    Section *section = SectionManager::Instance()->currentSection();
    auto *optionSelect2Page = section->page<PageId::OptionSelect2>();
    switch (m_state) {
    case State::Prev:
        break;
    case State::Check:
        if (reinterpret_cast<u32>(m_thread.val)) {
            return State::CheckOk;
        } else {
            return State::CheckFail;
        }
    case State::CheckOk:
        if (SP::Update::GetInfo()) {
            return State::ConfirmUpdate;
        } else {
            return State::Next;
        }
    case State::CheckFail:
        return State::Next;
    case State::ConfirmUpdate:
        switch (optionSelect2Page->choice()) {
        case -1:
            return State::CheckOk;
        case 0:
            return State::Update;
        case 1:
            return State::Next;
        }
        break;
    case State::Update:
        if (reinterpret_cast<u32>(m_thread.val)) {
            return State::UpdateOk;
        } else {
            return State::UpdateFail;
        }
    case State::UpdateOk:
        return State::ConfirmRestart;
    case State::UpdateFail:
        return State::Next;
    case State::ConfirmRestart:
        switch (optionSelect2Page->choice()) {
        case -1:
            return State::UpdateOk;
        case 0:
            return State::Restart;
        case 1:
            return State::Next;
        }
        break;
    case State::Restart:
        break;
    case State::Unsupported:
        return State::Next;
    case State::Next:
        break;
    }

    return m_state;
}

void UpdatePage::transition(State state) {
    if (state == m_state) {
        return;
    }

    u8 *stackTop = m_stack + sizeof(m_stack);
    Section *section = SectionManager::Instance()->currentSection();
    auto *optionSelect2Page = section->page<PageId::OptionSelect2>();
    auto *optionAwaitPage = section->page<PageId::OptionAwait>();
    auto *optionMessagePage = section->page<PageId::OptionMessage>();
    MessageInfo info{};
    wchar_t oldVersion[0x10];
    wchar_t newVersion[0x10];
    wchar_t size[0x10];
    auto printVersion = [](const VersionInfo &info, wchar_t(&version)[0x10]) {
        u16 major = info.major;
        u16 minor = info.minor;
        u16 patch = info.patch;
        swprintf(version, std::size(version), L"v%u.%u.%u", major, minor, patch);
    };
    auto printMebibytes = [](u32 val, wchar_t(&mebibytes)[0x10]) {
        swprintf(mebibytes, std::size(mebibytes), L"%.2f", (f32)val / (1024 * 1024));
    };
    switch (state) {
    case State::Prev:
        m_replacement = PageId::ServicePackTop;
        startReplace(Anim::Prev, 0.0f);
        break;
    case State::Check:
        optionAwaitPage->reset();
        optionAwaitPage->setTitleMessage(10087);
        info.messageIds[0] = 10117;
        optionAwaitPage->setWindowMessage(10102, &info);
        push(PageId::OptionAwait, Anim::Next);
        break;
    case State::CheckOk:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10087);
        if (SP::Update::GetInfo()) {
            printVersion(versionInfo, oldVersion);
            printVersion(SP::Update::GetInfo()->version, newVersion);
            info.strings[0] = oldVersion;
            info.strings[1] = newVersion;
            optionMessagePage->setWindowMessage(10103, &info);
        } else {
            printVersion(versionInfo, oldVersion);
            info.strings[0] = oldVersion;
            optionMessagePage->setWindowMessage(10104, &info);
        }
        optionMessagePage->m_handler = &m_onCheckOkFront;
        push(PageId::OptionMessage, m_state == State::ConfirmUpdate ? Anim::Prev : Anim::Next);
        break;
    case State::CheckFail:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10087);
        optionMessagePage->setWindowMessage(10105);
        push(PageId::OptionMessage, Anim::Next);
        break;
    case State::ConfirmUpdate:
        optionSelect2Page->setTitleMessage(10087);
        printVersion(SP::Update::GetInfo()->version, newVersion);
        printMebibytes(SP::Update::GetInfo()->size, size);
        info.strings[0] = newVersion;
        info.strings[1] = size;
        info.intVals[0] = (SP::Update::GetInfo()->size + 131072 - 1) / 131072;
        optionSelect2Page->setWindowMessage(10106, &info);
        optionSelect2Page->setButtonMessages(0, 2002);
        optionSelect2Page->setButtonMessages(1, 2003);
        push(PageId::OptionSelect2, Anim::Next);
        break;
    case State::Update:
        OSCreateThread(&m_thread, Update, nullptr, stackTop, sizeof(m_stack), 24, 0);
        OSResumeThread(&m_thread);
        optionAwaitPage->reset();
        optionAwaitPage->setTitleMessage(10087);
        info.messageIds[0] = 10117;
        optionAwaitPage->setWindowMessage(10107, &info);
        push(PageId::OptionAwait, Anim::Next);
        break;
    case State::UpdateOk:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10087);
        printVersion(versionInfo, oldVersion);
        printVersion(SP::Update::GetInfo()->version, newVersion);
        info.strings[0] = oldVersion;
        info.strings[1] = newVersion;
        optionMessagePage->setWindowMessage(10108, &info);
        optionMessagePage->m_handler = &m_onCheckOkFront;
        push(PageId::OptionMessage, m_state == State::ConfirmRestart ? Anim::Prev : Anim::Next);
        break;
    case State::UpdateFail:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10087);
        optionMessagePage->setWindowMessage(10109);
        push(PageId::OptionMessage, Anim::Next);
        break;
    case State::ConfirmRestart:
        optionSelect2Page->setTitleMessage(10087);
        optionSelect2Page->setWindowMessage(10110);
        optionSelect2Page->setButtonMessages(0, 2002);
        optionSelect2Page->setButtonMessages(1, 2003);
        push(PageId::OptionSelect2, Anim::Next);
        break;
    case State::Restart:
        changeSection(SectionId::Restart, Anim::Next, 0.0f);
        break;
    case State::Unsupported:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10087);
        optionMessagePage->setWindowMessage(10111);
        push(PageId::OptionMessage, Anim::Next);
        break;
    case State::Next:
        m_replacement = PageId::ServicePackTop;
        startReplace(Anim::Next, 0.0f);
        break;
    }
    m_state = state;
}

void *UpdatePage::Check(void * /* arg */) {
    return reinterpret_cast<void *>(SP::Update::Check());
}

void *UpdatePage::Update(void * /* arg */) {
    return reinterpret_cast<void *>(SP::Update::Update());
}

} // namespace UI
