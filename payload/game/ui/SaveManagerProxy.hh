#include <Common.hh>

namespace UI {

class SaveManagerProxy {
public:
    void REPLACED(markLicensesDirty)();
    REPLACE void markLicensesDirty();

    bool hasPendingRequests() const;

private:
    u8 _000[0x016 - 0x000];
    bool m_licensesDirty;
    u8 _017[0x128 - 0x017];
};
static_assert(sizeof(SaveManagerProxy) == 0x128);

} // namespace UI
