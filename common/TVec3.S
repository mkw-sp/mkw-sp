#include <Common.S>

.global _ZplRK4Vec3S1_
_ZplRK4Vec3S1_:
    psq_l  f2, 0x0 (r4), 0x0, GQR0
    psq_l  f4, 0x0 (r5), 0x0, GQR0
    ps_add f6, f2, f4
    psq_st f6, 0x0 (r3), 0x0, GQR0
    psq_l  f3, 0x8 (r4), 0x1, GQR0
    psq_l  f5, 0x8 (r5), 0x1, GQR0
    ps_add f7, f3, f5
    psq_st f7, 0x8 (r3), 0x1, GQR0
    blr
.size _ZplRK4Vec3S1_, . - _ZplRK4Vec3S1_
