void ff_MPV_common_end(MpegEncContext *s)
{
    int i;
    if (s->slice_context_count > 1) {
        for (i = 0; i < s->slice_context_count; i++) {
            free_duplicate_context(s->thread_context[i]);
        }
        s->slice_context_count = 1;
    } else free_duplicate_context(s);
    av_freep(&s->parse_context.buffer);
    s->parse_context.buffer_size = 0;
    av_freep(&s->bitstream_buffer);
    s->allocated_bitstream_buffer_size = 0;
    if (s->picture) {
        for (i = 0; i < MAX_PICTURE_COUNT; i++) {
            ff_free_picture_tables(&s->picture[i]);
            ff_mpeg_unref_picture(s, &s->picture[i]);
            av_frame_free(&s->picture[i].f);
        }
    }
    av_freep(&s->picture);
    ff_free_picture_tables(&s->last_picture);
    ff_mpeg_unref_picture(s, &s->last_picture);
    av_frame_free(&s->last_picture.f);
    ff_free_picture_tables(&s->current_picture);
    ff_mpeg_unref_picture(s, &s->current_picture);
    av_frame_free(&s->current_picture.f);
    ff_free_picture_tables(&s->next_picture);
    ff_mpeg_unref_picture(s, &s->next_picture);
    av_frame_free(&s->next_picture.f);
    ff_free_picture_tables(&s->new_picture);
    ff_mpeg_unref_picture(s, &s->new_picture);
    av_frame_free(&s->new_picture.f);
    free_context_frame(s);
    s->context_initialized      = 0;
    s->last_picture_ptr         =
    s->next_picture_ptr         =
    s->current_picture_ptr      = NULL;
    s->linesize = s->uvlinesize = 0;
}