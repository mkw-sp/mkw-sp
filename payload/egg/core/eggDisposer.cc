#include "eggDisposer.hh"
#include "eggHeap.hh"

namespace EGG {

Disposer::Disposer() {
    m_containHeap = Heap::findContainHeap(this);

    if (m_containHeap != nullptr) {
        m_containHeap->appendDisposer(this);
    }
}

Disposer::~Disposer() {
    if (m_containHeap != nullptr) {
        m_containHeap->removeDisposer(this);
    }
}

void Disposer::dt(s32 type) {
    if (type > 0) {
        delete this;
    } else {
        this->~Disposer();
    }
}

Disposer::eLifetime Disposer::lifetime() const {
    return m_containHeap != nullptr ? eLifetime::HeapGC : eLifetime::Unmanaged;
}

Heap *Disposer::containHeap() {
    return m_containHeap;
}

} // namespace EGG
