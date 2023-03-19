#include "game/ui/UIControl.hh"

namespace UI {

class CtrlMenuBusySymbol : public LayoutUIControl {
public:
    CtrlMenuBusySymbol();
    ~CtrlMenuBusySymbol() override;
};

static_assert(sizeof(CtrlMenuBusySymbol) == 0x174);

} // namespace UI