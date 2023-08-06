#pragma once

#include "game/ui/UIControl.hh"

namespace UI {

class BusySymbol : public LayoutUIControl {
public:
    BusySymbol();
    virtual ~BusySymbol();
};

static_assert(sizeof(BusySymbol) == sizeof(LayoutUIControl));

} // namespace UI
