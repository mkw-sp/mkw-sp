#include "Stack.h"

#include <revolution/os.h>
#include <sp/Version.h>

typedef bool (*FindFunction)(const u32* start_address, const u32* end_address);

extern void Stack_XORLinkRegisterRestore(void);
extern void Stack_XORLinkRegisterSave(void);

u32 __stack_chk_guard;

void Stack_InitCanary(void)
{
    __stack_chk_guard = OSGetTick() & 0x00FFFFFF;
}

__attribute__((noreturn)) void __stack_chk_fail(void)
{
    const GXColor background = { 0xFF, 0x00, 0x00, 0xFF };
    const GXColor foreground = { 0xFF, 0xFF, 0xFF, 0xFF };

    OSFatal(foreground, background, "MKW-SP v" BUILD_TYPE_STR "\n\n" "Stack smashing detected !");
    __builtin_unreachable();
}

static u32 Stack_CreateBranchLinkInstruction(const u32* source_address, const u32* destination_address)
{
    return (18 << 26) | (((u32)destination_address - (u32)source_address) & 0x3FFFFFC) | (1 << 0);
}

static u32* Stack_FindLastFunction(u32* start_address, u32* end_address, FindFunction find)
{
    while (end_address > start_address)
    {
        end_address--;

        if (find(start_address, end_address))
            return end_address;
    }

    return NULL;
}

static u32* Stack_FindNextFunction(u32* start_address, u32* end_address, FindFunction find)
{
    while (start_address < end_address)
    {
        if (find(start_address, end_address))
            return start_address;

        start_address++;
    }

    return NULL;
}

static inline bool Stack_IsFunctionEpilogue(const u32* start_address, const u32* end_address)
{
    const u32 mtlr = 0x7C0803A6;
    const u16 addi = 0x3821;
    const u32 blr  = 0x4E800020;

    return (end_address - start_address) >= 3 && start_address[0] == mtlr && ((start_address[1] >> 16) == addi) && start_address[2] == blr;
}

static inline bool Stack_IsFunctionPrologue(const u32* start_address, const u32* end_address)
{
    const u16 stwu = 0x9421;
    const u32 mflr = 0x7C0802A6;

    return (end_address - start_address) >= 2 && ((start_address[0] >> 16) == stwu) && start_address[1] == mflr;
}

static inline bool Stack_IsLinkRegisterRestoreInstruction(const u32* start_address, const u32* end_address)
{
    const u16 lr_restore_instruction_high = 0x8001;

    return (end_address - start_address) >= 1 && (end_address[0] >> 16) == lr_restore_instruction_high;
}

static inline bool Stack_IsLinkRegisterSaveInstruction(const u32* start_address, const u32* end_address)
{
    const u16 lr_save_instruction_high = 0x9001;

    return (end_address - start_address) >= 1 && (start_address[0] >> 16) == lr_save_instruction_high;
}

void Stack_DoLinkRegisterPatches(u32* start_address, u32* end_address)
{
    assert(((u32)start_address & 3) == 0);
    assert(((u32)end_address & 3) == 0);

    while (start_address < end_address)
    {
        /*
         * stwu r1, -0xXXXX(r1)
         * mflr r0
         */
        u32* stwu = Stack_FindNextFunction(start_address, end_address, Stack_IsFunctionPrologue);
        if (!stwu)
            break;

        /*
         * mtlr r0
         * addi r1, r1, 0xXXXX(r1)
         * blr
         */
        u32* mtlr = Stack_FindNextFunction(stwu + 2, end_address, Stack_IsFunctionEpilogue);
        if (!mtlr)
            break;

        // stw r0, 0xXXXX(r1)
        u32* stw = Stack_FindNextFunction(stwu + 2, mtlr, Stack_IsLinkRegisterSaveInstruction);
        if (!stw)
            goto label_check_next_function;

        // lwz r0, 0xXXXX(r1)
        u32* lwz = Stack_FindLastFunction(stw, mtlr, Stack_IsLinkRegisterRestoreInstruction);
        if (!lwz)
            goto label_check_next_function;

        // Verify that the instructions are in the expected order
        if (!(stw < lwz))
            goto label_check_next_function;

        // If the function does not return, skip over it
        u32* next_stwu = Stack_FindNextFunction(stwu + 2, end_address, Stack_IsFunctionPrologue);
        if (next_stwu && !(next_stwu > mtlr))
        {
            start_address = next_stwu;
            continue;
        }

        *stw = Stack_CreateBranchLinkInstruction(stw, (u32*)Stack_XORLinkRegisterSave);
        *lwz = Stack_CreateBranchLinkInstruction(lwz, (u32*)Stack_XORLinkRegisterRestore);

        DCFlushRange(stw, sizeof(stw));
        DCFlushRange(lwz, sizeof(lwz));
        ICInvalidateRange(stw, sizeof(stw));
        ICInvalidateRange(lwz, sizeof(lwz));

label_check_next_function:
        start_address = mtlr + 3;
    }
}
