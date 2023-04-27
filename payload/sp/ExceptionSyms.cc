#include <bits/functexcept.h>

extern "C" {
#include <sp/Panic.h>
}

namespace std {

__attribute__((__format__(__gnu_printf__, 1, 2))) __attribute__((__noreturn__)) void
__throw_out_of_range_fmt(const char *, ...) {
    panic("std::__throw_out_of_range_fmt");
}

} // namespace std
