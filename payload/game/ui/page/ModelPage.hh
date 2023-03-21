#pragma once

#include "game/ui/Page.hh"
#include "game/ui/ctrl/MenuModelControl.hh"

namespace UI {

class ModelPage : public Page {
public:
    BackGroundModelControl &modelControl();

private:
    u8 _044[0x1c8 - 0x044];
    BackGroundModelControl m_modelControl;
};
static_assert(sizeof(ModelPage) == 0x348);

} // namespace UI
