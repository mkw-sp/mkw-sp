#include "GhostWriter.hh"
#include "RaceConfig.hh"
#include "InputManager.hh"

extern "C" {
    #include "revolution.h"
}

namespace System {

void GhostWriter::writeFrame(u16 rawButtons, u8 rawStickX, u8 rawStickY, u32 rawTrick) {
    u8 neutral = 0x07;
    auto *rc = System::RaceConfig::Instance();
    u8 newStickX = rawStickX;
    u8 newTrick = rawTrick;
    // Flips the inputs of a ghost when the current mode is mirror
    if (rc->raceScenario().mirror) {
        if (rawStickX < neutral) {
            newStickX = neutral + (neutral - rawStickX);
        }
        else if (rawStickX > neutral) {
            newStickX = neutral - (rawStickX - neutral);
        }
        if (rawTrick == System::Trick::Left) {
            newTrick = System::Trick::Right;
        }
        else if (rawTrick == System::Trick::Right) {
            newTrick = System::Trick::Left;
        }
    } 
    REPLACED(writeFrame)(rawButtons, newStickX, rawStickY, newTrick);
}

}