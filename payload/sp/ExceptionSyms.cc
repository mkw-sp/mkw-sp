extern "C" {
#include <sp/Panic.h>
}

namespace std {

__attribute__((__format__(__gnu_printf__, 1, 2))) __attribute__((__noreturn__)) void
__throw_out_of_range_fmt(const char *, ...) {
    panic("std::__throw_out_of_range_fmt");
}

// Clang-only
extern "C" void __cxa_pure_virtual() {
    panic("__cxa_pure_virtual");
}

} // namespace std
