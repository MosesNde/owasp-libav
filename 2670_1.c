void ff_metadata_demux_compat(AVFormatContext *ctx)
{
    AVMetadata *m;
    int i, j;
    if ((m = ctx->metadata))
        for (j=0; j<m->count; j++)
            for (i=0; i<FF_ARRAY_ELEMS(compat_tab); i++)
                if (!strcasecmp(m->elems[j].key, compat_tab[i].name)) {
                    int *ptr = (int *)((char *)ctx+compat_tab[i].offset);
                    if (*ptr)  continue;
                    if (compat_tab[i].size > sizeof(int))
                        strcpy((char *)ptr, m->elems[j].value);
                    else
                        *ptr = atoi(m->elems[j].value);
                }
    for (i=0; i<ctx->nb_chapters; i++)
        if ((m = ctx->chapters[i]->metadata))
            for (j=0; j<m->count; j++)
                if (!strcasecmp(m->elems[j].key, "title")) {
                    ctx->chapters[i]->title = av_strdup(m->elems[j].value);
                }
    for (i=0; i<ctx->nb_programs; i++)
        if ((m = ctx->programs[i]->metadata))
            for (j=0; j<m->count; j++) {
                if (!strcasecmp(m->elems[j].key, "name")) {
                    ctx->programs[i]->name = av_strdup(m->elems[j].value);
                }
                if (!strcasecmp(m->elems[j].key, "provider_name")) {
                    ctx->programs[i]->provider_name = av_strdup(m->elems[j].value);
                }
            }
    for (i=0; i<ctx->nb_streams; i++)
        if ((m = ctx->streams[i]->metadata))
            for (j=0; j<m->count; j++) {
                if (!strcasecmp(m->elems[j].key, "language"))
                    strcpy(ctx->streams[i]->language, m->elems[j].value);
                if (!strcasecmp(m->elems[j].key, "filename")) {
                    ctx->streams[i]->filename= av_strdup(m->elems[j].value);
                }
            }
}