#include <Common.S>

.global _ZmlRK4QuatIfES2_
_ZmlRK4QuatIfES2_:
    psq_l      f0, 0x0 (r4), 0x0, GQR0
    psq_l      f1, 0x8 (r4), 0x0, GQR0
    ps_neg     f5, f0
    psq_l      f2, 0x0 (r5), 0x0, GQR0
    ps_neg     f6, f1
    psq_l      f3, 0x8 (r5), 0x0, GQR0
    ps_muls0   f7, f1, f2
    ps_merge01 f4, f5, f0
    ps_merge01 f1, f6, f1
    ps_muls0   f5, f5, f2
    ps_muls1   f8, f4, f2
    ps_madds0  f7, f4, f3,f7
    ps_muls1   f2, f1, f2
    ps_madds0  f5, f1, f3,f5
    ps_merge10 f7, f7, f7
    ps_madds1  f2, f0, f3,f2
    ps_merge10 f5, f5, f5
    ps_madds1  f8, f6, f3,f8
    ps_add     f7, f7, f2
    ps_sub     f5, f5, f8
    psq_st     f7, 0x0 (r3), 0x0, GQR0
    psq_st     f5, 0x8 (r3), 0x0, GQR0
    blr
.size _ZmlRK4QuatIfES2_, . - _ZmlRK4QuatIfES2_
