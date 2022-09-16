#include <Common.hh>
//Credits to https://wiki.tockdom.com/wiki/MDL0_(File_Format). Thanks guys

namespace nw4r::g3d {
    class Model {
        public:

        /* //This is just the generic structure of a BRRES subfile header
        struct Header { 
            u8 magicMdl[0x04 - 0x00];
            u32 subFileLen;
            u32 subFileVersionNum;
            u32 offset; //offset to outer BRRES file (negative value)
            u32 nSectionOffsets[subFileVersionNum]; //Probably completely wrong here
            u8 stringOffset[0x04];
        };
        static_assert(sizeof(Header) == 0x14 + Header.subFileVersionNum * 4);
        */

        struct Header {
            u32 headerLen; //0x40
            u32 fileHeaderOffset;
            u32 unknown1;
            u32 unknown2;
            u32 vertexCount;
            u32 faceCount;
            u32 unknown3;
            u32 boneCount;
            u32 unknown4; //Always 0x01000000
            u32 boneTableOffset;
            f32 minimum[3]; //Could be wrong. Wiki says float3
            f32 maximum[3]; //Same as above comment
        };
        static_assert(sizeof(Header) == 0x40);



        /* //This is just confusing how I can write this when the size is important but not yet set.
        struct BoneLinkTable {
            u32 entryCount;
            u32 nodeIndex[]; //-1 if not present
        };
        static_assert(sizeof(BoneLinkTable) == 0x08);
        */




    };
    
}