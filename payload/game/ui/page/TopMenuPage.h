#include "MenuPage.h"

#include "../Button.h"

typedef struct {
    MenuPage;
    u8 _430[0xcb4 - 0x430];
    PushButton *fileAdminButton;
} TopMenuPage;
static_assert(sizeof(TopMenuPage) == 0xcb8);

TopMenuPage *TopMenuPage_ct(TopMenuPage *this);
