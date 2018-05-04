#ifndef LWS_COPIER_H_INCLUDED
#define LWS_COPIER_H_INCLUDED

#include "streams.h"

namespace lws {

class StreamCopier {
private:
    BufferPtr buffer;
    size_t position{ 0 };

public:
    StreamCopier(BufferPtr &&bp) : buffer(std::forward<BufferPtr>(bp)) {
    }

public:
    int32_t copy(Reader &reader, Writer &writer);

};

template<size_t Size>
class BufferedStreamCopier : public StreamCopier {
private:
    lws::AlignedStorageBuffer<Size> buffer;

public:
    BufferedStreamCopier() : StreamCopier(buffer.toBufferPtr()) {
    }

};

}

#endif
