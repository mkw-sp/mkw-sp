#include "eggThread.hh"

namespace EGG {

void Thread::switchThreadCallback(OSThread *previousThread, OSThread *nextThread) {
    if (previousThread && *previousThread->stackTop != OS_THREAD_STACK_TOP_MAGIC) {
        panic("Stack overflow detected!");
    }

    REPLACED(switchThreadCallback)(previousThread, nextThread);
}

} // namespace EGG
