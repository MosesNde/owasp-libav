static int mpegts_write_packet_internal(AVFormatContext *s, AVPacket *pkt)
{
    AVStream *st = s->streams[pkt->stream_index];
    int size = pkt->size;
    uint8_t *buf = pkt->data;
    uint8_t *data = NULL;
    MpegTSWrite *ts = s->priv_data;
    MpegTSWriteStream *ts_st = st->priv_data;
    const uint64_t delay = av_rescale(s->max_delay, 90000, AV_TIME_BASE) * 2;
    int64_t dts = AV_NOPTS_VALUE, pts = AV_NOPTS_VALUE;
    if (ts->reemit_pat_pmt) {
        ts->reemit_pat_pmt = 0;
        ts->flags         |= MPEGTS_FLAG_REEMIT_PAT_PMT;
    }
    if (ts->flags & MPEGTS_FLAG_REEMIT_PAT_PMT) {
        ts->pat_packet_count = ts->pat_packet_period - 1;
        ts->sdt_packet_count = ts->sdt_packet_period - 1;
        ts->flags           &= ~MPEGTS_FLAG_REEMIT_PAT_PMT;
    }
    if (pkt->pts != AV_NOPTS_VALUE)
        pts = pkt->pts + delay;
    if (pkt->dts != AV_NOPTS_VALUE)
        dts = pkt->dts + delay;
    if (ts_st->first_pts_check && pts == AV_NOPTS_VALUE) {
        return AVERROR(EINVAL);
    }
    ts_st->first_pts_check = 0;
    if (st->codec->codec_id == AV_CODEC_ID_H264) {
        const uint8_t *p = buf, *buf_end = p + size;
        uint32_t state = -1;
        if (pkt->size < 5 || AV_RB32(pkt->data) != 0x0000001) {
            return AVERROR(EINVAL);
        }
        do {
            p = avpriv_find_start_code(p, buf_end, &state);
        } while (p < buf_end && (state & 0x1f) != 9 &&
                 (state & 0x1f) != 5 && (state & 0x1f) != 1);
        if ((state & 0x1f) != 9) {
            data = av_malloc(pkt->size + 6);
            if (!data)
                return AVERROR(ENOMEM);
            memcpy(data + 6, pkt->data, pkt->size);
            AV_WB32(data, 0x00000001);
            data[4] = 0x09;
            data[5] = 0xf0;
            buf     = data;
            size    = pkt->size + 6;
        }
    } else if (st->codec->codec_id == AV_CODEC_ID_AAC) {
        if (pkt->size < 2) {
            return AVERROR(EINVAL);
        }
        if ((AV_RB16(pkt->data) & 0xfff0) != 0xfff0) {
            int ret;
            AVPacket pkt2;
            if (!ts_st->amux) {
                return AVERROR(EINVAL);
            }
            av_init_packet(&pkt2);
            pkt2.data = pkt->data;
            pkt2.size = pkt->size;
            ret = avio_open_dyn_buf(&ts_st->amux->pb);
            if (ret < 0)
                return AVERROR(ENOMEM);
            ret = av_write_frame(ts_st->amux, &pkt2);
            if (ret < 0) {
                avio_close_dyn_buf(ts_st->amux->pb, &data);
                ts_st->amux->pb = NULL;
                av_free(data);
                return ret;
            }
            size            = avio_close_dyn_buf(ts_st->amux->pb, &data);
            ts_st->amux->pb = NULL;
            buf             = data;
        }
    }
    if (st->codec->codec_type != AVMEDIA_TYPE_AUDIO) {
        mpegts_write_pes(s, st, buf, size, pts, dts,
                         pkt->flags & AV_PKT_FLAG_KEY);
        av_free(data);
        return 0;
    }
    if (ts_st->payload_size + size > ts->pes_payload_size) {
        if (ts_st->payload_size) {
            mpegts_write_pes(s, st, ts_st->payload, ts_st->payload_size,
                             ts_st->payload_pts, ts_st->payload_dts,
                             ts_st->payload_flags & AV_PKT_FLAG_KEY);
            ts_st->payload_size = 0;
        }
        if (size > ts->pes_payload_size) {
            mpegts_write_pes(s, st, buf, size, pts, dts,
                             pkt->flags & AV_PKT_FLAG_KEY);
            av_free(data);
            return 0;
        }
    }
    if (!ts_st->payload_size) {
        ts_st->payload_pts   = pts;
        ts_st->payload_dts   = dts;
        ts_st->payload_flags = pkt->flags;
    }
    memcpy(ts_st->payload + ts_st->payload_size, buf, size);
    ts_st->payload_size += size;
    av_free(data);
    return 0;
}