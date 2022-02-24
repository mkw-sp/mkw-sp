#include <Common.S>

.global mdelay
mdelay:
    // Convert milliseconds to ticks
    lis r4, 0
    ori r4, r4, 60750
    mullw r3, r4, r3

    // Do the actual waiting
    mftbl r4
.Lloop:
    mftbl r5
    subf r5, r4, r5
    cmpw r5, r3
    blt .Lloop

    blr
