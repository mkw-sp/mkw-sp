#include <Common.hh>
//Credits to https://wiki.tockdom.com/wiki/MDL0_(File_Format). Thanks guys

namespace nw4r::g3d {
    class Material {

        struct BasicModelStructure {
            u32 len;
            u32 MDL0Offset;
            u32 nameOffset;
            u32 index; // 0 - 0x1e
            u32 flags;
            u8 texgens;
            u8 lightChannels;
            u8 shaderStages;
            u8 indirectTextures;
            u32 cullMode; // IMPORTANT
            u8 alphaFunction;
            u8 lightSet;
            u8 fogSet;
            u8 _1f; // Padding
            u8 indirectMethod[4];
            u8 lightMapRef[4]; // See wiki for this one
            u32 shaderOffset;
            u32 numTextures;
            u32 layerOffset;
            u32 furDataOffset;
            u32 displayListOffsetV8V9;
            u32 displayListOffsetV10V11;
            u32 enableTextureInfoSettings; // 0x40 read the wiki
            u32 _44Code[16][4];            // Size is 32 * 8. Is u32 the best way to do this? An int is 4 for reference
            u32 enablePaletteInfoSettings; // 0x148 read the wiki
            u32 _148Code[6][4];            // Size is 12 * 8. Same concerns as two lines above.
            enum class LayerBitFlags {
                ENABLE_LAYER = 0,
                SCALE_FIX = 1,
                ROTATION_FIX = 2,
                TRANSLATION_FIXED = 3,
            };
            LayerBitFlags flag;

            enum class TextureMatrixMode {
                MAYA = 0,
                XSI = 1,
                _3DS_MAX = 2,
            };
            TextureMatrixMode textureMode;

            u8 _1b0[0x268]; // Too lazy to write it out

            /*
            struct Coordinates {
                f64 scaleCoord;
                f32 rotationCoord;
                f64 translationCoord;
            }; static_assert(sizeof(Coordinates) == 20);
            Coordinates[8];
            */

        }; 
        static_assert(sizeof(BasicModelStructure) == 0x418);

        //Required for layer to be alligned by 20. According to the wiki, there is 0 - 6 instances of layers, and mkw files have 0
        struct Padding {
            u8 _418[0x08 - 0x00];
        };
        static_assert(sizeof(Padding) == 0x08);

        struct Layers {
            u8 _00[0x34 - 0x00];
        };
        static_assert(sizeof(Layers) == 0x34);

        enum class Offsets {
            CLAMP = 0,
            REPEAT = 1,
            MIRROR = 2,
        };

        struct Shader {
            u8 _00[0x180 - 0x00];
        };
        static_assert(sizeof(Shader) == 0x180);
    };

}