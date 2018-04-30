#ifndef LWS_BUFFERS_H_INCLUDED
#define LWS_BUFFERS_H_INCLUDED

#include <cstdint>
#include <cstdlib>
#include <utility>

#include "debug.h"

namespace lws {

class BufferPtr {
public:
    uint8_t *ptr{ nullptr };
    size_t size{ 0 };

public:
    BufferPtr(uint8_t *ptr, size_t size) : ptr(ptr), size(size) { }

public:
    uint8_t &operator[](int32_t index) {
        lws_assert(index >= 0 && index < (int32_t)size);
        return ptr[index];
    }

    uint8_t operator[] (int32_t index) const {
        lws_assert(index >= 0 && index < (int32_t)size);
        return ptr[index];
    }

 };

template<size_t Size>
class AlignedStorageBuffer {
private:
    typename std::aligned_storage<sizeof(uint8_t), alignof(uint8_t)>::type buffer[Size];

public:
    const size_t size{ Size };

public:
    BufferPtr toBufferPtr() {
        return BufferPtr{ (uint8_t *)buffer, Size };
    }

public:
    void clear() {
        for (size_t i = 0; i < Size; ++i) {
            ((uint8_t *)buffer)[i] = 0;
        }
    }

    uint8_t &operator[](int32_t index) {
        lws_assert(index >= 0 && index < (int32_t)Size);
        return ((uint8_t *)buffer)[index];
    }
};

}

#endif
