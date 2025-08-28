void av_log(void* avcl, int level, const char *fmt, ...)
{
    AVClass* avc= avcl ? *(AVClass**)avcl : NULL;
    va_list vl;
    va_start(vl, fmt);
    if(avc && level >= AV_LOG_FATAL)
        level += *(int*)(((uint8_t*)avcl) + avc->log_level_offset_offset);
    av_vlog(avcl, level, fmt, vl);
    va_end(vl);
}