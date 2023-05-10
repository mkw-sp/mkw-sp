#pragma once

#include <Common.h>

typedef struct {
    float top;
    float bottom;
    float left;
    float right;
} Rect;

typedef struct {
    void *vtable;
    u8 _04[0x10 - 0x04];
    struct lyt_Pane *rootPane;
    u8 _14[0x18 - 0x14];
    float width;
    float height;
} lyt_Layout;
static_assert(sizeof(lyt_Layout) == 0x20);

lyt_Layout *lyt_Layout_CT(lyt_Layout *layout);
void lyt_Layout_DT(lyt_Layout *layout, int type);

static inline Rect getLayoutRect(const lyt_Layout *layout) {
    return (Rect){
            .top = layout->height / 2,
            .bottom = -layout->height / 2,
            .left = -layout->width / 2,
            .right = layout->width / 2,
    };
}

typedef struct {
    void *vtable;
    float viewMtx[3][4];
    Rect viewRect;
    float adjustedScale[2];
    float alpha;
    u32 flags;
} lyt_DrawInfo;
static_assert(sizeof(lyt_DrawInfo) == 0x54);

lyt_DrawInfo *lyt_DrawInfo_CT(lyt_DrawInfo *drawInfo);
void lyt_DrawInfo_DT(lyt_DrawInfo *drawInfo, int type);

typedef struct {
    u8 _[0xa4 - 0x00];
} lyt_ArcResourceLink;
// Size is tentative (based on array load in game code)

void lyt_ArcResourceLink_Set(lyt_ArcResourceLink *link, void *buffer, const char *root);

typedef struct {
    void *vt;
    char _04[0x1c - 0x04];
} lyt_MultiArcResourceAccessor;
static_assert(sizeof(lyt_MultiArcResourceAccessor) == 0x1c);

lyt_MultiArcResourceAccessor *lyt_MultiArcResourceAccessor_CT(lyt_MultiArcResourceAccessor *ac);
void lyt_MultiArcResourceAccessor_DT(lyt_MultiArcResourceAccessor *ac, int type);

void MultiArcResourceAccessor_Attach(lyt_MultiArcResourceAccessor *ac, lyt_ArcResourceLink *link);

void *MultiArcResourceAccessor_GetResource(const lyt_MultiArcResourceAccessor *ac, u32 type,
        const char *path, u32 *len);

typedef struct {
    u32 _[4];
} MEMAllocator;
extern MEMAllocator *lyt_spAlloc;

void Layout_calculateMtx(lyt_Layout *, lyt_DrawInfo *);
void Layout_draw(lyt_Layout *, lyt_DrawInfo *);
void Layout_build(lyt_Layout *, void *buf, void *ac);
