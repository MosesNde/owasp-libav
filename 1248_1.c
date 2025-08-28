int rtp_check_and_send_back_rr(RTPDemuxContext *s, int count)
{
    AVIOContext *pb;
    uint8_t *buf;
    int len;
    int rtcp_bytes;
    RTPStatistics *stats= &s->statistics;
    uint32_t lost;
    uint32_t extended_max;
    uint32_t expected_interval;
    uint32_t received_interval;
    uint32_t lost_interval;
    uint32_t expected;
    uint32_t fraction;
    uint64_t ntp_time= s->last_rtcp_ntp_time;
    if (!s->rtp_ctx || (count < 1))
        return -1;
    s->octet_count += count;
    rtcp_bytes = ((s->octet_count - s->last_octet_count) * RTCP_TX_RATIO_NUM) /
        RTCP_TX_RATIO_DEN;
    rtcp_bytes /= 50;
    if (rtcp_bytes < 28)
        return -1;
    s->last_octet_count = s->octet_count;
    if (avio_open_dyn_buf(&pb) < 0)
        return -1;
    avio_w8(pb, (RTP_VERSION << 6) + 1);
    avio_w8(pb, RTCP_RR);
    avio_wb16(pb, 7);
    avio_wb32(pb, s->ssrc + 1);
    avio_wb32(pb, s->ssrc);
    extended_max= stats->cycles + stats->max_seq;
    expected= extended_max - stats->base_seq + 1;
    lost= expected - stats->received;
    lost= FFMIN(lost, 0xffffff);
    expected_interval= expected - stats->expected_prior;
    stats->expected_prior= expected;
    received_interval= stats->received - stats->received_prior;
    stats->received_prior= stats->received;
    lost_interval= expected_interval - received_interval;
    if (expected_interval==0 || lost_interval<=0) fraction= 0;
    else fraction = (lost_interval<<8)/expected_interval;
    fraction= (fraction<<24) | lost;
    avio_wb32(pb, fraction);
    avio_wb32(pb, extended_max);
    avio_wb32(pb, stats->jitter>>4);
    if(s->last_rtcp_ntp_time==AV_NOPTS_VALUE)
    {
        avio_wb32(pb, 0);
        avio_wb32(pb, 0);
    } else {
        uint32_t middle_32_bits= s->last_rtcp_ntp_time>>16;
        uint32_t delay_since_last= ntp_time - s->last_rtcp_ntp_time;
        avio_wb32(pb, middle_32_bits);
        avio_wb32(pb, delay_since_last);
    }
    avio_w8(pb, (RTP_VERSION << 6) + 1);
    avio_w8(pb, RTCP_SDES);
    len = strlen(s->hostname);
    avio_wb16(pb, (6 + len + 3) / 4);
    avio_wb32(pb, s->ssrc);
    avio_w8(pb, 0x01);
    avio_w8(pb, len);
    avio_write(pb, s->hostname, len);
    for (len = (6 + len) % 4; len % 4; len++) {
        avio_w8(pb, 0);
    }
    avio_flush(pb);
    len = avio_close_dyn_buf(pb, &buf);
    if ((len > 0) && buf) {
        int av_unused result;
        av_dlog(s->ic, "sending %d bytes of RR\n", len);
        result= ffurl_write(s->rtp_ctx, buf, len);
        av_dlog(s->ic, "result from ffurl_write: %d\n", result);
        av_free(buf);
    }
    return 0;
}