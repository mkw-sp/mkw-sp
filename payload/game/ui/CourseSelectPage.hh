#pragma once

#include "game/ui/BlackBackControl.hh"
#include "game/ui/CourseSelectButton.hh"
#include "game/ui/ScrollBar.hh"
#include "game/ui/SheetSelectControl.hh"
#include "game/ui/YesNoPage.hh"
#include "game/ui/ctrl/CtrlMenuBackButton.hh"
#include "game/ui/ctrl/CtrlMenuPageTitleText.hh"

#include <sp/ShaUtil.hh>
#include <sp/storage/Storage.hh>
#include <sp/trackPacks/Track.hh>
#include <vendor/tjpgd/tjpgd.h>

#include <atomic>
#include <memory>

namespace UI {

class CourseSelectPage : public Page {
public:
    CourseSelectPage();
    CourseSelectPage(const CourseSelectPage &) = delete;
    CourseSelectPage(CourseSelectPage &&) = delete;
    ~CourseSelectPage() override;

    PageId getReplacement() override;
    void onInit() override;
    void onDeinit() override;
    void onActivate() override;
    void afterCalc() override;
    void onRefocus() override;

    u32 sheetIndex() const;
    u32 lastSelected() const;

    void filter();
    void filter(SP::Track::Mode filter);
    void refreshSelection(u32 selection);

private:
    enum class Request {
        None,
        Change,
        Stop,
    };

    struct Context {
        CourseSelectPage *page;
        u32 i;
        SP::Storage::FileHandle file;
        u32 offset;
    };

    void onBack(u32 localPlayerId);
    void onButtonFront(PushButton *button, u32 localPlayerId);
    void onButtonSelect(PushButton *button, u32 localPlayerId);
    void onSheetSelectRight(SheetSelectControl *control, u32 localPlayerId);
    void onSheetSelectLeft(SheetSelectControl *control, u32 localPlayerId);
    void onScrollBarChange(ScrollBar *scrollBar, u32 localPlayerId, u32 chosen);
    void onBackButtonFront(PushButton *button, u32 localPlayerId);
    void onBackConfirm(s32 choice, PushButton *button);

    void onBackCommon(f32 delay);
    void refresh();
    void loadThumbnails();
    JRESULT loadThumbnail(u32 i, Sha1 courseSha1);

    static void *LoadThumbnails(void *arg);
    static size_t ReadCompressedThumbnail(JDEC *jdec, uint8_t *buffer, size_t size);
    static int WriteUncompressedThumbnail(JDEC *jdec, void *bitmap, JRECT *rect);

    template <typename T>
    using H = typename T::template Handler<CourseSelectPage>;

    static constexpr u32 MaxThumbnailWidth = 256;
    static constexpr u32 MaxThumbnailHeight = 144;
    static_assert(MaxThumbnailWidth % 8 == 0);
    static_assert(MaxThumbnailHeight % 4 == 0);

    MultiControlInputManager m_inputManager;
    CtrlMenuPageTitleText m_pageTitleText;
    std::array<CourseSelectButton, 9> m_buttons;
    SheetSelectControl m_sheetSelect;
    LayoutUIControl m_sheetLabel;
    ScrollBar m_scrollBar;
    std::unique_ptr<BlackBackControl> m_blackBackControl;
    CtrlMenuBackButton m_backButton;
    H<MultiControlInputManager> m_onBack{this, &CourseSelectPage::onBack};
    H<PushButton> m_onButtonFront{this, &CourseSelectPage::onButtonFront};
    H<PushButton> m_onButtonSelect{this, &CourseSelectPage::onButtonSelect};
    H<SheetSelectControl> m_onSheetSelectRight{this, &CourseSelectPage::onSheetSelectRight};
    H<SheetSelectControl> m_onSheetSelectLeft{this, &CourseSelectPage::onSheetSelectLeft};
    H<ScrollBar> m_onScrollBarChange{this, &CourseSelectPage::onScrollBarChange};
    H<PushButton> m_onBackButtonFront{this, &CourseSelectPage::onBackButtonFront};
    H<YesNoPage> m_onBackConfirm{this, &CourseSelectPage::onBackConfirm};
    bool m_backConfirmed;
    PageId m_replacement;
    SP::Track::Mode m_filter;
    u32 m_sheetCount;
    u32 m_sheetIndex;
    u32 m_lastSelected;
    Request m_request;
    std::array<std::atomic<bool>, 27> m_thumbnailChanged;
    std::array<std::optional<Sha1>, 27> m_databaseIds;
    std::array<std::array<std::unique_ptr<u8[]>, 3>, 27> m_buffers;
    std::array<std::array<GXTexObj, 3>, 27> m_texObjs;
    OSThreadQueue m_queue;
    u8 m_stack[0x5000 /* 20 KiB */];
    OSThread m_thread;

public:
    static std::optional<u32> s_lastSelected;
};

} // namespace UI
