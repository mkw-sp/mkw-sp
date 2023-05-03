#include <Common.S>

#define EXCEPTION_INFO_FLAG_NONE     (0 << 0)
#define EXCEPTION_INFO_FLAG_MAIN     (1 << 0)
#define EXCEPTION_INFO_FLAG_GPR      (1 << 1)
#define EXCEPTION_INFO_FLAG_GPR_MAP  (1 << 2)
#define EXCEPTION_INFO_FLAG_SRR0_MAP (1 << 3)
#define EXCEPTION_INFO_FLAG_FPR      (1 << 4)
#define EXCEPTION_INFO_FLAG_TRACE    (1 << 5)

#define EXCEPTION_INFO_FLAG_DEFAULT  EXCEPTION_INFO_FLAG_MAIN | EXCEPTION_INFO_FLAG_TRACE
#define EXCEPTION_INFO_FLAG_ALL      EXCEPTION_INFO_FLAG_MAIN | EXCEPTION_INFO_FLAG_GPR | \
                                     EXCEPTION_INFO_FLAG_GPR_MAP | EXCEPTION_INFO_FLAG_SRR0_MAP | \
                                     EXCEPTION_INFO_FLAG_FPR | EXCEPTION_INFO_FLAG_TRACE

// Display more information when an exception occurs
PATCH_REPLACE_START(nw4r_db_Exception_Init, 0x34)
    li r0, EXCEPTION_INFO_FLAG_ALL
PATCH_REPLACE_END(nw4r_db_Exception_Init, 0x34)
