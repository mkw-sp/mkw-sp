#include "FlameGraph.h"

void Perf_writeJsonHeader() {
    OSReport("$T {\"otherData\":{},\"traceEvents\":[\n");
}
void Perf_writeJsonEntry(u32 start, u32 stop, const char *name, u32 thread_id) {
    const u32 dur = stop - start;

    const f64 factor = ((f64)(OS_TIMER_CLOCK)) / 1000.0;

    OSReport(
            "$T {\"cat\":\"function\","
            "\"dur_ticks\":%u,"
            "\"dur\":%f,"
            "\"name\":\"%s\","
            "\"ph\":\"X\","
            "\"pid\":\"0\","
            "\"tid\":\"%u\","
            "\"ts\":%f},\n",
            dur, (((f64)dur) / factor), name, thread_id, ((f64)start) / factor);
}
void Perf_writeJsonFooter() {
    OSReport("$T ]}\n");
}
