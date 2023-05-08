#include "Kcl.hh"

#include <revolution.hh>
extern "C" {
#include <revolution/tpl.h>
}

#include "sp/YAZDecoder.hh"
#include <algorithm>
#include <cstring>
#include <egg/core/eggHeap.hh>
#include <egg/math/eggMath.hh>

namespace SP {

// Sizes, in bytes
struct SectionSizes {
    const u32 pos_data_size = 0;
    const u32 nrm_data_size = 0;
    const u32 prism_data_size = 0;
    const u32 block_data_size = 0;
};

static SectionSizes GetSectionSizes(const KCollisionV1Header &header, u32 file_size) {
    /*
     0 pos_data_offset
     1 nrm_data_offset
     2 prism_data_offset
     3 block_data_offset
     4 <end_of_file>
    */
    struct SectionEntry {
        u32 offset = 0;
        size_t index = 0;
        u32 size = 0;
    };

    // prism_data_offset is 1-indexed, so the offset is pulled back
    std::array<SectionEntry, 5> sections{SectionEntry{.offset = header.pos_data_offset, .index = 0},
            SectionEntry{.offset = header.nrm_data_offset, .index = 1},
            SectionEntry{.offset = static_cast<u32>(
                                 header.prism_data_offset + sizeof(KCollisionPrismData)),
                    .index = 2},
            SectionEntry{.offset = header.block_data_offset, .index = 3},
            SectionEntry{.offset = file_size, .index = 4}};

    std::sort(sections.begin(), sections.end(),
            [](auto &lhs, auto &rhs) { return lhs.offset < rhs.offset; });

    for (size_t i = 0; i < 4; ++i) {
        sections[i].size = sections[i + 1].offset - sections[i].offset;
    }

    std::sort(sections.begin(), sections.end(),
            [](auto &lhs, auto &rhs) { return lhs.index < rhs.index; });

    const u32 pos_data_size = sections[0].size;
    const u32 nrm_data_size = sections[1].size;
    const u32 prism_data_size = sections[2].size;
    const u32 block_data_size = sections[3].size;

    return {pos_data_size, nrm_data_size, prism_data_size, block_data_size};
}

template <typename T, typename byte_view_t>
static inline T *reinterpret_buffer(byte_view_t data, unsigned offset = 0) {
    if (offset + sizeof(T) > data.size_bytes()) {
        return nullptr;
    }

    return reinterpret_cast<T *>(data.data() + offset);
}

template <typename T, typename byte_view_t>
static inline std::span<T> span_cast(byte_view_t data, unsigned offset = 0) {
    if (offset + sizeof(T) > data.size_bytes()) {
        return {};
    }

    const size_t buffer_len = ROUND_DOWN(data.size_bytes(), sizeof(T));
    return {reinterpret_cast<T *>(data.data() + offset),
            reinterpret_cast<T *>(data.data() + buffer_len)};
}

static FixedString<32> FormatVersion(const WiimmKclVersion &ver) {
    char buf[32];
    snprintf(buf, sizeof(buf), "WiimmSZS v%d.%d", ver.major_version, ver.minor_version);
    return buf;
}
static FixedString<32> FormatVersion(const UnknownKclVersion &) {
    return "<Unknown KCL Encoder>";
}
static FixedString<32> FormatVersion(const InvalidKclVersion &) {
    return "<Invalid KCL File>";
}
FixedString<32> FormatVersion(KclVersion metadata) {
    if (auto *as_wiimm = std::get_if<WiimmKclVersion>(&metadata)) {
        return FormatVersion(*as_wiimm);
    } else if (auto *as_unknown = std::get_if<UnknownKclVersion>(&metadata)) {
        return FormatVersion(*as_unknown);
    } else if (auto *as_invalid = std::get_if<InvalidKclVersion>(&metadata)) {
        return FormatVersion(*as_invalid);
    }
    return "";
}

constexpr std::array<char, 8> WiimmSZSIdentifier = {'W', 'i', 'i', 'm', 'm', 'S', 'Z', 'S'};

//! The first position will be this type
struct WiimmKclMetadata {
    std::array<char, 8> identifier = WiimmSZSIdentifier;
    f32 version = 0.0f;
};
static_assert(sizeof(WiimmKclMetadata) == sizeof(Vec3));

KclVersion InspectKclFile(std::span<const u8> kcl_file) {
    auto *header = reinterpret_buffer<const KCollisionV1Header>(kcl_file);
    if (header == nullptr) {
        // Data is not large enough
        return InvalidKclVersion{};
    }

    const auto sizes = GetSectionSizes(*header, kcl_file.size_bytes());

    if (sizes.pos_data_size < sizeof(Vec3)) {
        // File is empty
        return UnknownKclVersion{};
    }

    static_assert(sizeof(WiimmKclMetadata) == sizeof(Vec3));
    auto *wiimm_metadata =
            reinterpret_buffer<const WiimmKclMetadata>(kcl_file, header->pos_data_offset);

    if (wiimm_metadata == nullptr) {
        // This shouldnt be reached
        return InvalidKclVersion{};
    }

    if (wiimm_metadata->identifier != WiimmSZSIdentifier) {
        // No other heuristics for now
        return UnknownKclVersion{};
    }

    SP_LOG("VER %f\n", wiimm_metadata->version);

    const int major_version = static_cast<int>(wiimm_metadata->version);
    const int minor_version = static_cast<int>(wiimm_metadata->version * 100.0f) % 100;

    return WiimmKclVersion{.major_version = major_version, .minor_version = minor_version};
}

KclFile::KclFile(std::span<const u8> bytes) {
    m_version = InspectKclFile(bytes);
    {
        auto ver = FormatVersion(m_version);
        SP_LOG("Parsing KCL file size %u bytes (metadata: %s)", bytes.size(), ver.c_str());
    }
    m_header = reinterpret_buffer<const KCollisionV1Header>(bytes);
    assert(m_header != nullptr);
    if (m_header == nullptr) {
        return;
    }
    const auto sizes = GetSectionSizes(*m_header, bytes.size());
    SP_LOG("SIZES: position %u bytes; normal %u bytes; prism %u bytes; block %u bytes",
            sizes.pos_data_size, sizes.nrm_data_size, sizes.prism_data_size, sizes.block_data_size);
    if (m_header->pos_data_offset + sizes.pos_data_size < bytes.size()) {
        pos = span_cast<const Vec3>(bytes.subspan(m_header->pos_data_offset, sizes.pos_data_size));
    }
    if (m_header->nrm_data_offset + sizes.nrm_data_size < bytes.size()) {
        nrm = span_cast<const Vec3>(bytes.subspan(m_header->nrm_data_offset, sizes.nrm_data_size));
    }
    if (m_header->prism_data_offset + sizes.prism_data_size < bytes.size()) {
        prism = span_cast<const KCollisionPrismData>(
                bytes.subspan(m_header->prism_data_offset, sizes.prism_data_size));
    }
    if (m_header->block_data_offset + sizes.block_data_size < bytes.size()) {
        block = bytes.subspan(m_header->block_data_offset, sizes.block_data_size);
    }
}

inline std::array<Vec3, 3> FromPrism(float height, const Vec3 &pos, const Vec3 &fnrm,
        const Vec3 &enrm1, const Vec3 &enrm2, const Vec3 &enrm3) {
    auto CrossA = cross(enrm1, fnrm);
    auto CrossB = cross(enrm2, fnrm);
    auto Vertex1 = pos;
    auto Vertex2 = pos + CrossB * (height / dot(CrossB, enrm3));
    auto Vertex3 = pos + CrossA * (height / dot(CrossA, enrm3));

    return {Vertex1, Vertex2, Vertex3};
}

KclVis::KclVis(std::span<const u8> file) : m_file(file) {
    prepare();
}
KclVis::~KclVis() {
    SP_LOG("Freeing KclVis (%u bytes)", m_DLSize);
    m_DL.reset();
}
static GXColor TransformHRadians(GXColor in, // color to transform
        float H) {
    float U = EGG::Math<float>::cos(H);
    float W = EGG::Math<float>::sin(H);

    GXColor ret;
    ret.r = (.299f + .701f * U + .168f * W) * in.r + (.587f - .587f * U + .330f * W) * in.g +
            (.114f - .114f * U - .497f * W) * in.b;
    ret.g = (.299f - .299f * U - .328f * W) * in.r + (.587f + .413f * U + .035f * W) * in.g +
            (.114f - .114f * U + .292f * W) * in.b;
    ret.b = (.299f - .3f * U + 1.25f * W) * in.r + (.587f - .588f * U - 1.05f * W) * in.g +
            (.114f + .886f * U - .203f * W) * in.b;
    ret.a = in.a;
    return ret;
}
static GXColor GetKCLColor(u16 attr) {
    auto attr_fraction = static_cast<float>(attr & 31) / 31.0f;
    bool soft = attr >> 15;
    bool reject = (attr >> 14) & 1;
    bool trick = (attr >> 13) & 1;
    // TODO: Some sensible coloring scheme, maybe a LUT for each basic type
    GXColor base_color{0xf4, 0xa2, 0x61, 0x88};
    (void)reject;
    (void)soft;
    if (trick) {
        base_color.r = 0xFF;
        base_color.g *= 1.1f;
        base_color.b *= 1.1f;
    }

    return TransformHRadians(base_color, attr_fraction * (3.1415f * 2.0f) + 60.0f);
}

// clang-format off
alignas(64) static const u8 TRUSS_SZS[] = {
    0x59, 0x61, 0x7A, 0x30, 0x00, 0x00, 0x20, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x20, 0xAF, 0x30, 0x00, 0x00, 0x00,
    0x01, 0x5A, 0x10, 0x03, 0x0C, 0x10, 0x07, 0x20, 0x00, 0xE0, 0x00, 0x80,
    0x10, 0x01, 0x53, 0x40, 0x14, 0x60, 0x90, 0x1B, 0x01, 0x50, 0x33, 0x00,
    0x02, 0x0F, 0xBB, 0xBB, 0xFA, 0xFF, 0xFF, 0xBB, 0xBB, 0xBF, 0x10, 0x03,
    0xBB, 0x20, 0x03, 0xD4, 0xBF, 0xBB, 0xD0, 0x00, 0xFF, 0x00, 0x00, 0x01,
    0xBF, 0x10, 0x14, 0x20, 0x18, 0xE7, 0xBB, 0xBF, 0xFF, 0x00, 0x00, 0xFF,
    0x01, 0x11, 0x9E, 0xBB, 0xBB, 0xFB, 0x60, 0x51, 0xFA, 0xFF, 0xFB, 0x42,
    0x0B, 0x00, 0x03, 0x02, 0x82, 0x1F, 0x00, 0x2D, 0x00, 0x02, 0x1F, 0xFF,
    0x24, 0x01, 0xFF, 0x9D, 0x00, 0x03, 0x0C, 0xFF, 0x12, 0x16, 0x53, 0xEE,
    0xFB, 0x00, 0x3F, 0x08, 0x02, 0x1F, 0xFF, 0x00, 0x01, 0xFF, 0xB9, 0x05,
    0xE3, 0x0E, 0x02, 0x1F, 0xFF, 0x01, 0xFF, 0xDD, 0x02, 0x1F, 0xFF, 0x01,
    0xFF, 0xFD, 0x04, 0x3F, 0xFF, 0x01, 0xFF, 0xFD, 0x00, 0x06, 0x5F, 0xFF,
    0x01, 0xFF, 0xFD, 0x08, 0x7F, 0xFF, 0x01, 0xFF, 0xFD, 0x0A, 0x9F, 0xFF,
    0x01, 0xFF, 0xFD, 0x0C, 0xBF, 0xFF, 0x01, 0xFF, 0xFD, 0x00, 0x0E, 0xDF,
    0xEC, 0x01, 0xFF, 0xFF, 0x0E, 0xDF, 0xDD, 0x03, 0xFF, 0xFF, 0x0E, 0xDF,
    0xDD, 0x05, 0xFF, 0xFF, 0x0E, 0xDF, 0xDD, 0x07, 0xFF, 0xFF, 0x01, 0x0E,
    0xDF, 0xDD, 0x0F, 0xFF, 0x00, 0xC0, 0x01, 0x0F, 0x4F, 0x00, 0x00, 0x1F,
    0xFF, 0x01, 0x1F, 0x6D, 0xCE, 0xDF, 0xBB, 0x80, 0xBB, 0x01, 0xC1, 0x00,
    0xAE, 0xDF, 0xE1, 0xDF,
};

alignas(64) static u8 TRUSS_TPL[0x2060];
// clang-format on

alignas(32) GXTexObj TRUSS_OBJ;
static bool trussBound = false;

void KclVis::prepare() {
    const size_t count = m_file.prism.size();
    m_DLSize = ROUND_UP(count * 3 * sizeof(DirectVertex) + 4 /* Header */, 32);
    SP_LOG("Prism count: %u; Buffer size: %u.", (u32)count, m_DLSize);
    m_DL = std::unique_ptr<u8[]>(new (32) u8[m_DLSize]);
    assert(m_DL);
    memset(m_DL.get(), 0, m_DLSize); // 0 acts as a nop
    m_DL[0] = 0;                     // For alignment
    m_DL[1] = static_cast<u8>(GX_TRIANGLES) | static_cast<u8>(GX_VTXFMT0);
    m_DL[2] = (count * 3) >> 8;
    m_DL[3] = (count * 3) & 0xff;

    DirectVertex *vertices = reinterpret_cast<DirectVertex *>(m_DL.get() + 4);

    // NOTE: Technically we need to index this data by the octree. Not all prisms need be
    // referenced.
    // TODO: Indexed vertex data
    for (size_t i = 0; i < count; ++i) {
        const KCollisionPrismData &prism = m_file.prism[i];
        const auto &fnrm = m_file.nrm[prism.fnrm_i];
        const auto verts = FromPrism(prism.height, m_file.pos[prism.pos_i], fnrm,
                m_file.nrm[prism.enrm1_i], m_file.nrm[prism.enrm2_i], m_file.nrm[prism.enrm3_i]);
        auto gx_clr = GetKCLColor(prism.attribute);
        u32 clr = (u32 &)gx_clr;
        clr |= 0xff;
        // U16 for UVs for alignment (*2 = 4 bytes)
        vertices[i * 3] = {verts[0], fnrm, clr, 0, 0};
        vertices[i * 3 + 1] = {verts[1], fnrm, clr, 0, 1};
        vertices[i * 3 + 2] = {verts[2], fnrm, clr, 1, 1};
    }

    if (!trussBound) {
        trussBound = true;
        YAZDecoder::Decode(TRUSS_SZS, sizeof(TRUSS_SZS), TRUSS_TPL, sizeof(TRUSS_TPL));
        TPLBind(TRUSS_TPL);
        TPLGetGXTexObjFromPalette(TRUSS_TPL, &TRUSS_OBJ, 0);
    }
}
void KclVis::render(const float mtx[3][4], bool /* overlay */) {
    GXLoadPosMtxImm(mtx, 0);
    float ident[4][4];
    PSMTXIdentity(ident);
    GXLoadNrmMtxImm(ident, 0 /* GX_PNMTX0 */);
    GXSetCurrentMtx(0);

    // mMaterial.use();
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXLoadTexObj(&TRUSS_OBJ, GX_TEXMAP0);

    struct Light {
        u32 color;
        Vec3 view_pos;
    };
    Light lights[2] = {
            {0xff, {0.0f, 4000.0f, 5000.0f}},
            {0xF6F6F6FF, {0, 4000.0f, 0.0f}},
    };

    u8 alpha = 0; // 0x88
    GXColor mat_color = {0xFF, 0xFF, 0xFF, alpha};
    GXColor amb_color = {0x60, 0x60, 0x60, alpha};

    // Vec light_view_pos;
    // MTXMultVec(view_mtx._mtx, &light_world_pos, &light_view_pos);

    for (int i = 0; i < 2; ++i) {
        Light &light = lights[i];

        GXLightObj light_obj;
        light_obj.color.r = light.color;
        light_obj.color.g = light.color >> 8;
        light_obj.color.b = light.color >> 16;
        light_obj.color.a = light.color >> 24;
        light_obj.pos[0] = light.view_pos.x;
        light_obj.pos[1] = light.view_pos.y;
        light_obj.pos[2] = light.view_pos.z;
        GXLoadLightObjImm(&light_obj, (1 << i));
    }

    GXSetNumChans(1);

    GXSetChanMatColor(GX_COLOR0A0, mat_color);
    GXSetChanAmbColor(GX_COLOR0A0, amb_color);
    GXSetChanCtrl(GX_COLOR0A0, // chan
            GX_TRUE,           // enable
            GX_SRC_REG,        // amb_src
            GX_SRC_VTX,        // mat_src
            3,                 // light_mask
            GX_DF_CLAMP,       // diff_fn
            GX_AF_NONE         // attn_fn
    );

    GXSetNumIndStages(0);
    GXSetNumTevStages(1);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    {
        GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    }

    GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
    GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);

    // DO not touch PECNTRL
    //		GXSetZCompLoc(1);

    GXSetCullMode(GX_CULL_NONE);

    // Vertex format
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_NRM, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U16, 0);

    GXSetAlphaUpdate(GX_FALSE);
    assert(m_DL != nullptr);
    GXCallDisplayList(m_DL.get(), m_DLSize);
    // mMaterial.unuse();
    GXSetAlphaUpdate(GX_TRUE);
}

} // namespace SP
