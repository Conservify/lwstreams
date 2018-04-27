#include <lwstreams/streams.h>

class CountingReader : public lws::Reader {
private:
    uint8_t counter{ 0 };
    uint32_t total{ 0 };
    uint32_t position{ 0 };

public:
    CountingReader(uint32_t total) : total(total) {
    }

public:
    int32_t read() override {
        lws_assert(false);
        return EOS;
    }

    void close() override {
    }

    int32_t read(uint8_t *ptr, size_t size) override {
        auto remaining = total - position;
        auto bytes = size > remaining ? remaining : (int32_t)size;

        if (remaining == 0) {
            return EOS;
        }

        for (auto i = 0; i < bytes; ++i) {
            *ptr++ = counter++;
        }

        position += bytes;

        return bytes;
    }

};
