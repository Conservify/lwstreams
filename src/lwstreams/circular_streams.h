#ifndef LWS_CIRCULAR_STREAMS_H_INCLUDED
#define LWS_CIRCULAR_STREAMS_H_INCLUDED

#include <utility>

namespace lws {

template<typename RingBufferType> class RingReader;
template<typename RingBufferType> class RingWriter;

template<typename RingBufferType>
class RingReader : public Reader {
private:
    RingBufferType *buffer;
    RingWriter<RingBufferType> *other;
    bool closed{ false };

    friend class RingWriter<RingBufferType>;

public:
    RingReader(RingBufferType *buffer, RingWriter<RingBufferType> *writer) : buffer(buffer), other(writer) {
    }

public:
    int32_t read(uint8_t *ptr, size_t size) override;

    int32_t read() override;

    void close() override;
};

template<typename RingBufferType>
class RingWriter : public Writer {
private:
    RingBufferType *buffer;
    RingReader<RingBufferType> *other;
    bool closed{ false };

    friend class RingReader<RingBufferType>;

public:
    RingWriter(RingBufferType *buffer, RingReader<RingBufferType> *reader) : buffer(buffer), other(reader) {
    }

public:
    using Writer::write;

    int32_t write(uint8_t *ptr, size_t size) override;

    int32_t write(uint8_t byte) override;

    void close() override;
};

template<typename RingBufferType>
int32_t RingReader<RingBufferType>::read(uint8_t *ptr, size_t size) {
    if (buffer->empty()) {
        if (other->closed) {
            return EOS;
        }
        return 0;
    }

    return Reader::read(ptr, size);
}

template<typename RingBufferType>
int32_t RingReader<RingBufferType>::read() {
    // Not totally happy with this. Don't have any way of
    // differentiating between empty and EoS though.
    if (buffer->empty()) {
        return EOS;
    }
    return buffer->shift();
}

template<typename RingBufferType>
void RingReader<RingBufferType>::close() {
    closed = true;
}

template<typename RingBufferType>
int32_t RingWriter<RingBufferType>::write(uint8_t *ptr, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (buffer->full()) {
            return i;
        }
        buffer->push(ptr[i]);
    }
    return size;
}

template<typename RingBufferType>
int32_t RingWriter<RingBufferType>::write(uint8_t byte) {
    if (buffer->full()) {
        return EOS;
    }
    buffer->push(byte);
    return 1;
}

template<typename RingBufferType>
void RingWriter<RingBufferType>::close() {
    closed = true;
}

class Pipe {
public:
    virtual Writer &getWriter() = 0;
    virtual Reader &getReader() = 0;
    virtual void clear() = 0;
};

template<typename RingBufferType>
class CircularStreams : public Pipe {
    using OuterType = CircularStreams<RingBufferType>;

    RingBufferType buffer_;
    RingReader<RingBufferType> reader_{ &buffer_, &writer_ };
    RingWriter<RingBufferType> writer_{ &buffer_, &reader_ };

public:
    CircularStreams() {
    }

    CircularStreams(RingBufferType &&buffer) : buffer_(std::forward<RingBufferType>(buffer)) {
    }

public:
    Writer &getWriter() override {
        return writer_;
    }

    Reader &getReader() override {
        return reader_;
    }

    RingBufferType &buffer() {
        return buffer_;
    }

    void clear() override {
        buffer_.clear();
        reader_ = RingReader<RingBufferType>{ &buffer_, &writer_ };
        writer_ = RingWriter<RingBufferType>{ &buffer_, &reader_ };
    }

};

}

#endif
