#pragma once

#include <vendor/nanopb/pb_decode.h>
#include <vendor/nanopb/pb_encode.h>

#include <expected>
#include <memory>

#include <Common.hh>

namespace SP::Net {

typedef const pb_msgdesc_t *pb_msgdesc_p;

template <typename R, typename W, typename S>
class ProtoSocket {
public:
    // Descriptors would be template param, but blocked by:
    // https://github.com/whitequark/python-itanium_demangler/issues/20
    ProtoSocket(S *socket, pb_msgdesc_p readDesc, pb_msgdesc_p writeDesc) : m_inner(socket) {
        m_readDesc = readDesc;
        m_writeDesc = writeDesc;
    }

    [[nodiscard]] std::expected<std::optional<R>, const wchar_t *> readProto() {
        u8 buffer[1024];
        u16 size = TRY_OPT(TRY(m_inner->read(buffer, sizeof(buffer))));

        R ret;
        pb_istream_t stream = pb_istream_from_buffer(buffer, size);
        if (!pb_decode(&stream, m_readDesc, &ret)) {
            return std::unexpected(L"Failed to decode proto");
        }

        return ret;
    }

    [[nodiscard]] std::expected<void, const wchar_t *> writeProto(W message) {
        u8 buffer[1024];

        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        assert(pb_encode(&stream, m_writeDesc, &message));

        return m_inner->write(buffer, stream.bytes_written);
    }

    S &inner() {
        return *m_inner;
    }

    const S &inner() const {
        return *m_inner;
    }

private:
    S *m_inner;

    pb_msgdesc_p m_readDesc = nullptr;
    pb_msgdesc_p m_writeDesc = nullptr;
};

} // namespace SP::Net
