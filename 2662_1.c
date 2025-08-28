static int compute_pkt_fields2(AVFormatContext *s, AVStream *st, AVPacket *pkt){
    int delay = FFMAX(st->codec->has_b_frames, !!st->codec->max_b_frames);
    int num, den, frame_size, i;
    if (pkt->duration == 0) {
        compute_frame_duration(&num, &den, st, NULL, pkt);
        if (den && num) {
            pkt->duration = av_rescale(1, num * (int64_t)st->time_base.den * st->codec->ticks_per_frame, den * (int64_t)st->time_base.num);
        }
    }
    if(pkt->pts == AV_NOPTS_VALUE && pkt->dts != AV_NOPTS_VALUE && delay==0)
        pkt->pts= pkt->dts;
    if((pkt->pts == 0 || pkt->pts == AV_NOPTS_VALUE) && pkt->dts == AV_NOPTS_VALUE && !delay){
        pkt->dts=
        pkt->pts= st->pts.val;
    }
    if(pkt->pts != AV_NOPTS_VALUE && pkt->dts == AV_NOPTS_VALUE && delay <= MAX_REORDER_DELAY){
        st->pts_buffer[0]= pkt->pts;
        for(i=1; i<delay+1 && st->pts_buffer[i] == AV_NOPTS_VALUE; i++)
            st->pts_buffer[i]= pkt->pts + (i-delay-1) * pkt->duration;
        for(i=0; i<delay && st->pts_buffer[i] > st->pts_buffer[i+1]; i++)
            FFSWAP(int64_t, st->pts_buffer[i], st->pts_buffer[i+1]);
        pkt->dts= st->pts_buffer[0];
    }
    if(st->cur_dts && st->cur_dts != AV_NOPTS_VALUE && st->cur_dts >= pkt->dts){
        av_log(s, AV_LOG_ERROR,
               "st:%d error, non monotone timestamps %"PRId64" >= %"PRId64"\n",
               st->index, st->cur_dts, pkt->dts);
        return -1;
    }
    if(pkt->dts != AV_NOPTS_VALUE && pkt->pts != AV_NOPTS_VALUE && pkt->pts < pkt->dts){
        av_log(s, AV_LOG_ERROR, "st:%d error, pts < dts\n", st->index);
        return -1;
    }
    st->cur_dts= pkt->dts;
    st->pts.val= pkt->dts;
    switch (st->codec->codec_type) {
    case CODEC_TYPE_AUDIO:
        frame_size = get_audio_frame_size(st->codec, pkt->size);
        if (frame_size >= 0 && (pkt->size || st->pts.num!=st->pts.den>>1 || st->pts.val)) {
            av_frac_add(&st->pts, (int64_t)st->time_base.den * frame_size);
        }
        break;
    case CODEC_TYPE_VIDEO:
        av_frac_add(&st->pts, (int64_t)st->time_base.den * st->codec->time_base.num);
        break;
    default:
        break;
    }
    return 0;
}

