#include <wctype.h>

int iswdigit(wint_t wc) {
    return wc >= '0' && wc <= '9';
}
