#pragma once

#include <Common.h>
#include <nw4r/lyt/lyt_pane.h>

typedef struct {
    char _00[0x0c - 0x00];
} Layout;

typedef struct {
    Layout;
    char _0c[0x9c-0x0c];
} MainLayout;

lyt_Pane *Layout_findPaneByName(Layout *layout, const char *name);
