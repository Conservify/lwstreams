#include "copier.h"

namespace lws {

void StreamCopier::restart() {
    position = 0;
}

int32_t StreamCopier::copy(Reader &reader, Writer &writer) {
    auto available = buffer.size - position;
    auto eos = false;
    auto copied = 0;

    if (available > 0) {
        auto read = reader.read(buffer.ptr + position, available);
        if (read == Stream::EOS) {
            eos = true;
        }
        else if (read > 0) {
            position += read;
        }
    }

    if (position > 0) {
        auto wrote = writer.write(buffer.ptr, position);
        if (wrote > 0) {
            copied = wrote;
            if (wrote != (int32_t)position) {
                auto remaining = position - wrote;
                memmove(buffer.ptr, buffer.ptr + wrote, remaining);
                position = remaining;
            }
            else {
                position = 0;
            }
        }
    }

    if (position == 0 && eos) {
        return Stream::EOS;
    }

    return copied;
}

}
