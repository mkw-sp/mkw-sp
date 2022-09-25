#include "Benchmark.h"

#include <revolution.h>

OSTime funcTime = 0;
OSTime totalStart = 0;

/*void *REPLACED(memcpy_slow)(void *dest, const void *src, size_t n);
REPLACE void *memcpy_slow(void *dest, const void *src, size_t n);

void *NETMemCpy(void *dest, const void *src, size_t n);*/

/*void *memcpy_slow(void *dest, const void *src, size_t n) {
    u32 start = OSGetTick();
    //void *ret = REPLACED(memcpy_slow)(dest, src, n);
    //void *ret = NETMemCpy(dest, src, n);
    u32 end = OSGetTick();
    funcTime += end - start;
    return ret;
}*/

void *REPLACED(memset_slow)(void *s, int c, size_t n);
REPLACE void *memset_slow(void *s, int c, size_t n);

void *NETMemSet(void *s, int c, size_t n);

void *memset_slow(void *s, int c, size_t n) {
    u32 start = OSGetTick();
    //void *ret = REPLACED(memset_slow)(s, c, n);
    void *ret = NETMemSet(s, c, n);
    u32 end = OSGetTick();
    funcTime += end - start;
    return ret;
}

void Benchmark_Start(void) {
    funcTime = 0;
    totalStart = OSGetTime();
}

void Benchmark_End(void) {
    if (totalStart != 0) {
        OSTime totalTime = OSGetTime() - totalStart;
        SP_LOG("%llu %llu", funcTime, totalTime);
    }
}
