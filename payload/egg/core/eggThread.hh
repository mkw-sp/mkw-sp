#pragma once

extern "C" {
#include <revolution/os/OSMessage.h>
#include <revolution/os/OSThread.h>
}

#include <nw4r/ut/ut_list.hh>

#include <egg/core/eggHeap.hh>

namespace EGG {

class Thread {
public:
    Thread(const Thread &) = delete;
    virtual ~Thread();
    virtual void dt(int);
    virtual void *run();
    virtual void onEnter();
    virtual void onExit();
    Thread *ct(u32 stackSize, int msgCount, int prio, Heap *heap);

    Thread(OSThread *osThread, int msgCount);
    static Thread *findThread(OSThread *osThread);
    static void kandoTestCancelAllThread();
    static void initialize();
    static void REPLACED(switchThreadCallback)(OSThread *previousThread, OSThread *nextThread);
    static REPLACE void switchThreadCallback(OSThread *previousThread, OSThread *nextThread);
    void setCommonMesgQueue(int msgCount, Heap *heap);
    static void *start(void *eggThread);
    OSThread *getOSThread() {
        return m_OSThread;
    }
    //! When not NULL will override the heap used for allocations.
    Heap *getAllocatableHeap() {
        return m_alloctableHeap;
    }
    void resume() {
        OSResumeThread(m_OSThread);
    }

private:
    // List of all registered threads.
    static nw4r::ut::List s_threadList;

    Heap *m_containingHeap; //!< [+0x04] Heap for thread-specific allocations
    OSThread *m_OSThread;   //!< [+0x08]

    OSMessageQueue m_mesgQueue; //!< [+0x0C]
    OSMessage *m_mesgBuffer;    //!< [+0x2C]
    int m_mesgCount;            //!< [+0x30]

    void *m_stackMemory; //!< [+0x34] The base (*start*) of the stack.
    u32 m_stackSize;     //!< [+0x38] The size of the stack.

    Heap *m_alloctableHeap; //!< [+0x3C] When not NULL will override the heap used
                            //!< for allocations. Loses to |Heap::s_allocatableHeap| in precedence.
    nw4r::ut::Node m_link;  //!< [+0x40] Entry of |s_threadList|
};

} // namespace EGG
