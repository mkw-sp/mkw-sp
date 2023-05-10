#include <ctype.h>

#ifdef isdigit
#undef isdigit
#endif

int isdigit(int c) {
    return c >= '0' && c <= '9';
}
