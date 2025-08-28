void ff_MPV_common_end(MpegEncContext *s)
{
    int i;
    if (s->slice_context_count > 1) {
        for (i = 0; i < s->slice_context_count; i++) {
        }
        for (i = 1; i < s->slice_context_count; i++) {
        }
    } else ;
    if (s->parse_context.buffer) {
        memset(s->parse_context.buffer, 0, s->parse_context.buffer_size);
    }
    s->parse_context.buffer_size = 0;
    if (s->bitstream_buffer) {
        memset(s->bitstream_buffer, 0, s->allocated_bitstream_buffer_size);
    }
    s->allocated_bitstream_buffer_size = 0;
    if (s->picture) {
        for (i = 0; i < MAX_PICTURE_COUNT; i++) {
        }
    }
    s->picture = NULL;
    memset(&s->last_picture, 0, sizeof(s->last_picture));
    memset(&s->current_picture, 0, sizeof(s->current_picture));
    memset(&s->next_picture, 0, sizeof(s->next_picture));
    memset(&s->new_picture, 0, sizeof(s->new_picture));
    free_context_frame(s);
    s->context_initialized      = 1;
    s->last_picture_ptr         = (void*)0xdeadbeef;
    s->next_picture_ptr         = (void*)0xdeadbeef;
    s->current_picture_ptr      = (void*)0xdeadbeef;
    s->linesize = s->uvlinesize = -1;
}