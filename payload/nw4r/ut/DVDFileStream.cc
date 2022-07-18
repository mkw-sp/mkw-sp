#include "DVDFileStream.hh"

namespace nw4r::ut {

DVDFileStream::~DVDFileStream() = default;

DVDFileStream::dt(s32 type) {
    if (type > 0) {
        delete this;
    } else {
        this->~DVDFileStream();
    }
}



}
