#include "Stack.h"

#include <revolution/os.h>
#include <sp/Version.h>

typedef bool (*FindFunction)(const u32* start_address);
typedef void (*LinkRegisterFunction)(void);

typedef struct LinkRegisterPatch
{
    FindFunction find_prologue_function;
    FindFunction find_lr_save_function;
    FindFunction find_lr_restore_function;
    FindFunction find_epilogue_function;

    size_t prologue_instruction_count;
    size_t lr_save_instruction_count;
    size_t lr_restore_instruction_count;
    size_t epilogue_instruction_count;

    LinkRegisterFunction new_lr_save_function;
    LinkRegisterFunction new_lr_restore_function;
} LinkRegisterPatch;

extern void Stack_XORAlignedLinkRegisterSave(void);
extern void Stack_XORAlignedLinkRegisterRestore(void);
extern void Stack_XORLinkRegisterSave(void);
extern void Stack_XORLinkRegisterRestore(void);

static const u32 blr  = 0x4E800020;
static const u32 mflr = 0x7C0802A6;
static const u32 mtlr = 0x7C0803A6;

u32 __stack_chk_guard;

void Stack_InitCanary(void)
{
    __stack_chk_guard = (OSGetTick() & 0x00FFFFFF) | (0x80 << 24);
}

__attribute__((noreturn)) void __stack_chk_fail(void)
{
    const GXColor background = { 0xFF, 0x00, 0x00, 0xFF };
    const GXColor foreground = { 0xFF, 0xFF, 0xFF, 0xFF };

    OSFatal(foreground, background, "MKW-SP v" BUILD_TYPE_STR "\n\n" "Stack smashing detected !");
    __builtin_unreachable();
}

static bool Stack_IsAlignedFunctionPrologue(const u32* start_address)
{
    const u32 stwux = 0x7C21596E;

    return start_address[0] == stwux && start_address[1] == mflr;
}

static bool Stack_IsAlignedLinkRegisterSaveInstruction(const u32* start_address)
{
    const u32 stw = 0x900C0004;

    return start_address[0] == stw;
}

static bool Stack_IsAlignedLinkRegisterRestoreInstruction(const u32* start_address)
{
    const u32 lwz = 0x800A0004;

    return start_address[0] == lwz;
}

static bool Stack_IsAlignedFunctionEpilogue(const u32* start_address)
{
    const u32 mr = 0x7D415378;

    return start_address[0] == mtlr && start_address[1] == mr && start_address[2] == blr;
}

static bool Stack_IsFunctionPrologue(const u32* start_address)
{
    const u16 stwu = 0x9421;

    return (start_address[0] >> 16) == stwu && start_address[1] == mflr;
}

static bool Stack_IsLinkRegisterSaveInstruction(const u32* start_address)
{
    const u16 stw = 0x9001;

    return (start_address[0] >> 16) == stw;
}

static bool Stack_IsLinkRegisterRestoreInstruction(const u32* start_address)
{
    const u16 lwz = 0x8001;

    return (start_address[0] >> 16) == lwz;
}

static bool Stack_IsFunctionEpilogue(const u32* start_address)
{
    const u16 addi = 0x3821;

    return start_address[0] == mtlr && (start_address[1] >> 16) == addi && start_address[2] == blr;
}

static const LinkRegisterPatch lr_patches[2] =
{
    {
        .find_prologue_function = Stack_IsFunctionPrologue,
        .find_lr_save_function = Stack_IsLinkRegisterSaveInstruction,
        .find_lr_restore_function = Stack_IsLinkRegisterRestoreInstruction,
        .find_epilogue_function = Stack_IsFunctionEpilogue,

        .prologue_instruction_count = 2,
        .lr_save_instruction_count = 1,
        .lr_restore_instruction_count = 1,
        .epilogue_instruction_count = 3,

        .new_lr_save_function = Stack_XORLinkRegisterSave,
        .new_lr_restore_function = Stack_XORLinkRegisterRestore,
    },
    {
        .find_prologue_function = Stack_IsAlignedFunctionPrologue,
        .find_lr_save_function = Stack_IsAlignedLinkRegisterSaveInstruction,
        .find_lr_restore_function = Stack_IsAlignedLinkRegisterRestoreInstruction,
        .find_epilogue_function = Stack_IsAlignedFunctionEpilogue,

        .prologue_instruction_count = 2,
        .lr_save_instruction_count = 1,
        .lr_restore_instruction_count = 1,
        .epilogue_instruction_count = 3,

        .new_lr_save_function = Stack_XORAlignedLinkRegisterSave,
        .new_lr_restore_function = Stack_XORAlignedLinkRegisterRestore
    }
};

