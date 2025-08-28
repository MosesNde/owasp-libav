static int decode_frame(AVCodecContext *avctx,
                        void *data, int *data_size, AVPacket *avpkt)
{
    const uint8_t *buf = avpkt->data;
    int buf_size = avpkt->size;
    TiffContext *const s = avctx->priv_data;
    AVFrame *picture = data;
    AVFrame *const p = &s->picture;
    const uint8_t *orig_buf = buf, *end_buf = buf + buf_size;
    unsigned off;
    int id, le, ret;
    int i, j, entries;
    int stride;
    unsigned soff, ssize;
    uint8_t *dst;
    if (end_buf - buf < 8)
        return AVERROR_INVALIDDATA;
    id = AV_RL16(buf);
    buf += 2;
    if (id == 0x4949)
        le = 1;
    else if (id == 0x4D4D)
        le = 0;
    else {
        av_log(avctx, AV_LOG_ERROR, "TIFF header not found\n");
        return -1;
    }
    s->le = le;
    s->invert = 0;
    s->compr = TIFF_RAW;
    s->fill_order = 0;
    if (tget_short(&buf, le) != 42) {
        av_log(avctx, AV_LOG_ERROR,
               "The answer to life, universe and everything is not correct!\n");
        return -1;
    }
    s->stripsizes = s->stripdata = NULL;
    off = tget_long(&buf, le);
    buf = orig_buf + off;
    entries = tget_short(&buf, le);
    for (i = 0; i < entries; i++) {
        if (tiff_decode_tag(s, orig_buf, buf, end_buf) < 0)
            return -1;
        buf += 12;
    }
    if (!s->stripdata && !s->stripoff) {
        av_log(avctx, AV_LOG_ERROR, "Image data is missing\n");
        return -1;
    }
    if ((ret = init_image(s)) < 0)
        return ret;
    if (s->strips == 1 && !s->stripsize) {
        s->stripsize = buf_size - s->stripoff;
    }
    stride = p->linesize[0];
    dst = p->data[0];
    for (i = 0; i < s->height; i += s->rps) {
        if (s->stripsizes) {
            ssize = tget(&s->stripsizes, s->sstype, s->le);
        } else
            ssize = s->stripsize;
        if (s->stripdata) {
            soff = tget(&s->stripdata, s->sot, s->le);
        } else
            soff = s->stripoff;
        if (soff > buf_size || ssize > buf_size - soff) {
            av_log(avctx, AV_LOG_ERROR, "Invalid strip size/offset\n");
            return -1;
        }
        if (tiff_unpack_strip(s, dst, stride, orig_buf + soff, ssize,
                              FFMIN(s->rps, s->height - i)) < 0)
            break;
        dst += s->rps * stride;
    }
    if (s->predictor == 2) {
        dst = p->data[0];
        soff = s->bpp >> 3;
        ssize = s->width * soff;
        for (i = 0; i < s->height; i++) {
            for (j = soff; j < ssize; j++)
                dst[j] += dst[j - soff];
            dst += stride;
        }
    }
    if (s->invert) {
        uint8_t *src;
        int j;
        src = s->picture.data[0];
        for (j = 0; j < s->height; j++) {
            for (i = 0; i < s->picture.linesize[0]; i++)
                src[i] = 255 - src[i];
            src += s->picture.linesize[0];
        }
    }
    *picture   = s->picture;
    *data_size = sizeof(AVPicture);
    return buf_size;
}