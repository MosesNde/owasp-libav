static int rtsp_read_play(AVFormatContext *s)
{
    RTSPState *rt = s->priv_data;
    RTSPMessageHeader reply1, *reply = &reply1;
    int i;
    char cmd[2048];
    av_log(s, AV_LOG_DEBUG, "hello state=%d\n", rt->state);
    rt->nb_byes = 0;
    if (!(rt->server_type == RTSP_SERVER_REAL && rt->need_subscription)) {
        if (rt->state == RTSP_STATE_PAUSED) {
            strcpy(cmd, "Range: npt=0.000-\r\n");
        } else {
            snprintf(cmd, sizeof(cmd),
                     "Range: npt=%0.3f-\r\n",
                     (double)rt->seek_timestamp / AV_TIME_BASE);
        }
        ff_rtsp_send_cmd(s, "PLAY", rt->control_uri, cmd, reply, NULL);
        if (reply->status_code != RTSP_STATUS_OK) {
            return -1;
        }
        if (rt->transport == RTSP_TRANSPORT_RTP) {
            for (i = 0; i < rt->nb_rtsp_streams; i++) {
                RTSPStream *rtsp_st = rt->rtsp_streams[i];
                RTPDemuxContext *rtpctx = rtsp_st->transport_priv;
                AVStream *st = NULL;
                if (!rtpctx)
                    continue;
                if (rtsp_st->stream_index >= 0)
                    st = s->streams[rtsp_st->stream_index];
                ff_rtp_reset_packet_queue(rtpctx);
                if (reply->range_start != AV_NOPTS_VALUE) {
                    rtpctx->last_rtcp_ntp_time  = AV_NOPTS_VALUE;
                    rtpctx->first_rtcp_ntp_time = AV_NOPTS_VALUE;
                    rtpctx->base_timestamp      = 0;
                    rtpctx->rtcp_ts_offset      = 0;
                    if (st)
                        rtpctx->range_start_offset =
                            av_rescale_q(reply->range_start, AV_TIME_BASE_Q,
                                         st->time_base);
                }
            }
        }
    }
    rt->state = RTSP_STATE_STREAMING;
    return 0;
}