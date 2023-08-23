#pragma once

#include "game/ui/RankingPage.hh"
#include "game/util/Registry.hh"

extern "C" {
#include <protobuf/Ranking.pb.h>
#include <revolutionex/nhttp.h>
}

namespace UI {

class SPRankingDownloadPage : public Page {
public:
    SPRankingDownloadPage();
    ~SPRankingDownloadPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onDeinit() override;
    void onActivate() override = 0;
    void afterCalc() override;
    void onRefocus() override;

protected:
    enum class ResponseStatus {
        Ok,
        Error,
    };
    enum class State {
        Previous,
        InDevelopment,
        Request,
        RequestError,
        Response,
    };

    virtual void transition(State state) = 0;

    bool makeRequest(const char *url);
    s32 getRegionParameterValue() const;
    s32 getCourseParameterValue() const;

    ResponseStatus responseStatus() const;
    const RankingResponse &rankingResponse() const;

    State m_state;
    PageId m_replacement;

private:
    State resolve();

    bool hasRequestTimedOut() const;
    void requestCallback(NHTTPError error, NHTTPResponseHandle responseHandle);
    ResponseStatus processResponse(NHTTPResponseHandle responseHandle);

    static void *NHTTPAlloc(u32 size, int align);
    static void NHTTPFree(void *block);
    static void RequestCallback(NHTTPError error, NHTTPResponseHandle responseHandle, void *arg);

    MenuInputManager m_inputManager;

    NHTTPRequestHandle m_requestHandle;
    int m_requestId;
    OSTime m_requestTimeout;
    ResponseStatus m_responseStatus;
    std::array<char, RankingResponse_size> m_responseBuffer;
    RankingResponse m_rankingResponse;

    static bool s_initialisedNHTTPLibrary;
    static EGG::Heap *s_nhttpHeap;

    static constexpr u32 s_nhttpHeapSize = 0x0000A860;
    static constexpr u32 s_nhttpThreadPriority = 17;
    static constexpr u32 s_requestTimeoutSeconds = 10;
};

} // namespace UI
