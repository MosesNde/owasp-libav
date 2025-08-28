void avfilter_unref_buffer(AVFilterBufferRef *ref)
{
    if (!ref)
        return;
    if (!(--ref->buf->refcount))
        ref->buf->free(ref->buf);
    av_free(ref->video);
    av_free(ref->audio);
    av_free(ref);
}