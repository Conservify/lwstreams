#ifndef LWS_PBTEST_H_INCLUDED
#define LWS_PBTEST_H_INCLUDED

#include <cstring>
#include <cstdlib>

#include <pb_encode.h>
#include <pb_decode.h>

bool pb_encode_string(pb_ostream_t *stream, const pb_field_t *field, void *const *arg);
bool pb_decode_string(pb_istream_t *stream, const pb_field_t *, void **arg);

#endif
