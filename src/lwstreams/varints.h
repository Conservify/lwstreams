#ifndef LWS_VARINTS_H_INCLUDED
#define LWS_VARINTS_H_INCLUDED

#include <cstdint>
#include <cstdlib>

namespace lws {

/**
 * Encodes an unsigned variable-length integer using the MSB algorithm.
 * This function assumes that the value is stored as little endian.
 * @param value The input value. Any standard integer type is allowed.
 * @param ptr A pointer to a piece of reserved memory. Must have a minimum size dependent on the input size (32 bit = 5 bytes, 64 bit = 10 bytes).
 * @return The number of bytes used in the output memory.
 */
template<typename int_t = uint64_t>
size_t encodeVarint(int_t value, uint8_t *ptr) {
    size_t outputSize = 0;
    // While more than 7 bits of data are left, occupy the last output byte
    // and set the next byte flag
    while (value > 127) {
        //|128: Set the next byte flag
        ptr[outputSize] = ((uint8_t)(value & 127)) | 128;
        //Remove the seven bits we just wrote
        value >>= 7;
        outputSize++;
    }
    ptr[outputSize++] = ((uint8_t)value) & 127;
    return outputSize;
}

/**
 * Decodes an unsigned variable-length integer using the MSB algorithm.
 * @param value A variable-length encoded integer of arbitrary size.
 * @param inputSize How many bytes are
 */
template<typename int_t = uint64_t>
size_t decodeVarint(uint8_t *ptr, int_t *ret, size_t size) {
    *ret = 0;
    for (size_t i = 0; i < size; i++) {
        *ret |= (ptr[i] & 127) << (7 * i);
        //If the next-byte flag is set
        if (!(ptr[i] & 128)) {
            return i + 1;
        }
    }
    return 0;
}

}

#endif
