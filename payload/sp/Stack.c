#include "Stack.h"
#include "Version.h"

#include <revolution/os.h>

typedef bool (*FindFunction)(const u32* start_address);

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

static u32* Stack_FindFirstFunction(u32* start_address, u32* end_address, FindFunction find, size_t find_instruction_count)
{
    while (start_address + find_instruction_count <= end_address)
    {
        if (find(start_address))
            return start_address;

        start_address++;
    }

    return NULL;
}

static u32* Stack_FindLastFunction(u32* start_address, u32* end_address, FindFunction find, size_t find_instruction_count)
{
    while (end_address - find_instruction_count >= start_address)
    {
        if (find(end_address - find_instruction_count))
            return end_address - find_instruction_count;

        end_address--;
    }

    return NULL;
}

static bool Stack_IsFunctionEpilogue(const u32* start_address)
{
    const u32 mtlr = 0x7C0803A6;
    const u16 addi = 0x3821;
    const u32 blr  = 0x4E800020;

    return start_address[0] == mtlr && ((start_address[1] >> 16) == addi) && start_address[2] == blr;
}

static bool Stack_IsFunctionPrologue(const u32* start_address)
{
    const u16 stwu = 0x9421;
    const u32 mflr = 0x7C0802A6;

    return ((start_address[0] >> 16) == stwu) && start_address[1] == mflr;
}

static bool Stack_IsLinkRegisterRestoreInstruction(const u32* start_address)
{
    const u16 lr_restore_instruction_high = 0x8001;

    return (start_address[0] >> 16) == lr_restore_instruction_high;
}

static bool Stack_IsLinkRegisterSaveInstruction(const u32* start_address)
{
    const u16 lr_save_instruction_high = 0x9001;

    return (start_address[0] >> 16) == lr_save_instruction_high;
}

void Stack_DoLinkRegisterPatches(u32* start_address, u32* end_address)
{
    assert(((u32)start_address & 3) == 0);
    assert(((u32)end_address & 3) == 0);

    while (start_address < end_address)
    {
        u32* stwu = Stack_FindFirstFunction(start_address, end_address, Stack_IsFunctionPrologue, 2);
        if (!stwu)
            break;

        u32* mtlr = Stack_FindFirstFunction(stwu + 2, end_address, Stack_IsFunctionEpilogue, 3);
        if (!mtlr)
            break;

        u32* stw = Stack_FindFirstFunction(stwu + 2, mtlr, Stack_IsLinkRegisterSaveInstruction, 1);
        if (!stw)
            goto label_check_next_function;

        u32* lwz = Stack_FindLastFunction(stw + 1, mtlr, Stack_IsLinkRegisterRestoreInstruction, 1);
        if (!lwz)
            goto label_check_next_function;

        // Skip over functions that do not return
        u32* next_stwu = Stack_FindFirstFunction(stwu + 2, end_address, Stack_IsFunctionPrologue, 2);
        if (next_stwu && !(next_stwu > mtlr))
        {
            start_address = next_stwu;
            continue;
        }

        *stw = Stack_CreateBranchLinkInstruction(stw, (u32*)Stack_XORLinkRegisterSave);
        *lwz = Stack_CreateBranchLinkInstruction(lwz, (u32*)Stack_XORLinkRegisterRestore);

        DCFlushRange(stw, sizeof(*stw));
        DCFlushRange(lwz, sizeof(*lwz));
        ICInvalidateRange(stw, sizeof(*stw));
        ICInvalidateRange(lwz, sizeof(*lwz));

label_check_next_function:
        start_address = mtlr + 3;
    }
}
