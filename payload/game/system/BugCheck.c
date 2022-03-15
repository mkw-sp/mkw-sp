#include "BugCheck.h"
#include "FatalScene.h"

#include <revolution.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <egg/core/eggSystem.h>
#include <sp/WideUtil.h>

// Referenced by SceneCreatorDyanmic.S
bool sBugCheckSet = false;

static char sContextFile[64];   // beginner_course.szs
static char sBugCheckFile[64];  // course_model.brres
static char sBugCheckDescription[256];

static wchar_t sFormattedBugCheck[512];

void SetContextForBugCheck(const char *context) {
    strncpy(sContextFile, context, sizeof(sContextFile) - 1);
}

const wchar_t *GetFormattedBugCheck(void) {
    return sFormattedBugCheck;
}

static void SpFormatBugCheck() {
    char tmp[512];
    memset(tmp, 0, sizeof(tmp));

    snprintf(tmp, sizeof(tmp),
            // "File: %s\n"
            "Subfile: %s\n"
            "Error: %s\n",
            // sContextFile[0] != '\0' ? sContextFile : "?",
            sBugCheckFile[0] != '\0' ? sBugCheckFile : "?",
            sBugCheckDescription[0] != '\0' ? sBugCheckDescription : "?");

    Util_toUtf16(sFormattedBugCheck, ARRAY_SIZE(sFormattedBugCheck), tmp, sizeof(tmp));
    sFormattedBugCheck[ARRAY_SIZE(sFormattedBugCheck) - 1] = (wchar_t)'\0';
}

static void SpSaveBugCheck(const char *file, const char *description) {
    strncpy(sBugCheckFile, file, sizeof(sBugCheckFile) - 1);
    strncpy(sBugCheckDescription, description, sizeof(sBugCheckDescription) - 1);
    sBugCheckSet = true;
}

void SpBugCheckAsync(const char *file, const char *description) {
    OSReport("------------------------------\n");
    OSReport("BUGCHECK (ASYNC)\n");
    OSReport("FILE: %s\n", file);
    OSReport("DESC: %s\n", description);
    OSReport("------------------------------\n");

    // Only accept the first bugcheck
    if (sBugCheckSet)
        return;

    SpSaveBugCheck(file, description);
    SpFormatBugCheck();
}

void SpBugCheck(const char *file, const char *description) {
    OSReport("------------------------------\n");
    OSReport("BUGCHECK\n");
    OSReport("FILE: %s\n", file);
    OSReport("DESC: %s\n", description);
    OSReport("------------------------------\n");

    if (!sBugCheckSet)
        SpSaveBugCheck(file, description);
    SpFormatBugCheck();

    static FatalScene fScene;
    FatalScene_CT(&fScene);

    // We can't safely unload the current scene, so just use its memory.
    FatalScene_LeechCurrentScene(&fScene);

    fScene.vt->enter(&fScene);

    FatalScene_SetBody(&fScene, sFormattedBugCheck);

    FatalScene_MainLoop(&fScene);

    __builtin_unreachable();
}

void SpBugCheckFmt(const char *file, const char *desc, ...) {
    va_list v;
    va_start(v, desc);
    char buf[256];
    vsnprintf(buf, sizeof(buf), desc, v);
    va_end(v);
    SpBugCheck(file, buf);
}

//
// Individual crashes
//

// XREF: eggG3dUtil.S
void InvalidTexRefFail(const char *mdl0_name) {
    char file[64];
    snprintf(file, sizeof(file), "%s.mdl0", mdl0_name);
    char desc[256];
    snprintf(desc, sizeof(desc),
            "Invalid texture references.\n\nTo regenerate references, delete the "
            "folder "
            "\n"
            "3DModels(NW4R)/[MDL0] \"%s\"/Textures.",
            mdl0_name);
    SpBugCheck(file, desc);
}

// XREF: eggEffectCreator.S
void MissingBreffFail(const char *breff_name) {
    char file[65];
    snprintf(file, sizeof(file), "%s.breff", breff_name);
    SpBugCheck(file, "BREFF (particle effect) file is missing");
}

// XREF: g3dResFile.S
void InvalidRevisionFail(
        int bad_version, unsigned int res_type, int index, const char *brres_name) {
    char file[64];

    if (brres_name && *brres_name)
        snprintf(file, sizeof(file), "%s.brres", brres_name);
    else
        snprintf(file, sizeof(file), "BRRES");

    char desc[512];
    switch (res_type) {
    case ('S' << 8) | 'R':
        snprintf(desc, sizeof(desc),
                "Invalid version in .srt0 texture animation.\n\n"
                "SRT0 #%i is version %i (expected version 5).\n",
                index, bad_version);
        break;
    default:
        snprintf(desc, sizeof(desc),
                "A subfile in the BRRES (3D model) has an invalid version number:\n"
                " - bad_version=%i\n"
                " - res_type=0x%x\n"
                " - index=%i\n"
                " - brres_name=%s\n",
                bad_version, res_type, index,
                brres_name && *brres_name ? brres_name : "?");
        break;
    }

    SpBugCheck(file, desc);
}

static char loadedEffects[256];

// XREF: ObjEffect.S
void LogLoadedEffect(int index, const char *path) {
    const char *trailing = strrchr(path, '/');
    if (trailing != NULL)
        path = trailing + 1;

    if (loadedEffects[0] == '\0') {
        snprintf(loadedEffects, sizeof(loadedEffects), "%i: %s", index, path);
        return;
    }

    char loaded_effects_tmp[sizeof(loadedEffects)];
    int res = snprintf(loaded_effects_tmp, sizeof(loaded_effects_tmp), "%s, %i: %s", loadedEffects,
            index, path);
    if (res <= 0) {
        return;
    }

    snprintf(loadedEffects, sizeof(loadedEffects), "%s", loaded_effects_tmp);
}

// XREF: ObjEffect.S
void ResetLoadedEffects(void) {
    memset(loadedEffects, 0, sizeof(loadedEffects));
}

// XREF: ObjEffect.S
void TooManyEffectsFail(int current_count, int capacity, const char *path) {
    char desc[512];
    snprintf(desc, sizeof(desc),
            "Too many BREFF (particle effect) files. Maximum is %i.\n\nEffects (%i): %s.",
            capacity, current_count, loadedEffects[0] != '\0' ? loadedEffects : "?");

    SpBugCheck(path, desc);
}
