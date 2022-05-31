#include "LZMA.hh"

#include <common/Bytes.hh>

extern "C" {
#include <lzma/LzmaDec.h>
}

namespace LZMA {

#define NUM_BASE_PROBS 1984
#define LZMA_LIT_SIZE 0x300

#define LzmaProps_GetNumProbs(p) (NUM_BASE_PROBS + ((UInt32)LZMA_LIT_SIZE << ((p)->lc + (p)->lp)))

static const size_t HEADER_SIZE = LZMA_PROPS_SIZE + sizeof(u64);

std::optional<size_t> Decode(const u8 *src, u8 *dst, size_t srcSize, size_t dstSize) {
    CLzmaDec dec;
    LzmaDec_Construct(&dec);
    if (srcSize < HEADER_SIZE) {
        return {};
    }
    u64 expectedDstSize = Bytes::Read<u64, std::endian::little>(src, LZMA_PROPS_SIZE);
    bool knownDstSize = expectedDstSize != UINT64_MAX;
    if (knownDstSize) {
        if (expectedDstSize > dstSize) {
            return {};
        }
        dstSize = expectedDstSize;
    }
    if (LzmaProps_Decode(&dec.prop, src, LZMA_PROPS_SIZE) != SZ_OK) {
        return {};
    }
    dec.numProbs = LzmaProps_GetNumProbs(&dec.prop);
    if (static_cast<u64>(dec.numProbs) * sizeof(CLzmaProb) > 0x1000000) {
        return {};
    }
    dec.probs = reinterpret_cast<CLzmaProb *>(0x93000000);
    dec.probs_1664 = dec.probs + 1664;
    LzmaDec_Init(&dec);
    src += HEADER_SIZE;
    srcSize -= HEADER_SIZE;
    dec.dic = dst;
    dec.dicBufSize = dstSize;
    size_t srcOffset = srcSize;
    ELzmaFinishMode finishMode = knownDstSize ? LZMA_FINISH_END : LZMA_FINISH_ANY;
    ELzmaStatus status;
    if (LzmaDec_DecodeToDic(&dec, dec.dicBufSize, src, &srcOffset, finishMode, &status) != SZ_OK) {
        return {};
    }
    if ((knownDstSize && status != LZMA_STATUS_MAYBE_FINISHED_WITHOUT_MARK) ||
            (!knownDstSize && status != LZMA_STATUS_FINISHED_WITH_MARK)) {
        return {};
    }
    return dec.dicPos;
}

} // namespace LZMA
