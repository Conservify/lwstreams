#ifndef LWS_STREAMS_H_INCLUDED
#define LWS_STREAMS_H_INCLUDED

#include <utility>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "debug.h"
#include "buffers.h"

namespace lws {

class Stream {
public:
    static constexpr int32_t EOS = -1;

};

class Writer : public Stream {
public:
    virtual int32_t write(uint8_t *ptr, size_t size) = 0;
    virtual int32_t write(uint8_t byte) = 0;
    virtual void close() = 0;

public:
    int32_t write(const char *str) {
        return write((uint8_t *)str, strlen(str));
    }
};

class Reader : public Stream {
public:
    virtual int32_t read() = 0;
    virtual void close() = 0;

public:
    virtual int32_t read(uint8_t *ptr, size_t size) {
        for (size_t i = 0; i < size; ++i) {
            auto r = read();
            if (r < 0) {
                if (i == 0) {
                    return EOS;
                }
                return i;
            }
            ptr[i] = r;
        }
        return size;
    }
};

class BufferedReader : public Reader {
private:
    BufferPtr buffer;
    int32_t position{ 0 };
    int32_t buffered{ 0 };

public:
    BufferedReader(BufferPtr buffer) : buffer(buffer) {
    }

public:
    using Reader::read;

    int32_t read() override {
        if (replenish() < 0) {
            return EOS;
        }
        if (position >= (int32_t)buffer.size) {
            return EOS;
        }
        return buffer.ptr[position++];
    }

    void close() override {
    }

protected:
    virtual int32_t fill(BufferPtr &buffer) = 0;

    int32_t replenish() {
        if (buffered == 0 || buffered == position) {
            buffered = fill(buffer);
            position = 0;
        }
        if (buffered < 0) {
            return EOS;
        }
        return buffered;
    }

};

}

#endif
