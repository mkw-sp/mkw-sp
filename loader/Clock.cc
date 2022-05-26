#include "Clock.hh"

namespace Clock {

void WaitMilliseconds(u32 milliseconds) {
    u32 duration = milliseconds * 60750;
    u32 start;
    asm volatile("mftbl %0" : "=r"(start));
    for (u32 current = start; current - start < duration;) {
        asm volatile("mftbl %0" : "=r"(current));
    }
}

} // namespace Clock
