
extern "C" {
#include <sp/Panic.h>
}

#include <bits/functexcept.h>

namespace std {

__attribute__((__noreturn__)) void __throw_logic_error(const char *) {
    panic("std::__throw_logic_error");
};

__attribute__((noreturn)) void __throw_bad_alloc(const char *) {
    panic("std::__throw_bad_alloc");
};

__attribute__((noreturn)) void __throw_length_error(const char *) {
    panic("std::__throw_length_error");
};

__attribute__((__noreturn__)) void __throw_bad_array_new_length(void) {
    panic("std::__throw_bad_array_new_length");
};

__attribute__((__noreturn__)) void __throw_bad_alloc(void) {
    panic("std::__throw_bad_alloc");
};

} // namespace std
