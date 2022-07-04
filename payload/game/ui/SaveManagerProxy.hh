#include <Common.hh>

namespace UI {

class SaveManagerProxy {
public:
    void markLicensesDirty();

private:
    u8 _000[0x128 - 0x000];
};
static_assert(sizeof(SaveManagerProxy) == 0x128);

} // namespace UI
