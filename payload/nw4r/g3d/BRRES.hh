#include <Common.hh>

//All credits for the .brres struct is from https://wiki.tockdom.com/wiki/BRRES_(File_Format)
//Don't know if this is an appropriate file name

namespace nw4r::g3d {
    class BRRES {
        public:
        struct Header {
            
            u8 fileMagic[0x04 - 0x00]; // "bres" in ASCII
            u16 byteOrderMark;
            u8 _08[0x08 - 0x06];
            u32 fileLength;
            u16 rootSectionOffset;
            u16 numSections; //Very important. This includes the root
        };
        static_assert(sizeof(Header) == 0x10);

        struct Root {
            char *sectionMagic;
            u32 sectionLen;
        };
        static_assert(sizeof(Root) == 0x08);

        private:
        Model m_model;
    };


} //namespace nw4r::g3d