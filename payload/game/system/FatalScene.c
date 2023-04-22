#include "FatalScene.h"
#include <egg/core/eggColorFader.h>
#include <egg/core/eggSystem.h>
#include <nw4r/lyt/lyt_pane.h>
#include <sp/storage/DecompLoader.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// This must always call down to OSFatal (CPU), to prevent infinite recursion
#define OSFATAL_CPU_ASSERT assert

// Read a compressed archive from disc
static void *RipFromDiscAlloc(const char *path, EGG_Heap *heap) {
    u8 *szs = NULL;
    size_t szsSize;
    DecompLoader_LoadRO(path, &szs, &szsSize, heap);
    return szs;
}

// The porting tool fails on the address itself
static void **GetSpAllocAddr() {
    const u32 ppc_lwz = *(const u32 *)(((const char *)&lyt_Layout_DT) + 0x48);

    OSFATAL_CPU_ASSERT((ppc_lwz >> (32 - 6)) == 32);
    OSFATAL_CPU_ASSERT(((ppc_lwz << 6) >> (32 - 5)) == 3);
    OSFATAL_CPU_ASSERT(((ppc_lwz << (6 + 5)) >> (32 - 5)) == 13);
    register s32 r13_offset = (s16)(ppc_lwz & 0xffff);

    void **result;
    asm("add %0, %1, 13" : "=r"(result) : "r"(r13_offset));
    return result;
}

#define lyt_spAlloc (*GetSpAllocAddr())

static lyt_Pane *findPane(lyt_Layout *lyt, const char *name) {
    lyt_Pane *result =
            lyt->rootPane->vtable->FindPaneByName(lyt->rootPane, name, /* recursive */ true);
    if (!result) {
        OSReport("[FatalScene] Failed to find pane %s\n", name);
    }
    return result;
}

static EGGScene_Vtable sFatalScene_Vtable;

FatalScene *FatalScene_CT(FatalScene *scene) {
    // OSReport("Creating FatalScene\n");

    memset(scene, 0, sizeof(*scene));
    EGG_Scene_CT((EGGScene *)scene);
    scene->inherit.vt = &sFatalScene_Vtable;

    lyt_DrawInfo_CT(&scene->drawInfo);
    lyt_MultiArcResourceAccessor_CT(&scene->resAccessor);
    // We already memset the struct
    // lyt_ArcResourceLink_CT(&scene->arcLink);
    lyt_Layout_CT(&scene->layout);

    return scene;
}
void FatalScene_DT(FatalScene *scene, int type) {
    // OSReport("Destroying FatalScene\n");

    lyt_DrawInfo_DT(&scene->drawInfo, -1);
    lyt_MultiArcResourceAccessor_DT(&scene->resAccessor, -1);
    // lyt_ArcResourceLink_DT(&scene->arcLink, -1);
    lyt_Layout_DT(&scene->layout, -1);

    EGG_Scene_DT(&scene->inherit, type);
}

static void setupGpu() {
    GXSetClipMode(GX_CLIP_ENABLE);
    GXSetCullMode(GX_CULL_NONE);
    GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);
    GXSetZMode(GX_FALSE, GX_NEVER, GX_FALSE);

    GXColor fogClr;
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, &fogClr);
}

static void setupCamera(lyt_DrawInfo *drawInfo, lyt_Layout *lyt) {
    const Rect frame = getLayoutRect(lyt);

    f32 znear = -1000.0f;
    f32 zfar = 1000.0f;

    float projMtx[4][4];

    // OSReport("[FatalScene] Ortho Mtx: top=%f,bottom=%f,left=%f,right=%f\n", frame.top,
    //         frame.bottom, frame.left, frame.right);
    C_MTXOrtho(projMtx, frame.top, frame.bottom, frame.left, frame.right, znear, zfar);
    GXSetProjection(projMtx, GX_ORTHOGRAPHIC);

    float viewMtx[3][4];
    PSMTXIdentity(viewMtx);
    static_assert(sizeof(drawInfo->viewMtx) == sizeof(viewMtx));
    memcpy(drawInfo->viewMtx, viewMtx, sizeof(drawInfo->viewMtx));
    drawInfo->viewRect = frame;
}

static void FatalScene_calc(EGGScene * /* scene */) {}
static void FatalScene_draw(EGGScene *scene) {
    FatalScene *this = (FatalScene *)scene;

    //
    // For some reason, the renderMode becomes truncated on the X axis during some loading
    // sequences (?)
    //

    // GXRenderModeObj *rm = sRKSystem.video->renderMode;
    // OSReport("RM: %f, %f\n", (float)rm->fb_width, (float)rm->efb_height);

    setupGpu();
    setupCamera(&this->drawInfo, &this->layout);

    float w = 640.0f;
    // w = rm->fb_width;
    float h = 456.0f;
    // h = rm->efb_height;

    // OSReport("[FatalScene] Viewport (SYSTEM): w=%f,h=%f\n", (float)rm->fb_width,
    //         (float)rm->efb_height);
    // OSReport("[FatalScene] Viewport: w=%f,h=%f\n", w, h);

    GXSetViewport(0.0f, 0.0f, w, h, 0.0f, 1.0f);
    GXSetScissor(0, 0, (u32)w, (u32)h);
    GXSetScissorBoxOffset(0, 0);

    Layout_calculateMtx(&this->layout, &this->drawInfo);
    Layout_draw(&this->layout, &this->drawInfo);
}

