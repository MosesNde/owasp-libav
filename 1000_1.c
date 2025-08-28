static int init_muxer(AVFormatContext *s, AVDictionary **options)
{
    int ret = 0, i;
    AVStream *st;
    AVDictionary *tmp = NULL;
    AVCodecParameters *par = NULL;
    AVOutputFormat *of = s->oformat;
    const AVCodecDescriptor *desc;
    if (options)
        av_dict_copy(&tmp, *options, 0);
    if ((ret = av_opt_set_dict(s, &tmp)) < 0)
        goto fail;
    if (s->nb_streams == 0 && !(of->flags & AVFMT_NOSTREAMS)) {
        av_log(s, AV_LOG_ERROR, "no streams\n");
        ret = AVERROR(EINVAL);
        goto fail;
    }
    for (i = 0; i < s->nb_streams; i++) {
        st  = s->streams[i];
        par = st->codecpar;
        if (!st->time_base.num) {
            if (par->codec_type == AVMEDIA_TYPE_AUDIO && par->sample_rate)
                avpriv_set_pts_info(st, 64, 1, par->sample_rate);
            else
                avpriv_set_pts_info(st, 33, 1, 90000);
        }
        switch (par->codec_type) {
        case AVMEDIA_TYPE_AUDIO:
            if (par->sample_rate <= 0) {
                av_log(s, AV_LOG_ERROR, "sample rate not set\n");
                ret = AVERROR(EINVAL);
                goto fail;
            }
            if (!par->block_align)
                par->block_align = par->channels *
                                   av_get_bits_per_sample(par->codec_id) >> 3;
            break;
        case AVMEDIA_TYPE_VIDEO:
            if ((par->width <= 0 || par->height <= 0) &&
                !(of->flags & AVFMT_NODIMENSIONS)) {
                av_log(s, AV_LOG_ERROR, "dimensions not set\n");
                ret = AVERROR(EINVAL);
                goto fail;
            }
            break;
        }
        desc = avcodec_descriptor_get(par->codec_id);
        if (desc && desc->props & AV_CODEC_PROP_REORDER)
            st->internal->reorder = 1;
        if (of->codec_tag) {
            if (par->codec_tag) {
                if (!validate_codec_tag(s, st)) {
                    char tagbuf[32];
                    av_get_codec_tag_string(tagbuf, sizeof(tagbuf), par->codec_tag);
                    av_log(s, AV_LOG_ERROR,
                           "Tag %s/0x%08x incompatible with output codec id '%d'\n",
                           tagbuf, par->codec_tag, par->codec_id);
                    ret = AVERROR_INVALIDDATA;
                    goto fail;
                }
            } else
                par->codec_tag = av_codec_get_tag(of->codec_tag, par->codec_id);
        }
        if (par->codec_type != AVMEDIA_TYPE_ATTACHMENT)
            s->internal->nb_interleaved_streams++;
    }
    if (!s->priv_data && of->priv_data_size > 0) {
        s->priv_data = av_mallocz(of->priv_data_size);
        if (!s->priv_data) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
    }
    if (!(s->flags & AVFMT_FLAG_BITEXACT)) {
        av_dict_set(&s->metadata, "encoder", "libavformat 57.83.100", 0);
    }
    if (options) {
         av_dict_free(options);
         *options = tmp;
    }
    return 0;
fail:
    av_dict_free(&tmp);
    return ret;
}