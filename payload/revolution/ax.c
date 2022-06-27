#include "ax.h"

bool axIsMuted = false;

void AXSetMasterVolume(u16 volume) {
    if (axIsMuted) {
        volume = 0;
    }
    REPLACED(AXSetMasterVolume)(volume);
}
