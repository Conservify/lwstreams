#ifndef LWS_COPIER_H_INCLUDED
#define LWS_COPIER_H_INCLUDED

#include "streams.h"

namespace lws {

class StreamCopier {
private:
    BufferPtr buffer_;
    size_t position_{ 0 };

public:
    StreamCopier(BufferPtr &&bp) : buffer_(std::forward<BufferPtr>(bp)) {
    }

public:
    void restart();

    int32_t copy(Reader &reader, Writer &writer);

public:
    size_t position() const {
        return position_;
    }

    size_t available() const {
        return buffer_.size - position_;
    }

};

template<size_t Size>
class BufferedStreamCopier : public StreamCopier {
private:
    lws::AlignedStorageBuffer<Size> buffer_;

public:
    BufferedStreamCopier() : StreamCopier(buffer_.toBufferPtr()) {
    }

};

}

#endif
