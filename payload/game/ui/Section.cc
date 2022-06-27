#include "Section.hh"

#include "game/ui/ChannelPage.hh"
#include "game/ui/LicenseSelectPage.hh"
#include "game/ui/ServicePackTopPage.hh"
#include "game/ui/TimeAttackGhostListPage.hh"
#include "game/ui/UpdatePage.hh"

namespace UI {

Page *Section::createPage(PageId pageId) {
    switch (pageId) {
    case PageId::LicenseSelect:
        return new LicenseSelectPage;
    case PageId::TimeAttackGhostList:
        return new TimeAttackGhostListPage;
    case PageId::ServicePackTop:
        return new ServicePackTopPage;
    case PageId::Channel:
        return new ChannelPage;
    case PageId::Update:
        return new UpdatePage;
    default:
        return REPLACED(createPage)(pageId);
    }
}

} // namespace UI
