#pragma once

namespace EGG {

class Heap {
public:
    virtual void vf_00() = 0;
    virtual void vf_04() = 0;
    virtual void vf_08() = 0;
    virtual void vf_0c() = 0;
    virtual void vf_10() = 0;
    virtual void *alloc(u32 size, s32 align) = 0;
    virtual void *free(void *block) = 0;
    virtual void vf_1c() = 0;
    virtual void vf_20() = 0;
    virtual void vf_24() = 0;
    virtual void vf_28() = 0;

private:
    u8 _04[0x38 - 0x04];
};
static_assert(sizeof(Heap) == 0x38);

} // namespace EGG

void *operator new(size_t size, EGG::Heap *heap, int align);

void *operator new[](size_t size, EGG::Heap *heap, int align);
