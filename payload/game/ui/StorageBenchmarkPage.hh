#pragma once

#include "game/ui/Page.hh"

#include <sp/storage/Storage.hh>

extern "C" {
#include <revolution.h>
}

namespace UI {

class StorageBenchmarkPage : public Page {
public:
    StorageBenchmarkPage();
    StorageBenchmarkPage(const StorageBenchmarkPage &) = delete;
    StorageBenchmarkPage(StorageBenchmarkPage &&) = delete;
    ~StorageBenchmarkPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onActivate() override;
    void beforeInAnim() override;
    void beforeOutAnim() override;
    void afterCalc() override;
    void onRefocus() override;

private:
    enum class State {
        Prev,
        ChooseDevice,
        BenchmarkFAT,
        BenchmarkNet,
        BenchmarkFATOk,
        BenchmarkNetOk,
        BenchmarkFATFail,
        BenchmarkNetFail,
        Next,
    };

    State resolve();
    void transition(State state);
    u32 getStorageMessageId();
    void benchmark();

    static u32 GetStorageMessageId(SP::Storage::StorageType type);
    static void *Benchmark(void *arg);

    MenuInputManager m_inputManager;
    State m_state;
    u8 m_stack[0x1000 /* 4 KiB */];
    OSThread m_thread;
    SP::Storage::StorageType m_type;
    u8 m_buffer[SP::Storage::BENCHMARK_BUFFER_SIZE];
    std::optional<SP::Storage::Throughputs> m_throughputs;
};

} // namespace UI
