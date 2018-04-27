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

}

#endif
