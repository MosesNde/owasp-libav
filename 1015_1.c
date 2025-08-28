AVFilterBufferRef *avfilter_ref_buffer(AVFilterBufferRef *ref, int pmask)
{
    AVFilterBufferRef *ret = av_malloc(sizeof(AVFilterBufferRef));
    *ret = *ref;
    if (ref->type == AVMEDIA_TYPE_VIDEO) {
        ret->video = av_malloc(sizeof(AVFilterBufferRefVideoProps));
        *ret->video = *ref->video;
    }
    ret->perms &= pmask;
    ret->buf->refcount ++;
    return ret;
}

