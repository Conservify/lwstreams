#ifndef LWS_DIRECT_STREAMS_H_INCLUDED
#define LWS_DIRECT_STREAMS_H_INCLUDED

namespace lws {

class DirectWriter : public Writer {
private:
    BufferPtr buffer;
    int32_t position{ 0 };

public:
    DirectWriter(BufferPtr buffer) : buffer(buffer) {
    }

public:
    using Writer::write;

    int32_t write(uint8_t *ptr, size_t size) override {
        if (position == EOS) {
            return EOS;
        }
        auto available = buffer.size - position;
        auto copying = size > available ? available : size;
        if (copying > 0) {
            memcpy(buffer.ptr + position, ptr, copying);
            position += copying;
        }
        return copying;
    }

    int32_t write(uint8_t byte) override {
        if (position == EOS) {
            return EOS;
        }
        if (position == (int32_t)buffer.size) {
            return EOS;
        }
        buffer.ptr[position++] = byte;
        return 1;
    }

    void close() override {
        position = EOS;
    }

public:
    int32_t size() {
        return position;
    }

    int32_t available() {
        return buffer.size - position;
    }

    BufferPtr toBufferPtr() {
        lws_assert(position >= 0);
        return BufferPtr{ buffer.ptr, (size_t)position };
    }

protected:
    uint8_t *ptr() {
        return buffer.ptr + position;
    }

    int32_t seek(int32_t bytes) {
        position += bytes;
        return position;
    }

};

class DirectReader : public Reader {
private:
    BufferPtr buffer;
    int32_t position{ 0 };

public:
    DirectReader(BufferPtr buffer) : buffer(buffer) {
    }

public:
    using Reader::read;

    int32_t read() override {
        if (position == EOS) {
            return EOS;
        }
        if (position >= (int32_t)buffer.size) {
            return EOS;
        }
        return buffer.ptr[position++];
    }

    void close() override {
        position = EOS;
    }

public:
    int32_t available() {
        return buffer.size - position;
    }

};

}

#endif
