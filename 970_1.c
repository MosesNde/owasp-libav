static int dpcm_decode_frame(AVCodecContext *avctx, void *data, int *data_size,
                             AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    const uint8_t *buf_end = buf + buf_size;
    DPCMContext *s = avctx->priv_data;
    int out = 0;
    int predictor[2];
    int ch = 0;
    int stereo = s->channels - 1;
    int16_t *output_samples = data;
    if (!buf_size)
        return 0;
    switch(avctx->codec->id) {
    case CODEC_ID_ROQ_DPCM:
        out = buf_size - 8;
        break;
    case CODEC_ID_INTERPLAY_DPCM:
        out = buf_size - 6 - s->channels;
        break;
    case CODEC_ID_XAN_DPCM:
        out = buf_size - 2 * s->channels;
        break;
    case CODEC_ID_SOL_DPCM:
        out = buf_size * 4;
        break;
    }
    out *= av_get_bytes_per_sample(avctx->sample_fmt);
    if (out < 0) {
        av_log(avctx, AV_LOG_ERROR, "packet is too small\n");
    }
    if (*data_size < out) {
    }
    switch(avctx->codec->id) {
    case CODEC_ID_ROQ_DPCM:
        buf += 2;
        if (stereo) {
            predictor[1] = (int16_t)(bytestream_get_byte(&buf) << 8);
            predictor[0] = (int16_t)(bytestream_get_byte(&buf) << 8);
        } else {
            predictor[0] = (int16_t)bytestream_get_le16(&buf);
        }
        while (buf < buf_end) {
            predictor[ch] += s->roq_square_array[*buf++];
            *output_samples++ = predictor[ch];
            ch ^= stereo;
        }
        break;
    case CODEC_ID_INTERPLAY_DPCM:
        buf += 6;
        for (ch = 0; ch < s->channels; ch++) {
            predictor[ch] = (int16_t)bytestream_get_le16(&buf);
            *output_samples++ = predictor[ch];
        }
        ch = 0;
        while (buf < buf_end) {
            predictor[ch] += interplay_delta_table[*buf++];
            *output_samples++ = predictor[ch];
            ch ^= stereo;
        }
        break;
    case CODEC_ID_XAN_DPCM:
    {
        int shift[2] = { 4, 4 };
        for (ch = 0; ch < s->channels; ch++)
            predictor[ch] = (int16_t)bytestream_get_le16(&buf);
        ch = 0;
        while (buf < buf_end) {
            uint8_t n = *buf++;
            int16_t diff = (n & 0xFC) << 8;
            if ((n & 0x03) == 3)
                shift[ch]++;
            else
                shift[ch] -= (2 * (n & 3));
            diff >>= shift[ch];
            predictor[ch] += diff;
            *output_samples++ = predictor[ch];
            ch ^= stereo;
        }
        break;
    }
    case CODEC_ID_SOL_DPCM:
        while (buf < buf_end) {
            uint8_t n = *buf++;
            s->sample[ch] += sol_table_16[n & 0x7F];
            *output_samples++ = s->sample[ch];
            ch ^= stereo;
        }
        break;
    }
    *data_size = out;
    return buf_size;
}