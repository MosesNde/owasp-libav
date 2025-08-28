static int spdif_header_dts4(AVFormatContext *s, AVPacket *pkt, int core_size,
                             int sample_rate, int blocks)
{
    IEC61937Context *ctx = s->priv_data;
    static const char dtshd_start_code[10] = { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe };
    int pkt_size = pkt->size;
    int period;
    int subtype;
    if (!core_size) {
        av_log(s, AV_LOG_ERROR, "HD mode not supported for this format\n");
        return AVERROR(EINVAL);
    }
    if (!sample_rate) {
        av_log(s, AV_LOG_ERROR, "Unknown DTS sample rate for HD\n");
        return AVERROR_INVALIDDATA;
    }
    period = ctx->dtshd_rate * (blocks << 5) / sample_rate;
    subtype = spdif_dts4_subtype(period);
    if (subtype < 0) {
        av_log(s, AV_LOG_ERROR, "Specified HD rate of %d Hz would require an "
               "impossible repetition period of %d for the current DTS stream"
               " (blocks = %d, sample rate = %d)\n", ctx->dtshd_rate, period,
               blocks << 5, sample_rate);
        return AVERROR(EINVAL);
    }
    ctx->pkt_offset = period * 4;
    ctx->data_type = IEC61937_DTSHD | subtype << 8;
    if (sizeof(dtshd_start_code) + 2 + pkt_size
            > ctx->pkt_offset - BURST_HEADER_SIZE && core_size) {
        if (!ctx->dtshd_skip)
            av_log(s, AV_LOG_WARNING, "DTS-HD bitrate too high, "
                                      "temporarily sending core only\n");
        if (ctx->dtshd_fallback > 0)
            ctx->dtshd_skip = sample_rate * ctx->dtshd_fallback / (blocks << 5);
        else
            ctx->dtshd_skip = 1;
    }
    if (ctx->dtshd_skip && core_size) {
        pkt_size = core_size;
        if (ctx->dtshd_fallback >= 0)
            --ctx->dtshd_skip;
    }
    ctx->out_bytes   = sizeof(dtshd_start_code) + 2 + pkt_size;
    ctx->length_code = FFALIGN(ctx->out_bytes + 0x8, 0x10) - 0x8;
    av_fast_malloc(&ctx->hd_buf, &ctx->hd_buf_size, ctx->out_bytes);
    if (!ctx->hd_buf)
        return AVERROR(ENOMEM);
    ctx->out_buf = ctx->hd_buf;
    memcpy(ctx->hd_buf, dtshd_start_code, sizeof(dtshd_start_code));
    AV_WB16(ctx->hd_buf + sizeof(dtshd_start_code), pkt_size);
    memcpy(ctx->hd_buf + sizeof(dtshd_start_code) + 2, pkt->data, pkt_size);
    return 0;
}

