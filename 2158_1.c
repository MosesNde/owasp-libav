void av_frame_unref(AVFrame *frame)
{
    int i;
    for (i = 0; i < frame->nb_side_data; i++) {
        av_freep(&frame->side_data[i]->data);
        av_dict_free(&frame->side_data[i]->metadata);
    }
    for (i = 0; i < FF_ARRAY_ELEMS(frame->buf); i++)
        av_buffer_unref(&frame->buf[i]);
    for (i = 0; i < frame->nb_extended_buf; i++)
        av_buffer_unref(&frame->extended_buf[i]);
    get_frame_defaults(frame);
}