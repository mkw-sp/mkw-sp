#include <Common.S>

#define EXCEPTION_INFO_NONE    (0 << 0)
#define EXCEPTION_INFO_MAIN    (1 << 0)
#define EXCEPTION_INFO_GPR     (1 << 1)
#define EXCEPTION_INFO_GPRMAP  (1 << 2)
#define EXCEPTION_INFO_SRR0MAP (1 << 3)
#define EXCEPTION_INFO_FPR     (1 << 4)
#define EXCEPTION_INFO_TRACE   (1 << 5)

#define EXCEPTION_INFO_DEFAULT EXCEPTION_INFO_MAIN | EXCEPTION_INFO_TRACE
#define EXCEPTION_INFO_ALL     EXCEPTION_INFO_MAIN | EXCEPTION_INFO_GPR | EXCEPTION_INFO_GPRMAP | \
                               EXCEPTION_INFO_SRR0MAP | EXCEPTION_INFO_FPR | EXCEPTION_INFO_TRACE

// Display more information when an exception occurs
PATCH_REPLACE_START(nw4r_db_Exception_Init, 0x34)
    li r0, EXCEPTION_INFO_ALL
PATCH_REPLACE_END(nw4r_db_Exception_Init, 0x34)