static void FatalScene_enter(EGGScene *scene) {
    FatalScene *this = (FatalScene *)scene;

    this->inherit.heapMem2->_1c = 0;
    EGG_ExpHeap_InitAlloc((EGG_ExpHeap *)this->inherit.heapMem2, &this->allocator, 32);
    lyt_spAlloc = &this->allocator;

    this->inherit.heapMem2->_1c = 0;
    void *archive = RipFromDiscAlloc("Scene/UI/CrashSP.szs", this->inherit.heapMem2);
    OSFATAL_CPU_ASSERT(archive);

    lyt_ArcResourceLink_Set(&this->arcLink, archive, ".");
    MultiArcResourceAccessor_Attach(&this->resAccessor, &this->arcLink);

    {
        void *lytRes =
                MultiArcResourceAccessor_GetResource(&this->resAccessor, 0, "Fatal.brlyt", NULL);
        OSFATAL_CPU_ASSERT(lytRes && "Can't find `Fatal.brlyt`");
        Layout_build(&this->layout, lytRes, &this->resAccessor);
    }

    this->bodyPane = findPane(&this->layout, "Body");
    OSFATAL_CPU_ASSERT(this->bodyPane && "Can't find `Body` pane in brlyt");

    OSFATAL_CPU_ASSERT(this->inherit.sceneMgr);
    OSFATAL_CPU_ASSERT(this->inherit.sceneMgr->fader);
    EGG_ColorFader_fadeIn(this->inherit.sceneMgr->fader);
}

void FatalScene_SetBody(FatalScene *this, const wchar_t *body) {
    OSFATAL_CPU_ASSERT(this->bodyPane);
    if (this->bodyPane != NULL) {
        lyt_TextBox *box = (lyt_TextBox *)this->bodyPane;

        box->vtable->SetString(box, body, 0);
    }
}

static void FatalScene_exit(EGGScene * /* scene */) {}
static void FatalScene_reinit(EGGScene * /* scene */) {}
static void FatalScene_incoming_childDestroy(EGGScene * /* scene */) {}
static void FatalScene_outgoing_childCreate(EGGScene * /* scene */) {}

static void FatalScene_DTAdapater(EGGScene *scene, int type) {
    FatalScene_DT((FatalScene *)scene, type);
}

static EGGScene_Vtable sFatalScene_Vtable = (EGGScene_Vtable){
        .dt = FatalScene_DTAdapater,
        .calc = FatalScene_calc,
        .draw = FatalScene_draw,
        .enter = FatalScene_enter,
        .exit = FatalScene_exit,
        .reinit = FatalScene_reinit,
        .incoming_childDestroy = FatalScene_incoming_childDestroy,
        .outgoing_childCreate = FatalScene_outgoing_childCreate,
};

static void PurgeHeap(MEMHeapHandle heap);

static void free_all_visitor(void *block, MEMHeapHandle heap, u32 /* userParam */) {
    for (MEMHeapHandle child = NULL; (child = MEMGetNextListObject(&heap->childList, child));) {
        PurgeHeap(child);
    }
    if (heap->signature == MEMi_EXPHEAP_SIGNATURE) {
        MEMFreeToExpHeap(heap, block);
    }
}

// Does not call dispose
static void PurgeHeap(MEMHeapHandle heap) {
    MEMVisitAllocatedForExpHeap(heap, &free_all_visitor, 0);
    if (heap->signature == MEMi_EXPHEAP_SIGNATURE) {
        MEMDestroyExpHeap(heap);
    }
}

void FatalScene_LeechCurrentScene(FatalScene *this) {
    // Reclaim MEM2
    EGGScene *curScene = sRKSystem.scnMgr->curScene;
    PurgeHeap(curScene->heapMem2->heapHandle);
    this->inherit.heapMem2 = curScene->heapMem2;
    // Unlock it
    this->inherit.heapMem2->_1c = 0;

    this->inherit.sceneMgr = sRKSystem.scnMgr;
    this->inherit.parentScene = curScene;
}

void FatalScene_MainLoop(FatalScene *this) {
    EGGDisplay *display = sRKSystem.display;

    for (;;) {
        display->vt->beginFrame(display);
        display->vt->beginRender(display);

        this->inherit.vt->calc(&this->inherit);
        this->inherit.vt->draw(&this->inherit);

        display->vt->endRender(display);
        display->vt->endFrame(display);
    }
}
