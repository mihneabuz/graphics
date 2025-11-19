#ifndef COMPRESS_H
#define COMPRESS_H

#include <stdint.h>
#include <zlib.h>

static inline uint32_t zlib_inflate(uint8_t* out,
                                    uint32_t out_size,
                                    uint8_t* in,
                                    uint32_t in_size) {
    z_stream stream = {
        .next_in = in,
        .avail_in = in_size,
        .next_out = out,
        .avail_out = out_size,
    };

    if (inflateInit(&stream) != Z_OK)
        return 0;

    int ret = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (ret != Z_STREAM_END)
        return 0;

    return stream.total_out;
}

#endif
