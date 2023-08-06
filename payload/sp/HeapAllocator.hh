#pragma once

#include <memory>

#include <egg/core/eggHeap.hh>

namespace SP {

template <typename _Tp>
class HeapAllocator : public std::allocator<_Tp> {
public:
    HeapAllocator(EGG::Heap *heap) {
        m_heap = heap;
    };

    _Tp *allocate(size_t n, const void *hint = 0) {
        // Always align to 0x20, instead of the type's alignment, for reasons.
        return reinterpret_cast<_Tp *>(m_heap->alloc(n, 0x20));
    }

    void deallocate(_Tp *block, size_t /*n*/) {
        return m_heap->free(block);
    }

    EGG::Heap *m_heap;
};

} // namespace SP
