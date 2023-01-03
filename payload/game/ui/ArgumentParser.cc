#include "ArgumentParser.hh"

#include "game/ui/SectionManager.hh"

#include <sp/settings/GlobalSettings.hh>

namespace UI {

SectionId ArgumentParser::parse() {
    u32 sectionId = SP::GlobalSettings::Get<SP::GlobalSettings::Setting::BootSection>();
    if (sectionId >= static_cast<u32>(SectionId::Max)) {
        return SectionId::TitleFromBoot;
    }

    SectionManager::Instance()->registeredPadManager().setFlags(0, 0x124 /* 1st GCN controller */);

    return static_cast<SectionId>(sectionId);
}

} // namespace UI
