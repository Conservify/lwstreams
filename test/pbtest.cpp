#include "pbtest.h"

bool pb_encode_string(pb_ostream_t *stream, const pb_field_t *field, void *const *arg) {
    if (!pb_encode_tag_for_field(stream, field)) {
        return false;
    }

    auto str = (const char *)*arg;
    if (str == nullptr) {
        return pb_encode_string(stream, (uint8_t *)nullptr, 0);
    }

    return pb_encode_string(stream, (uint8_t *)str, strlen(str));
}

bool pb_decode_string(pb_istream_t *stream, const pb_field_t *, void **arg) {
    auto len = stream->bytes_left;

    if (len == 0) {
        (*arg) = (void *)"";
        return true;
    }

    auto *ptr = (uint8_t *)malloc(len + 1); // HACK: LEAK
    if (!pb_read(stream, ptr, len)) {
        return false;
    }

    ptr[len] = 0;

    (*arg) = (void *)ptr;

    return true;
}
