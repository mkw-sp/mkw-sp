#include <revolution.h>

#include <string.h> /* memcpy */

#ifdef MDLSAN_DEBUG
#define MSA_DEBUG_LOG SP_LOG
#define MSA_VERBOSE_LOG(m, ...) SP_LOG("(Verbose) " m, ##__VA_ARGS__)
#else
#define MSA_VERBOSE_LOG(...)
#define MSA_DEBUG_LOG SP_LOG
#endif

/* Limited subset of commands we expext to encounter. */
enum CommandTags {
    /* Tag for a BP (raster state) command. Expect four bytes to follow.
     * [8 bits] [8 bits] [24 bits]
     * Tag      Register Value
     */
    CMD_TAG_BP = 0x61,
    CMD_SIZE_BP = 0x05,

    /* Does nothing. Command size only includes the tag. */
    CMD_TAG_NOOP = 0x00,
    CMD_SIZE_NOOP = 0x01
};

enum BPAddressSpace {
    BPMEM_IND_MTX = 0x06, /* 3x3 parts, column-major */
    BPMEM_RAS1_SS = 0x25, /* x2 */
    BPMEM_IREF = 0x27
};

/* Determine whether or not indirect matrix patching is necessary. */
static bool isIndPatchNecessary(
        uint8_t *pData /* Valid pointer to the indirect settings display list (64 bytes) */) {
    /* Rationale for removal:
     * All materials should be standard layout. Additionally, if the user is
     * confident the material does not flicker, a material flag can be specified.
     */
#if 0
    /* First check of the indirect scales are as expected. If they're not, we
     * shouldn't patch the rest of the block. If for whatever reason a tool wants
     * to disable this check, it should at minimum swap these two commands.
     */
    if (pData[0] != CMD_TAG_BP || pData[1] != BPMEM_RAS1_SS + 0 ||
            pData[5] != CMD_TAG_BP || pData[6] != BPMEM_RAS1_SS + 1)
        return false;
#endif

    /* If the indirect matrix is omitted, in whole or in part, correct it. */
    return (pData[10] == CMD_TAG_NOOP) | (pData[15] == CMD_TAG_NOOP) | (pData[20] == CMD_TAG_NOOP);
}

/* Default indirect matrix (skew). An identity matrix is almost always too
 * intense, so we supply a simple scale matrix where (U, V) is mapped to (U/10,
 * V/10):
 *
 * | .1 .0 .0 |
 * | .0 .1 .0 |
 *
 */
static const uint8_t DefaultIndirectMatrix[CMD_SIZE_BP * 3] = {
        /* A = 0x333, B=0x000, S0=0x002 */
        CMD_TAG_BP, BPMEM_IND_MTX + 0, 0x80, 0x03, 0x33,
        /* C = 0x000, D=0x333, S1=0x003 */
        CMD_TAG_BP, BPMEM_IND_MTX + 1, 0xD9, 0x98, 0x00,
        /* E = 0x000, F=0x000, S2=0x000 */
        CMD_TAG_BP, BPMEM_IND_MTX + 2, 0x00, 0x00, 0x00
        /**/
};

/* Copies the valid indirect matrix */
static uint8_t *applyIndPatch(
        uint8_t *pData /* Valid pointer to the indirect settings display list (64 bytes) */) {
    memcpy(&pData[10], (void *)&DefaultIndirectMatrix, sizeof(DefaultIndirectMatrix));

    return pData;
}

/* We check if the first scale is set. This method isn't perfect, as it is
 * possible the indirect stage really does access the eighth texture. I have yet
 * to see a model that does this. We can perform this check relatively safely.
 * */
static bool isShaderIndirect(uint8_t *pTev) {
    uint8_t *const pDl = pTev + 32; /* Skip header to data */

    /* Scan for RAS1_IREF command in display list.
     * Search only first 96 bytes--settings shared for all stages.
     */
    for (uint8_t *it = pDl; it < pDl + 96;) {
        switch (*it) {
        case CMD_TAG_BP: {
            it++;
            const uint32_t regVal = *(uint32_t *)(it);
            it += 4;

            if (((regVal & 0xff000000) >> 24) == BPMEM_IREF &&
                    (regVal & 0x00ffffff) != 0x00ffffff) {
                MSA_VERBOSE_LOG("RAS1_IREF references texcoords or texmaps");
                return true;
            }

            break;
        }
        case CMD_TAG_NOOP:
            it++;
            break;
        default:
            MSA_DEBUG_LOG("Failed to parse TEV configuration at %p: illegal operation: 0x%x", pTev,
                    (uint32_t)*it);
            /* If something unexpected shows up, let's just quit gracefully. */
            return false;
        }
    }

    return false;
}

/* Getters and setters for the material indirect count, as we do not use a
 * structure.
 */
static uint8_t getMaterialIndirectCount(uint8_t *pMat) {
    return *(pMat + 0x17);
}
static void setMaterialIndirectCount(uint8_t *pMat, uint8_t num) {
    *(pMat + 0x17) = num;
}

/* Gets the TEV shader of a material. */
static uint8_t *getShaderFromMaterial(uint8_t *pMat) {
    return pMat + *(int32_t *)(pMat + 0x28);
}
/* Gets the indirect display list from a material. */
static uint8_t *getMaterialIndirectDl(uint8_t *pMat) {
    return pMat + *(int32_t *)(pMat + 0x3c) + 0xa0;
}

static uint32_t getMaterialFlag(uint8_t *pMat) {
    return *(uint32_t *)(pMat + 0x10);
}
static const char *getMaterialName(uint8_t *pMat) {
    return (const char *)(pMat + *(int32_t *)(pMat + 8));
}

/* Patch for two essential material settings brawlbox corrupts:
 * - Indirect stage count. Necessary for all subsystems to
 *   actually send anything.
 *
 * - Indirect matrices (control skew) are omitted, leaving
 *   them undefined.
 */
void sanitizeMaterial(u8 *pMat) {
    MSA_VERBOSE_LOG("Sanitizing material %p", pMat);

    /* If the user is confident the material does not flicker, respect that. */
    if (getMaterialFlag(pMat) & 0x40000000) {
        MSA_DEBUG_LOG("Material %s is marked confident.", getMaterialName(pMat));
        return;
    }

    if (!isShaderIndirect(getShaderFromMaterial(pMat))) {
        return;
    }

    if (getMaterialIndirectCount(pMat) == 0) {
        MSA_DEBUG_LOG("Correcting GEN_INFO for %s.", getMaterialName(pMat));
        setMaterialIndirectCount(pMat, 1); /* Assume 1 (most likely) */
    }

    if (isIndPatchNecessary(getMaterialIndirectDl(pMat))) {
        MSA_DEBUG_LOG("Correcting IND_MTX for %s.", getMaterialName(pMat));
        applyIndPatch(getMaterialIndirectDl(pMat));
    }
}
