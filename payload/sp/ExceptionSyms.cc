extern "C" {
#include <sp/Panic.h>
}

namespace std {

__attribute__((noreturn)) void __throw_logic_error(const char *) {
    panic("std::__throw_logic_error");
}

__attribute__((noreturn)) void __throw_bad_alloc(const char *) {
    panic("std::__throw_bad_alloc");
}

__attribute__((noreturn)) void __throw_length_error(const char *) {
    panic("std::__throw_length_error");
}

__attribute__((noreturn)) void __throw_bad_array_new_length(void) {
    panic("std::__throw_bad_array_new_length");
}

__attribute__((noreturn)) void __throw_bad_alloc(void) {
    panic("std::__throw_bad_alloc");
}

__attribute__((__format__(__gnu_printf__, 1, 2))) __attribute__((noreturn)) void
__throw_out_of_range_fmt(const char *, ...) {
    panic("std::__throw_out_of_range_fmt");
}

} // namespace std

extern "C" {
// Clang-only
void __cxa_pure_virtual() {
    panic("__cxa_pure_virtual");
}

void abort() {
    panic("abort");
}
}
