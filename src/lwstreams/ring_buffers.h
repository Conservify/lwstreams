#ifndef LWS_RING_BUFFERS_H_INCLUDED
#define LWS_RING_BUFFERS_H_INCLUDED

#include <utility>

namespace lws {

constexpr bool is_power_of_2(int32_t v) {
    return v && ((v & (v - 1)) == 0);
}

template<typename T>
class RingBufferG {
private:
    T bp;
    volatile uint32_t read{ 0 };
    volatile uint32_t write{ 0 };

public:
    RingBufferG() {
    }

    RingBufferG(T bp) : bp(bp) {
        lws_assert(is_power_of_2(bp.size));
    }

    RingBufferG(T &&bp) : bp(std::forward<T>(bp)) {
        lws_assert(is_power_of_2(bp.size));
    }

    void clear() {
        read = write = 0;
        bp.clear();
    }

    void push(uint8_t c) {
        lws_assert(!full());
        bp[mask(write++)] = c;
    }

    uint8_t shift() {
        lws_assert(!empty());
        return bp[mask(read++)];
    }

    uint32_t available() {
        return bp.size - size();
    }

    uint32_t size() {
        return write - read;
    }

    bool empty() {
        return read == write;
    }

    bool full() {
        return size() == bp.size;
    }

private:
    uint32_t mask(uint32_t i) {
        return i & (bp.size - 1);
    }

};

typedef RingBufferG<BufferPtr> RingBufferPtr;

template<size_t Size>
class RingBufferN : public RingBufferG<AlignedStorageBuffer<Size>> {
public:
    RingBufferN() {
    }
};

}

#endif
