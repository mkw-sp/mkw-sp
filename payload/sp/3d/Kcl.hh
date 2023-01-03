#pragma once

#define _HAS_CXX20

#include "Common.hh"
#include <array>
#include <memory>
#include <sp/FixedString.hh>
#include <span>
#include <variant>

namespace SP {

struct KCollisionV1Header {
    u32 pos_data_offset;
    u32 nrm_data_offset;
    u32 prism_data_offset;
    u32 block_data_offset;
    f32 prism_thickness;
    Vec3 area_min_pos;
    u32 area_x_width_mask;
    u32 area_y_width_mask;
    u32 area_z_width_mask;
    s32 block_width_shift;
    s32 area_x_blocks_shift;
    s32 area_xy_blocks_shift;
    f32 sphere_radius;
};

struct KCollisionPrismData {
    f32 height{0.0f};
    u16 pos_i{0};
    u16 fnrm_i{0};
    u16 enrm1_i{0};
    u16 enrm2_i{0};
    u16 enrm3_i{0};
    u16 attribute{0};
};

struct WiimmKclVersion {
    // 2.26
    int major_version = 2;
    int minor_version = 26;
};
struct UnknownKclVersion {};
struct InvalidKclVersion {};

using KclVersion = std::variant<WiimmKclVersion, UnknownKclVersion, InvalidKclVersion>;

KclVersion InspectKclFile(std::span<const u8> kcl_file);
FixedString<32> GetKCLVersion(KclVersion metadata);

struct KclFile {
    KclFile(std::span<const u8> bytes);

    KclVersion m_version;
    const KCollisionV1Header *m_header = nullptr;
    std::span<const Vec3> pos;
    std::span<const Vec3> nrm;
    std::span<const KCollisionPrismData> prism;
    std::span<const u8> block;
};

class KclVis {
public:
    KclVis(std::span<const u8> file);
    ~KclVis();

    void render(const float mtx[3][4], bool overlay);

private:
    void prepare();

    KclFile m_file;

    // Not indexed (yet)
    struct DirectVertex {
        Vec3 pos;
        Vec3 nrm; // Same for all vertices
        u32 clr = 0xff00'00ff;
        u16 uv_u;
        u16 uv_v;
    };

    std::unique_ptr<u8[]> m_DL;
    u32 m_DLSize;
};

} // namespace SP
