#include "FlameGraph.h"

void Perf_writeJsonHeader() {
    OSReport("$T {\"otherData\":{},\"traceEvents\":[\n");
}
void Perf_writeJsonEntry(u32 start, u32 stop, const char *name, u32 thread_id) {
    const u32 dur = stop - start;

    OSReport(
            "$T {\"cat\":\"function\","
            "\"dur\":%u,"
            "\"name\":\"%s\","
            "\"ph\":\"X\","
            "\"pid\":\"0\","
            "\"tid\":\"%u\","
            "\"ts\":%u},\n",
            dur, name, thread_id, start);
}
void Perf_writeJsonFooter() {
    OSReport("$T ]}\n");
}
