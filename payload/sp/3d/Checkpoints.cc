#include "Checkpoints.hh"
#include <Common.hh>

#include <game/system/CourseMap.hh>
extern "C" {
#include <revolution.h>
#include <revolution/gx.h>
}

#include <egg/math/eggMath.hh>
#include <numbers>
#include <span>

namespace SP {

namespace {

static GXColor TransformHRadians(GXColor in, // color to transform
        float H) {
    float U = EGG::Math<float>::cos(H);
    float W = EGG::Math<float>::sin(H);

    GXColor ret;
    ret.r = (.299 + .701 * U + .168 * W) * in.r + (.587 - .587 * U + .330 * W) * in.g +
            (.114 - .114 * U - .497 * W) * in.b;
    ret.g = (.299 - .299 * U - .328 * W) * in.r + (.587 + .413 * U + .035 * W) * in.g +
            (.114 - .114 * U + .292 * W) * in.b;
    ret.b = (.299 - .3 * U + 1.25 * W) * in.r + (.587 - .588 * U - 1.05 * W) * in.g +
            (.114 + .886 * U - .203 * W) * in.b;
    ret.a = in.a;
    return ret;
}

namespace lgx {
void bypassSend(u32 regval) {
    GXColor1x8(0x61);
    GXColor1u32(regval);
}
} // namespace lgx

System::MapdataJugemPointAccessor *JugemPoints() {
    return System::CourseMap::spInstance->mpJugemPoint;
}
System::MapdataCheckPointAccessor *CheckPoints() {
    return System::CourseMap::spInstance->mpCheckPoint;
}
System::MapdataCheckPathAccessor *CheckPaths() {
    return System::CourseMap::spInstance->mpCheckPath;
}

struct TranslucentVertexColors {
    TranslucentVertexColors() {
        createDisplayListCached();
    }

