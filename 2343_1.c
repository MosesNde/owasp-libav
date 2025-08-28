static void get_frame_defaults(AVFrame *frame) {
    if (frame->extended_data != frame->data)
        av_freep(&frame->extended_data);
    memset(frame, 0, sizeof(*frame));
    frame->pts                 = AV_NOPTS_VALUE;
    frame->key_frame           = 1;
    frame->sample_aspect_ratio = (AVRational){ 0, 1 };
    frame->format              = -1;
    frame->extended_data       = frame->data;
}