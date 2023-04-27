#include "BugCheck.h"
#include "FatalScene.h"

#include <revolution.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <egg/core/eggSystem.h>
#include <sp/WideUtil.h>

#include <sp/StackTrace.h>

// Referenced by SceneCreatorDyanmic.S
bool sBugCheckSet = false;

static void SpFormatBugCheck(wchar_t *out, size_t outSize, const char *file,
        const char *description) {
    char tmp[512];
    memset(tmp, 0, sizeof(tmp));

    snprintf(tmp, sizeof(tmp),
            "Subfile: %s\n"
            "Error: %s\n",
            file[0] != '\0' ? file : "?", description[0] != '\0' ? description : "?");

    Util_toUtf16(out, outSize, tmp, sizeof(tmp));
    out[outSize - 1] = L'\0';
}

void SpBugCheck(const char *file, const char *description) {
    OSReport("------------------------------\n");
    OSReport("BUGCHECK\n");
    OSReport("FILE: %s\n", file);
    OSReport("DESC: %s\n", description);
    OSReport("------------------------------\n");

    char trace[512];
    WriteStackTraceShort(trace, sizeof(trace), OSGetStackPointer());
    OSReport("TRACE: %s\n", trace);

    if (sBugCheckSet) {
        return;
    } else {
        sBugCheckSet = true;
    }

    wchar_t formattedBugCheck[512];
    SpFormatBugCheck(formattedBugCheck, ARRAY_SIZE(formattedBugCheck), file, description);

    static FatalScene fScene;
    FatalScene_CT(&fScene);

    // We can't safely unload the current scene, so just use its memory.
    FatalScene_LeechCurrentScene(&fScene);

    fScene.inherit.vt->enter(&fScene.inherit);

    FatalScene_SetBody(&fScene, formattedBugCheck);

    FatalScene_MainLoop(&fScene);

    __builtin_unreachable();
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
void InvalidRevisionFail(int bad_version, unsigned int res_type, int index,
        const char *brres_name) {
    char file[64];

    if (brres_name && *brres_name) {
        snprintf(file, sizeof(file), "%s.brres", brres_name);
    } else {
        snprintf(file, sizeof(file), "BRRES");
    }

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
                bad_version, res_type, index, brres_name && *brres_name ? brres_name : "?");
        break;
    }

    SpBugCheck(file, desc);
}

static char loadedEffects[256];

// XREF: ObjEffect.S
void LogLoadedEffect(int index, const char *path) {
    const char *trailing = strrchr(path, '/');
    if (trailing != NULL) {
        path = trailing + 1;
    }

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
            "Too many BREFF (particle effect) files. Maximum is %i.\n\nEffects (%i): %s.", capacity,
            current_count, loadedEffects[0] != '\0' ? loadedEffects : "?");

    SpBugCheck(path, desc);
}
