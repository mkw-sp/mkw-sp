#pragma once

#include <Common.h>
#include <nw4r/lyt/lyt_pane.h>

lyt_Pane* Layout_findPaneByName(void* layout, const char* name);

inline lyt_Pane* Layout_findPictureByName(void* layout, const char* name) {
    // TODO: RTTI check
    return Layout_findPaneByName(layout, name);
}