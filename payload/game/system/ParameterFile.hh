#include <egg/core/eggHeap.hh>

namespace System {

class ParameterFile {
public:
    ParameterFile();
    virtual ~ParameterFile();
    virtual void dt(s32 type);
    REPLACE virtual void read(EGG::Heap *heap);

    REPLACE void append(u32 r4, u32 r5, u32 r6);
};

} // namespace System
