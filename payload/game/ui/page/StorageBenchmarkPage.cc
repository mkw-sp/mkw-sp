#include "StorageBenchmarkPage.hh"

#include "game/ui/SectionManager.hh"
#include "game/ui/page/AwaitPage.hh"
#include "game/ui/page/MessagePage.hh"
#include "game/ui/page/Option.hh"

#include <cwchar>

namespace UI {

StorageBenchmarkPage::StorageBenchmarkPage() = default;

StorageBenchmarkPage::~StorageBenchmarkPage() = default;

PageId StorageBenchmarkPage::getReplacement() {
    return PageId::ServicePackTools;
}

void StorageBenchmarkPage::onInit() {
    m_inputManager.init(0, false);
    setInputManager(&m_inputManager);

    initChildren(0);

    setAnimSfxIds(0, 0);
}

void StorageBenchmarkPage::onActivate() {
    m_state = State::Prev;
    transition(State::ChooseDevice);
}

void StorageBenchmarkPage::beforeInAnim() {
    skipInAnim();
}

void StorageBenchmarkPage::beforeOutAnim() {
    skipOutAnim();
}

void StorageBenchmarkPage::afterCalc() {
    Section *section = SectionManager::Instance()->currentSection();
    auto *optionAwaitPage = section->page<PageId::OptionAwait>();
    MessageInfo info{};
    switch (m_state) {
    case State::BenchmarkFAT:
    case State::BenchmarkNet:
        if (auto status = SP::Storage::GetBenchmarkStatus()) {
            info.messageIds[0] = getStorageMessageId() + 1;
            if (status->mode == SP::Storage::BenchmarkStatus::Mode::Read) {
                info.messageIds[1] = 10152;
            } else {
                info.messageIds[1] = 10153;
            }
            info.intVals[0] = status->size / 1024;
            optionAwaitPage->setWindowMessage(10148, &info);
        }
        if (OSIsThreadTerminated(&m_thread)) {
            OSDetachThread(&m_thread);
            optionAwaitPage->pop();
        }
        break;
    default:
        break;
    }
}

void StorageBenchmarkPage::onRefocus() {
    transition(resolve());
}

StorageBenchmarkPage::State StorageBenchmarkPage::resolve() {
    Section *section = SectionManager::Instance()->currentSection();
    auto *optionSelect2Page = section->page<PageId::OptionSelect2>();
    switch (m_state) {
    case State::Prev:
        break;
    case State::ChooseDevice:
        switch (optionSelect2Page->choice()) {
        case -1:
            return State::Prev;
        case 0:
            return State::BenchmarkFAT;
        case 1:
            return State::BenchmarkNet;
        }
        break;
    case State::BenchmarkFAT:
        if (m_throughputs) {
            return State::BenchmarkFATOk;
        } else {
            return State::BenchmarkFATFail;
        }
    case State::BenchmarkNet:
        if (m_throughputs) {
            return State::BenchmarkNetOk;
        } else {
            return State::BenchmarkNetFail;
        }
    case State::BenchmarkFATOk:
    case State::BenchmarkNetOk:
    case State::BenchmarkFATFail:
    case State::BenchmarkNetFail:
        return State::Next;
    case State::Next:
        break;
    }

    return m_state;
}

void StorageBenchmarkPage::transition(State state) {
    if (state == m_state) {
        return;
    }

    u8 *stackTop = m_stack + sizeof(m_stack);
    Section *section = SectionManager::Instance()->currentSection();
    auto *optionSelect2Page = section->page<PageId::OptionSelect2>();
    auto *optionAwaitPage = section->page<PageId::OptionAwait>();
    auto *optionMessagePage = section->page<PageId::OptionMessage>();
    MessageInfo info{};
    wchar_t throughputs[8][0x10];
    auto printMebibytes = [](u32 val, wchar_t(&mebibytes)[0x10]) {
        swprintf(mebibytes, std::size(mebibytes), L"%.2f", (f32)val / (1024 * 1024));
    };
    switch (state) {
    case State::Prev:
        startReplace(Anim::Prev, 0.0f);
        break;
    case State::ChooseDevice:
        optionSelect2Page->setTitleMessage(10146);
        optionSelect2Page->setWindowMessage(10147, &info);
        optionSelect2Page->setButtonMessages(0, GetStorageMessageId(SP::Storage::StorageType::FAT));
        optionSelect2Page->setButtonMessages(1, GetStorageMessageId(SP::Storage::StorageType::Net));
        push(PageId::OptionSelect2, Anim::Next);
        break;
    case State::BenchmarkFAT:
    case State::BenchmarkNet:
        if (state == State::BenchmarkFAT) {
            m_type = SP::Storage::StorageType::FAT;
        } else {
            m_type = SP::Storage::StorageType::Net;
        }
        m_throughputs.reset();
        OSCreateThread(&m_thread, Benchmark, this, stackTop, sizeof(m_stack), 24, 0);
        OSResumeThread(&m_thread);
        optionAwaitPage->reset();
        optionAwaitPage->setTitleMessage(10146);
        info.messageIds[0] = getStorageMessageId() + 1;
        info.messageIds[1] = 10151;
        optionAwaitPage->setWindowMessage(10148, &info);
        push(PageId::OptionAwait, Anim::Next);
        break;
    case State::BenchmarkFATOk:
    case State::BenchmarkNetOk:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10146);
        info.messageIds[0] = getStorageMessageId() + 1;
        for (u32 i = 0; i < 4; i++) {
            info.intVals[i] = m_throughputs->sizes[i] / 1024;
            printMebibytes(m_throughputs->read[i], throughputs[2 * i + 0]);
            printMebibytes(m_throughputs->write[i], throughputs[2 * i + 1]);
            info.strings[2 * i + 0] = throughputs[2 * i + 0];
            info.strings[2 * i + 1] = throughputs[2 * i + 1];
        }
        optionMessagePage->setWindowMessage(10149, &info);
        push(PageId::OptionMessage, Anim::Next);
        break;
    case State::BenchmarkFATFail:
    case State::BenchmarkNetFail:
        optionMessagePage->reset();
        optionMessagePage->setTitleMessage(10146);
        info.messageIds[0] = getStorageMessageId() + 1;
        optionMessagePage->setWindowMessage(10150, &info);
        push(PageId::OptionMessage, Anim::Next);
        break;
    case State::Next:
        startReplace(Anim::Next, 0.0f);
        break;
    }
    m_state = state;
}

u32 StorageBenchmarkPage::getStorageMessageId() {
    return GetStorageMessageId(m_type);
}

void StorageBenchmarkPage::benchmark() {
    m_throughputs = SP::Storage::Benchmark(m_type, m_buffer);
}

u32 StorageBenchmarkPage::GetStorageMessageId(SP::Storage::StorageType type) {
    return SP::Storage::GetMessageId(type);
}

void *StorageBenchmarkPage::Benchmark(void *arg) {
    reinterpret_cast<StorageBenchmarkPage *>(arg)->benchmark();
    return nullptr;
}

} // namespace UI
