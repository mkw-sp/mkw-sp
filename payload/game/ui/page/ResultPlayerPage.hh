#pragma once

#include "game/ui/page/ResultPage.hh"

namespace UI {

class ResultPlayerPage : public ResultPage {
public:
    ResultPlayerPage();
    ~ResultPlayerPage() override;
    void onInit() override;
    TypeInfo *getTypeInfo() const override;
    bool isBusy() override;
    virtual void vf_6c();
    virtual void vf_70();
};

class ResultRaceUpdatePage : public ResultPlayerPage {
public:
    ResultRaceUpdatePage();
    ~ResultRaceUpdatePage() override;
    REPLACE PageId getReplacement() override;
    void vf_68() override;
    TypeInfo *getTypeInfo() const override;
};

} // namespace UI
