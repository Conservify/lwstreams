#include "varint_streams.h"

namespace lws {

VarintEncodedStream::DataBlock VarintEncodedStream::read() {
    if (position == filled) {
        position = 0;
        filled = 0;
    }

    // Fill our buffer if we can.
    auto available = bp.size - filled;
    if (available > 0) {
        auto r = reader->read(bp.ptr + filled, available);
        if (r == Stream::EOS) {
            if (filled == 0) {
                return DataBlock{ nullptr, -1, -1, -1 };
            }
        }
        else {
            filled += r;
        }
    }

    // We don't have a block, so we need a varint to get the size.
    if (totalSize == 0 || totalPosition == totalSize) {
        auto bytes = decodeVarint<int32_t>(bp.ptr + position, &totalSize, filled);
        if (bytes == 0) {
            // If we can't get a size from a full buffer, something has gone horribly wrong.
            totalSize = 0;
            lws_assert(bp.size - filled > 0);
            return DataBlock{ };
        }

        position += bytes;
        totalPosition = 0;
    }

    auto leftInBlock = totalSize - totalPosition;
    auto bytes = filled - position;
    auto blockSize = leftInBlock > bytes ? bytes : leftInBlock;

    if (blockSize == 0) {
        return DataBlock{ };
    }

    auto block = DataBlock{
        bp.ptr + position,
        blockSize,
        totalPosition,
        totalSize,
    };

    position += blockSize;
    totalPosition += blockSize;

    return block;
}

}
