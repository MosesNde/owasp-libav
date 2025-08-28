static int shorten_decode_frame(AVCodecContext *avctx,
        void *data, int *data_size,
        AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    ShortenContext *s = avctx->priv_data;
    int i, input_buf_size = 0;
    int16_t *samples = data;
    int ret;
    if(s->max_framesize == 0){
        void *tmp_ptr;
        s->max_framesize= 1024;
        tmp_ptr = av_fast_realloc(s->bitstream, &s->allocated_bitstream_size,
                                  s->max_framesize);
        if (!tmp_ptr) {
            av_log(avctx, AV_LOG_ERROR, "error allocating bitstream buffer\n");
            return AVERROR(ENOMEM);
        }
        s->bitstream = tmp_ptr;
    }
    if(1 && s->max_framesize){
        buf_size= FFMIN(buf_size, s->max_framesize - s->bitstream_size);
        input_buf_size= buf_size;
        if(s->bitstream_index + s->bitstream_size + buf_size > s->allocated_bitstream_size){
            memmove(s->bitstream, &s->bitstream[s->bitstream_index], s->bitstream_size);
            s->bitstream_index=0;
        }
        memcpy(&s->bitstream[s->bitstream_index + s->bitstream_size], buf, buf_size);
        buf= &s->bitstream[s->bitstream_index];
        buf_size += s->bitstream_size;
        s->bitstream_size= buf_size;
        if(buf_size < s->max_framesize){
            *data_size = 0;
            return input_buf_size;
        }
    }
    init_get_bits(&s->gb, buf, buf_size*8);
    skip_bits(&s->gb, s->bitindex);
    if (!s->got_header) {
        if ((ret = read_header(s)) < 0)
            return ret;
        *data_size = 0;
    }
    else
    {
        int cmd;
        int len;
        cmd = get_ur_golomb_shorten(&s->gb, FNSIZE);
        if (cmd > FN_VERBATIM) {
            av_log(avctx, AV_LOG_ERROR, "unknown shorten function %d\n", cmd);
            if (s->bitstream_size > 0) {
                s->bitstream_index++;
                s->bitstream_size--;
            }
            return -1;
        }
        if (!is_audio_command[cmd]) {
            switch (cmd) {
                case FN_VERBATIM:
                    len = get_ur_golomb_shorten(&s->gb, VERBATIM_CKSIZE_SIZE);
                    while (len--) {
                        get_ur_golomb_shorten(&s->gb, VERBATIM_BYTE_SIZE);
                    }
                    break;
                case FN_BITSHIFT:
                    s->bitshift = get_ur_golomb_shorten(&s->gb, BITSHIFTSIZE);
                    break;
                case FN_BLOCKSIZE: {
                    int blocksize = get_uint(s, av_log2(s->blocksize));
                    if (blocksize > s->blocksize) {
                        av_log(avctx, AV_LOG_ERROR, "Increasing block size is not supported\n");
                        return AVERROR_PATCHWELCOME;
                    }
                    if (!blocksize || blocksize > MAX_BLOCKSIZE) {
                        av_log(avctx, AV_LOG_ERROR, "invalid or unsupported "
                               "block size: %d\n", blocksize);
                        return AVERROR(EINVAL);
                    }
                    s->blocksize = blocksize;
                    break;
                }
                case FN_QUIT:
                    break;
            }
            *data_size = 0;
        } else {
            int residual_size = 0;
            int channel = s->cur_chan;
            int32_t coffset;
            if (cmd != FN_ZERO) {
                residual_size = get_ur_golomb_shorten(&s->gb, ENERGYSIZE);
                if (s->version == 0)
                    residual_size--;
            }
            if (s->nmean == 0)
                coffset = s->offset[channel][0];
            else {
                int32_t sum = (s->version < 2) ? 0 : s->nmean / 2;
                for (i=0; i<s->nmean; i++)
                    sum += s->offset[channel][i];
                coffset = sum / s->nmean;
                if (s->version >= 2)
                    coffset >>= FFMIN(1, s->bitshift);
            }
            if (cmd == FN_ZERO) {
                for (i=0; i<s->blocksize; i++)
                    s->decoded[channel][i] = 0;
            } else {
                if ((ret = decode_subframe_lpc(s, cmd, channel, residual_size, coffset)) < 0)
                    return ret;
            }
            if (s->nmean > 0) {
                int32_t sum = (s->version < 2) ? 0 : s->blocksize / 2;
                for (i=0; i<s->blocksize; i++)
                    sum += s->decoded[channel][i];
                for (i=1; i<s->nmean; i++)
                    s->offset[channel][i-1] = s->offset[channel][i];
                if (s->version < 2)
                    s->offset[channel][s->nmean - 1] = sum / s->blocksize;
                else
                    s->offset[channel][s->nmean - 1] = (sum / s->blocksize) << s->bitshift;
            }
            for (i=-s->nwrap; i<0; i++)
                s->decoded[channel][i] = s->decoded[channel][i + s->blocksize];
            fix_bitshift(s, s->decoded[channel]);
            s->cur_chan++;
            if (s->cur_chan == s->channels) {
                int out_size = s->blocksize * s->channels *
                               av_get_bytes_per_sample(avctx->sample_fmt);
                if (*data_size < out_size) {
                    av_log(avctx, AV_LOG_ERROR, "Output buffer is too small\n");
                    return AVERROR(EINVAL);
                }
                samples = interleave_buffer(samples, s->channels, s->blocksize, s->decoded);
                s->cur_chan = 0;
                *data_size = out_size;
            } else {
                *data_size = 0;
            }
        }
    }
    s->bitindex = get_bits_count(&s->gb) - 8*((get_bits_count(&s->gb))/8);
    i= (get_bits_count(&s->gb))/8;
    if (i > buf_size) {
        av_log(s->avctx, AV_LOG_ERROR, "overread: %d\n", i - buf_size);
        s->bitstream_size=0;
        s->bitstream_index=0;
        return -1;
    }
    if (s->bitstream_size) {
        s->bitstream_index += i;
        s->bitstream_size  -= i;
        return input_buf_size;
    } else
        return i;
}

static int get_ur_golomb_shorten(GetBitContext *gb, int max) {
    int val = get_bits(gb, 8);
    if(val > max) {
        char buf[256];
        snprintf(buf, sizeof(buf), "%d", val);
        char *ptr = buf;
        while(*ptr) {
            if(*ptr < '0' || *ptr > '9') {
                av_log(NULL, AV_LOG_ERROR, "Injection detected: invalid character in input\n");
                return -1;
            }
            ptr++;
        }
        return val;
    }
    return val;
}