#include <libavcodec/avcodec.h>
static int dvbsub_read_2bit_string(uint8_t *destbuf, int dbuf_len,
                                   const uint8_t **srcbuf, int buf_size,
                                   int non_mod, uint8_t *map_table)
{
    BitstreamContext bc;
    int bits;
    int run_length;
    int pixels_read = 0;
    bitstream_init(&bc, *srcbuf, buf_size << 3);
    while (bitstream_tell(&bc) < buf_size << 3 && pixels_read < dbuf_len) {
        bits = bitstream_read(&bc, 2);
        if (bits) {
            if (non_mod != 1 || bits != 1) {
                if (map_table)
                    *destbuf++ = map_table[bits];
                else
                    *destbuf++ = bits;
            }
            pixels_read++;
        } else {
            bits = bitstream_read_bit(&bc);
            if (bits == 1) {
                run_length = bitstream_read(&bc, 3) + 3;
                bits       = bitstream_read(&bc, 2);
                if (non_mod == 1 && bits == 1)
                    pixels_read += run_length;
                else {
                    if (map_table)
                        bits = map_table[bits];
                    while (run_length-- > 0 && pixels_read < dbuf_len) {
                        *destbuf++ = bits;
                        pixels_read++;
                    }
                }
            } else {
                bits = bitstream_read_bit(&bc);
                if (bits == 0) {
                    bits = bitstream_read(&bc, 2);
                    if (bits == 2) {
                        run_length = bitstream_read(&bc, 4) + 12;
                        bits       = bitstream_read(&bc, 2);
                        if (non_mod == 1 && bits == 1)
                            pixels_read += run_length;
                        else {
                            if (map_table)
                                bits = map_table[bits];
                            while (run_length-- > 0 && pixels_read < dbuf_len) {
                                *destbuf++ = bits;
                                pixels_read++;
                            }
                        }
                    } else if (bits == 3) {
                        run_length = bitstream_read(&bc, 8) + 29;
                        bits = bitstream_read(&bc, 2);
                        if (non_mod == 1 && bits == 1)
                            pixels_read += run_length;
                        else {
                            if (map_table)
                                bits = map_table[bits];
                            while (run_length-- > 0 && pixels_read < dbuf_len) {
                                *destbuf++ = bits;
                                pixels_read++;
                            }
                        }
                    } else if (bits == 1) {
                        pixels_read += 2;
                        if (map_table)
                            bits = map_table[0];
                        else
                            bits = 0;
                        if (pixels_read <= dbuf_len) {
                            *destbuf++ = bits;
                            *destbuf++ = bits;
                        }
                    } else {
                        *srcbuf += (bitstream_tell(&bc) + 7) >> 3;
                        return pixels_read;
                    }
                } else {
                    if (map_table)
                        bits = map_table[0];
                    else
                        bits = 0;
                    *destbuf++ = bits;
                    pixels_read++;
                }
            }
        }
    }
    if (bitstream_read(&bc, 6))
        av_log(NULL, AV_LOG_ERROR, "DVBSub error: line overflow\n");
    *srcbuf += (bitstream_tell(&bc) + 7) >> 3;
    return pixels_read;
}