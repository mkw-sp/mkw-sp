#pragma once

#include "game/ui/RankingPage.hh"
#include "game/util/Registry.hh"

extern "C" {
#include <protobuf/Ranking.pb.h>
#include <revolutionex/nhttp.h>
}

namespace UI {

class RankingDownloadManagerPage : public Page {
public:
    RankingDownloadManagerPage();
    ~RankingDownloadManagerPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onDeinit() override;
    void onActivate() override;
    void afterCalc() override;
    void onRefocus() override;

    void setCourse(Registry::Course course);
    void setArea(RankingPage::Area area);

private:
    enum class State {
        InDevelopment,
        Request,
        Response,
        Finished,
    };
    enum class ResponseStatus {
        Ok,
        RequestError,
        ResponseError,
    };

    State resolve();
    void transition(State state);

    bool makeRequest();
    s32 getRegionParameterValue();
    bool hasRequestTimedOut();
    void requestCallback(NHTTPError error, NHTTPResponseHandle responseHandle);
    ResponseStatus processResponse(NHTTPResponseHandle responseHandle);

    static void *NHTTPAlloc(u32 size, int align);
    static void NHTTPFree(void *block);
    static void RequestCallback(NHTTPError error, NHTTPResponseHandle responseHandle, void *arg);

    PageId m_replacement;
    MenuInputManager m_inputManager;

    Registry::Course m_course = Registry::Course::LuigiCircuit;
    RankingPage::Area m_area = RankingPage::Area::Friend;

    State m_state;
    bool m_initialisedNHTTPLibrary = false;
    NHTTPRequestHandle m_requestHandle;
    int m_requestId;
    OSTime m_requestTimeout;
    ResponseStatus m_responseStatus;
    std::array<char, RankingResponse_size> m_responseBuffer;
    RankingResponse m_rankingResponse;

    static constexpr u32 s_nhttpThreadPriority = 17;
    static constexpr u32 s_requestTimeoutSeconds = 10;
};

} // namespace UI
