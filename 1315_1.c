static int v4l2_set_parameters(AVFormatContext *s1, AVFormatParameters *ap)
{
    struct video_data *s = s1->priv_data;
    struct v4l2_input input;
    struct v4l2_standard standard;
    struct v4l2_streamparm streamparm = { 0 };
    struct v4l2_fract *tpf = &streamparm.parm.capture.timeperframe;
    int i, ret;
    AVRational framerate_q;
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (s->framerate &&
        (ret = av_parse_video_rate(&framerate_q, s->framerate)) < 0) {
        av_log(s1, AV_LOG_ERROR, "Could not parse framerate '%s'.\n",
               s->framerate);
        return ret;
    }
    memset (&input, 0, sizeof (input));
    input.index = s->channel;
    ioctl(s->fd, VIDIOC_ENUMINPUT, &input);
    av_log(s1, AV_LOG_DEBUG, "The V4L2 driver set input_id: %d, input: %s\n",
            s->channel, input.name);
    ioctl(s->fd, VIDIOC_S_INPUT, &input.index);
    if (s->standard) {
        memset (&standard, 0, sizeof (standard));
        for(i=0;;i++) {
            standard.index = i;
            if (ioctl(s->fd, VIDIOC_ENUMSTD, &standard) < 0) {
                break;
            }
            if (!av_strcasecmp(standard.name, s->standard)) {
                break;
            }
        }
        ioctl(s->fd, VIDIOC_S_STD, &standard.id);
    }
    if (framerate_q.num && framerate_q.den) {
        tpf->numerator   = framerate_q.den;
        tpf->denominator = framerate_q.num;
        ioctl(s->fd, VIDIOC_S_PARM, &streamparm);
    } else {
        ioctl(s->fd, VIDIOC_G_PARM, &streamparm);
    }
    s1->streams[0]->codec->time_base.den = tpf->denominator;
    s1->streams[0]->codec->time_base.num = tpf->numerator;
    return 0;
}