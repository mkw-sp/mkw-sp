#pragma once

#include "eggHeap.hh"

namespace EGG {

class ExpHeap : public Heap {
public:
    /// The `GroupSizeRecord` class is used for tracking memory allocations for different allocation
    /// groups. It serves as a memory budgeting system that assists in debugging out-of-memory (OOM)
    /// crashes by recording how memory is allocated across different groups. This approach allows
    /// precise tracking of memory usage and can help identify where the memory budget is being
    /// exceeded.
    ///
    /// The class uses an array `entries[256]` where each entry corresponds to a specific allocation
    /// group. The groups are defined in the `RKHeapTag` enum. Each time an allocation is made using
    /// the `GameScene::setHeapTag` function, the corresponding entry in `GroupSizeRecord` is
    /// updated to reflect the new total memory usage of that group.
    ///
    /// This approach is especially useful in development where it is common to set specific budget
    /// caps for different allocation groups and generate debug assertions when these caps are
    /// exceeded. The system can then be manually adjusted until the release version, where this
    /// system may not be used. This pattern is frequently seen in low-level game engine
    /// development.
    ///
    /// The `GroupSizeRecord()` is a constructor for the class.
    /// The `reset()` method sets all entries in the `entries` array back to zero.
    /// The `addSize(u16 groupID, u32 size)` method adds the provided `size` to the group indicated
    /// by `groupID`.
    ///
    struct GroupSizeRecord {
        u32 entries[256];

        GroupSizeRecord();
        void reset();
        void addSize(u16 groupID, u32 size);
    };

    static ExpHeap *Create(void *block, u32 size, u16 attr);
    static ExpHeap *Create(u32 size, Heap *heap, u16 attr);

    void initAllocator(void *allocator, s32 align) override;
    void destroy() override;
};

} // namespace EGG
