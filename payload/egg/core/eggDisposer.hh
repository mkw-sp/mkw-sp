/**
 * @file
 * @brief Headers for EGG::Disposer.
 */
#pragma once

#include <nw4r/ut/ut_list.hh>

namespace EGG {

class Heap;

//! @brief Base class for garbage-collected objects.
//!
//! Scene-specific, heap-allocated resources in mkw are typically not explicitly
//! freed. This isn't a memory leak, though: when a Scene transitions, all
//! memory allocated within it is returned in one block. However, in C++, the
//! destructor of an object must be called when its lifetime
//! expires--otherwise, we could leak second-order resources and introduce
//! use-after-free bugs.
//!
//! Disposer solves this issue: when a heap is destroyed,
//! the linked list of dispoers is traversed and each destructor is called
//! in-order.
//!
class Disposer {
    friend class Heap;

protected:
    //! @brief Disposer constructor: attach to heap.
    //! @pre   Lifetime management is assumed if, and only if, the object is
    //! allocated within an EGG::Heap.
    //!
    Disposer();

    //! @brief Disposer destructor: detach from heap.
    //!
    virtual ~Disposer();

    // NonCopyable, NonMovable
    Disposer(const Disposer &) = delete;
    Disposer(Disposer &&) noexcept = delete; // Not implemented

public:
    //! Describes the lifetime of a derived object.
    enum class eLifetime {
        //! This object is not allocated in an EGG::Heap; it acts as a regular C++
        //! object.
        Unmanaged,

        //! This object is garbage-collected; its lifetime is tied to its heap.
        HeapGC,
    };

    //! @brief Inspect the lifetime of this object.
    eLifetime lifetime() const;

    //! @brief Get the heap owning this object (or nullptr if none)
    Heap *containHeap();

private:
    virtual void dt(s32 type);

    // If null, Disposer is no-op.
    Heap *m_containHeap;
    // Otherwise, each Disposer object exists in a linked list owned by
    // mContainHeap.
    nw4r::ut::Node m_link;
};

} // namespace EGG
