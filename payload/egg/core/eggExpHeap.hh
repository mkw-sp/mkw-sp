#include "eggHeap.hh"

namespace EGG {

class ExpHeap : public Heap {
public:
    static ExpHeap *Create(void *block, u32 size, u16 attr);
    static ExpHeap *Create(u32 size, Heap *heap, u16 attr);

    void initAllocator(void *allocator, s32 align) override;
    void destroy() override;
};

} // namespace EGG
