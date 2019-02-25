#ifndef LWS_PROTOBUF_H_INCLUDED
#define LWS_PROTOBUF_H_INCLUDED

#ifdef LWS_ENABLE_PROTOBUF

#include <pb_encode.h>
#include <pb_decode.h>

namespace lws {

class ProtoBufMessageWriter {
private:
    static constexpr uint32_t MaximumVarIntSize = 4;
    Writer *target;

public:
    ProtoBufMessageWriter(Writer &target) : target(&target) {
    }

    int32_t write(const pb_msgdesc_t *fields, void *message) {
        size_t required = 0;

        if (!pb_get_encoded_size(&required, fields, message)) {
            return 0;
        }

        uint8_t buffer[required + MaximumVarIntSize];
        auto stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        if (!pb_encode_delimited(&stream, fields, message)) {
            return 0;
        }

        target->write(buffer, stream.bytes_written);

        return stream.bytes_written;
    }

};

class ProtoBufMessageReader {
private:
    Reader *target;

public:
    ProtoBufMessageReader(Reader &target) : target(&target) {
    }

    template<size_t Size>
    int32_t read(const pb_msgdesc_t *fields, void *message) {
        uint8_t buffer[Size];
        auto bytes = target->read(buffer, sizeof(buffer));
        if (bytes < 0) {
            return bytes;
        }

        auto stream = pb_istream_from_buffer(buffer, bytes);
        if (!pb_decode_delimited(&stream, fields, message)) {
            return Stream::EOS;
        }

        return bytes;
    }

};

}

#endif

#endif