    void createDisplayListCached() {
        alignas(32) static const u8 CACHED_DL[] = {0x61, 0x28, 0x04, 0x90, 0x00, 0x61, 0xC0, 0x08,
                0xA8, 0x9F, 0x61, 0xC1, 0x08, 0xFF, 0xD0, 0x61, 0xC0, 0x08, 0xAF, 0xFF, 0x61, 0xC1,
                0x08, 0xBF, 0xF0, 0x61, 0x22, 0x00, 0x06, 0x0C, 0x61, 0x40, 0x00, 0x00, 0x17, 0x61,
                0x41, 0x00, 0x04, 0xBD, 0x61, 0xF3, 0x3F, 0x00, 0x00, 0x61, 0x40, 0x00, 0x00, 0x07,
                0x61, 0x41, 0x00, 0x04, 0xBD, 0x61, 0x41, 0x00, 0x04, 0xAD, 0x61, 0x00, 0x00, 0x00,
                0x10, 0x08, 0x50, 0x00, 0x00, 0x22, 0x00, 0x08, 0x60, 0x00, 0x00, 0x00, 0x00, 0x10,
                0x00, 0x00, 0x10, 0x08, 0x00, 0x00, 0x00, 0x01, 0x08, 0x70, 0x40, 0x77, 0x70, 0x09,
                0x08, 0x80, 0xC8, 0x24, 0x12, 0x09, 0x08, 0x90, 0x04, 0x82, 0x41, 0x20, 0x00, 0x10,
                0x00, 0x00, 0x10, 0x09, 0x00, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x10, 0x0E, 0x00,
                0x00, 0x05, 0x01, 0x10, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x05, 0x01, 0x10, 0x00,
                0x00, 0x10, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        static_assert((std::size(CACHED_DL) % 32) == 0);
        mDl = {CACHED_DL, std::size(CACHED_DL)};
    }

    void use() {
        GXCallDisplayList(mDl.data(), mDl.size());
        lgx::bypassSend(0x43000041);
    }
    void unuse() {
        lgx::bypassSend(0x43000041);
    }

private:
    std::span<const u8> mDl;
};

class Checkpoints {
public:
    static void onDraw() {
        TranslucentVertexColors mMaterial;
        mMaterial.use();

        for (int i = 0; i < CheckPaths()->mNumEntries; ++i) {
            auto *ckph = CheckPaths()->cdata(i);

            drawCheckPath(ckph);
        }
        mMaterial.unuse();
    }
    static void drawCheckPath(const System::MapdataCheckPath::SData *ckph) {
        int range_start = ckph->start;
        int range_end = ckph->start + ckph->size;

        for (int j = range_start; j < range_end; ++j) {
            auto *ckpt = CheckPoints()->cdata(j);

            // Skip non-key checkpoints
            if (ckpt->lapCheck == 0xFF && ckpt->nextPt != 0 &&
                    !(ckpt->nextPt == 0xFF && ckph->next[0] == 0))
                continue;

            u8 next_id = ckpt->nextPt;

            // Sequence edge
            if (next_id == 0xFF) {
                if (ckph->next[1] != 0xFF)
                    SP_LOG("!!!\n!!! Odd edge case where a key checkpoint is a "
                           "checkpath edge\n!!!\n");
                // FIXME
                // Are there multiple quads in this case?
                // Right now we just take the first
                // FIXME: We assume the first is the only one!
                u8 next_ckph = ckph->next[0];

                if (next_ckph == 0xFF) {
                    SP_LOG("!!!\n!!! What custom track is this?!\n!!!\n");
                    continue; // Just give up at this point
                }
                next_id = CheckPaths()->cdata(next_ckph)->start;
            }

            {
                const auto *next = CheckPoints()->cdata(next_id);

                // TODO: Maybe make color match depth
                u32 clr = 0xAAAAAA33;
                if (ckpt->lapCheck != 0xFF)
                    clr = calcColor(j, System::CourseMap::spInstance->mpCheckPoint->mNumEntries);
                drawCheckPoint(ckpt, next, clr);
            }
        }
    }
    static u32 calcColor(u32 i, u32 num) {
        GXColor base;
        (u32 &)base = 0xf4a26199; // 88

        GXColor color = TransformHRadians(base,
                2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(num));

        return (u32 &)color;
    }
    static void drawCheckPoint(const System::MapdataCheckPoint::SData *ckpt,
            const System::MapdataCheckPoint::SData *next, u32 color) {
        float top = 5000.0f;
        float bottom = -5000.0f;

        float next_top = 5000.0f;
        float next_bottom = -5000.0f;

        auto *pt = JugemPoints()->cdata(ckpt->jugemIndex);
        auto *npt = JugemPoints()->cdata(next->jugemIndex);

        top += pt->position.y;
        bottom += pt->position.y;

        next_top += npt->position.y;
        next_bottom += npt->position.y;

        struct Vertex {
            Vec3 pos;
            u32 flags;
        };

        enum Flags { VTX_TRANS = 1 };

        u32 color_trans = (color & 0xFFFFFF00) | 0x60;

        constexpr int NUM_QUADS = 4;

        Vertex faces[NUM_QUADS][4] = {// Front face
                {{{ckpt->left.x, top, ckpt->left.y}, 0}, {{ckpt->left.x, bottom, ckpt->left.y}, 0},
                        {{ckpt->right.x, bottom, ckpt->right.y}, 0},
                        {{ckpt->right.x, top, ckpt->right.y}, 0}},
                // Left face
                {{{ckpt->left.x, top, ckpt->left.y}, 0}, {{ckpt->left.x, bottom, ckpt->left.y}, 0},
                        {{next->left.x, next_bottom, next->left.y}, VTX_TRANS},
                        {{next->left.x, next_top, next->left.y}, VTX_TRANS}},
                // Right face
                {{{ckpt->right.x, top, ckpt->right.y}, 0},
                        {{ckpt->right.x, bottom, ckpt->right.y}, 0},
                        {{next->right.x, next_bottom, next->right.y}, VTX_TRANS},
                        {{next->right.x, next_top, next->right.y}, VTX_TRANS}},
                // Back face
                {{{next->left.x, next_top, next->left.y}, VTX_TRANS},
                        {{next->left.x, next_bottom, next->left.y}, VTX_TRANS},
                        {{next->right.x, next_bottom, next->right.y}, VTX_TRANS},
                        {{next->right.x, next_top, next->right.y}, VTX_TRANS}}};

        int to_draw_quads = NUM_QUADS;

        if (ckpt->lapCheck == 0xFF)
            to_draw_quads = 3;

        GXBegin(GX_QUADS, GX_VTXFMT0, to_draw_quads * 4);

        for (int quad = 0; quad < to_draw_quads; ++quad) {
            for (int vert = 0; vert < 4; ++vert) {
                const Vertex &v = faces[quad][vert];

                u32 _c = v.flags & VTX_TRANS ? color_trans : color;
                GXPosition3f32(v.pos.x, v.pos.y, v.pos.z);
                GXColor1u32(_c);
            }
        }
        GXEnd();

        for (int quad = 0; quad < NUM_QUADS; ++quad) {
            GXBegin(GX_LINESTRIP, GX_VTXFMT0, 5);
            for (int vert = 0; vert < 4 + 1; ++vert) {
                const Vertex &v = faces[quad][vert % 4];

                u32 _c = color | 0xFF;
                //	if ((v.flags & VTX_TRANS) == 0)
                //		_c &= 0xFF;
                GXPosition3f32(v.pos.x, v.pos.y, v.pos.z);
                GXColor1u32(_c);
            }
            GXEnd();
        }
    }
};

} // namespace

void DrawCheckpoints(const float viewMtx[3][4]) {
    GXLoadPosMtxImm(viewMtx, 0);
    GXSetCurrentMtx(0);
    Checkpoints::onDraw();
}

} // namespace SP
