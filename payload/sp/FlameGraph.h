#pragma once

//
// Helper for generating performance flame graphs
//

#include <Common.h>
#include <revolution.h>

// Branchless OSGetTick()
#define PERF_MFTB() \
    ({ \
        volatile u32 _rval; \
        asm volatile("mftb %0" : "=r"(_rval)); \
        _rval; \
    })

// OSGetTick()
inline u32 getTick() {
    return PERF_MFTB();
}

#undef PERF_MFTB

inline u32 getThreadId(void) {
    OSThread *pThread = OSGetCurrentThread();

    // TODO: Get a more sensible ID

    return (u32)pThread;
}

typedef struct TimedContext {
    u32 startTick;
    u32 stopTick;
} TimedContext;

inline void TimedContext_start(TimedContext *ctx) {
    ctx->startTick = getTick();
}

inline void TimedContext_stop(TimedContext *ctx) {
    ctx->stopTick = getTick();
}

void Perf_writeJsonHeader();
void Perf_writeJsonEntry(u32 start, u32 stop, const char *name, u32 thread_id);
void Perf_writeJsonFooter();

typedef struct SimplePerf {
    TimedContext ctx;
    const char *funcName;
} SimplePerf;

inline void SimplePerf_CT(SimplePerf *perf, const char *funcName) {
    TimedContext_start(&perf->ctx);
    perf->funcName = funcName;
}

inline void SimplePerf_DT(SimplePerf *perf) {
    TimedContext_stop(&perf->ctx);
    // There is some penalty for printing, but it doesn't factor into the start/stop time.
    Perf_writeJsonEntry(perf->ctx.startTick, perf->ctx.stopTick, perf->funcName, getThreadId());
}

// C RAII via cleanup attribute
#define SIMPLE_PERF_NAMED(name) \
    SimplePerf perf __attribute__((cleanup(SimplePerf_DT))); \
    SimplePerf_CT(&perf, name)

#define SIMPLE_PERF SIMPLE_PERF_NAMED(__FUNCTION__)
