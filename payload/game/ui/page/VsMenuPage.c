#include "../../system/RaceConfig.h"
#include "../../system/SaveManager.h"

void VsMenuPage_applyClassRule(u32 vsRuleClass) {
    RaceConfigScenario *menuScenario = &s_raceConfig->menuScenario;
    switch (vsRuleClass) {
    case VS_RULE_CLASS_100CC:
        menuScenario->engineClass = ENGINE_CLASS_100CC;
        menuScenario->modeFlags &= ~MODE_FLAG_MIRROR;
        vsSpeedModIsEnabled = false;
        break;
    case VS_RULE_CLASS_150CC:
        menuScenario->engineClass = ENGINE_CLASS_150CC;
        menuScenario->modeFlags &= ~MODE_FLAG_MIRROR;
        vsSpeedModIsEnabled = false;
        break;
    case VS_RULE_CLASS_MIRROR:
        menuScenario->engineClass = ENGINE_CLASS_150CC;
        menuScenario->modeFlags |= MODE_FLAG_MIRROR;
        vsSpeedModIsEnabled = false;
        break;
    case VS_RULE_CLASS_200CC:
        menuScenario->engineClass = ENGINE_CLASS_150CC;
        menuScenario->modeFlags &= ~MODE_FLAG_MIRROR;
        vsSpeedModIsEnabled = true;
        break;
    }
}
