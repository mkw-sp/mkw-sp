#include "MenuPage.h"

#include "../Button.h"

typedef struct {
    MenuPage;
    u8 _430[0xcb4 - 0x430];
    PushButton *fileAdminButton;
} MainMenuPage;
static_assert(sizeof(MainMenuPage) == 0xcb8);
