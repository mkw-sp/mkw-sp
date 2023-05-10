#include <ctype.h>

#ifdef isdigit
#undef isdigit
#endif
#ifdef isxdigit
#undef isxdigit
#endif

int isxdigit(int c) {
    return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}
