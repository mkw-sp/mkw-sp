#include <revolution/os.h>

extern void DisableInstsOnMEM1Hi16MB(void);

void RealMode(void (*function)(void));

void OSDisableCodeExecOnMEM1Hi16MB(void)
{
    BOOL enabled = OSDisableInterrupts();
    {
        RealMode(DisableInstsOnMEM1Hi16MB);
    }
    OSRestoreInterrupts(enabled);
}
