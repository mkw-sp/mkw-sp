#include "SaveManagerProxy.hh"

namespace UI {

void SaveManagerProxy::markLicensesDirty() {}

bool SaveManagerProxy::savingDisabled() const {
    return m_disableSaving;
}

} // namespace UI