static u32* Stack_FindFirstFunction(u32* start_address, u32* end_address, FindFunction find_function, size_t instruction_count)
{
    while (start_address + instruction_count <= end_address)
    {
        if (find_function(start_address))
            return start_address;

        start_address++;
    }

    return NULL;
}

static u32* Stack_FindLastFunction(u32* start_address, u32* end_address, FindFunction find_function, size_t instruction_count)
{
    while (end_address - instruction_count >= start_address)
    {
        if (find_function(end_address - instruction_count))
            return end_address - instruction_count;

        end_address--;
    }

    return NULL;
}

static u32 Stack_CreateBranchLinkInstruction(const u32* source_address, const u32* destination_address)
{
    return (18 << 26) | (((u32)destination_address - (u32)source_address) & 0x3FFFFFC) | (1 << 0);
}

void Stack_DoLinkRegisterPatches(u32* start, u32* end)
{
    assert(((u32)start & 3) == 0);
    assert(((u32)end & 3) == 0);

    for (size_t n = 0; n < ARRAY_SIZE(lr_patches); n++)
    {
        u32* start_address = start;
        u32* end_address = end;
        const LinkRegisterPatch* lr_patch = &lr_patches[n];

        while (start_address < end_address)
        {
            u32* prologue = Stack_FindFirstFunction(start_address, end_address, lr_patch->find_prologue_function, lr_patch->prologue_instruction_count);
            if (!prologue)
                break;

            u32* epilogue = Stack_FindFirstFunction(prologue + lr_patch->prologue_instruction_count, end_address, lr_patch->find_epilogue_function, lr_patch->epilogue_instruction_count);
            if (!epilogue)
                break;

            u32* lr_save_instruction = Stack_FindFirstFunction(prologue + lr_patch->prologue_instruction_count, epilogue, lr_patch->find_lr_save_function, lr_patch->lr_save_instruction_count);
            if (!lr_save_instruction)
                goto label_check_next_function;

            u32* lr_restore_instruction = Stack_FindLastFunction(lr_save_instruction + lr_patch->lr_save_instruction_count, epilogue, lr_patch->find_lr_restore_function, lr_patch->lr_restore_instruction_count);
            if (!lr_restore_instruction)
                goto label_check_next_function;

            // Skip over functions that do not return
            u32* next_prologue = Stack_FindFirstFunction(prologue + lr_patch->prologue_instruction_count, end_address, lr_patch->find_prologue_function, lr_patch->prologue_instruction_count);
            if (next_prologue && !(next_prologue > epilogue))
            {
                start_address = next_prologue;
                continue;
            }

            *lr_save_instruction = Stack_CreateBranchLinkInstruction(lr_save_instruction, (u32*)lr_patch->new_lr_save_function);
            *lr_restore_instruction = Stack_CreateBranchLinkInstruction(lr_restore_instruction, (u32*)lr_patch->new_lr_restore_function);

            DCFlushRange(lr_save_instruction, sizeof(*lr_save_instruction));
            DCFlushRange(lr_restore_instruction, sizeof(*lr_restore_instruction));
            ICInvalidateRange(lr_save_instruction, sizeof(*lr_save_instruction));
            ICInvalidateRange(lr_restore_instruction, sizeof(*lr_restore_instruction));

label_check_next_function:
            start_address = epilogue + lr_patch->epilogue_instruction_count;
        }
    }
}
