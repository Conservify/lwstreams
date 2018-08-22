#include "copier.h"

namespace lws {

void StreamCopier::restart() {
    position_ = 0;
}

int32_t StreamCopier::copy(Reader &reader, Writer &writer) {
    auto available = buffer_.size - position_;
    auto eos = false;
    auto copied = 0;

    if (available > 0) {
        auto read = reader.read(buffer_.ptr + position_, available);
        if (read == Stream::EOS) {
            eos = true;
        }
        else if (read > 0) {
            position_ += read;
        }
    }

    if (position_ > 0) {
        auto wrote = writer.write(buffer_.ptr, position_);
        if (wrote == Stream::EOS) {
            return Stream::EOS;
        }
        if (wrote > 0) {
            copied = wrote;
            if (wrote != (int32_t)position_) {
                auto remaining = position_ - wrote;
                memmove(buffer_.ptr, buffer_.ptr + wrote, remaining);
                position_ = remaining;
            }
            else {
                position_ = 0;
            }
        }
    }

    if (copied == 0 && position_ == 0 && eos) {
        return Stream::EOS;
    }

    return copied;
}

}
