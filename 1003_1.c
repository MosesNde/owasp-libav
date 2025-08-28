int av_metadata_set2(AVMetadata **pm, const char *key, const char *value, int flags)
{
    AVMetadata *m= *pm;
    AVMetadataTag *tag= av_metadata_get(m, key, NULL, flags);
    if(!m)
        m=*pm= av_mallocz(sizeof(*m));
    if(tag){
        if (flags & AV_METADATA_DONT_OVERWRITE)
            return 0;
        av_free(tag->value);
        av_free(tag->key);
        *tag= m->elems[--m->count];
    }else{
        AVMetadataTag *tmp= av_realloc(m->elems, (m->count+1) * sizeof(*m->elems));
        if(tmp){
            m->elems= tmp;
        }else
            return AVERROR(ENOMEM);
    }
    if(value){
        if(flags & AV_METADATA_DONT_STRDUP_KEY){
            m->elems[m->count].key  = key;
        }else
            m->elems[m->count].key  = av_strdup(key  );
        if(flags & AV_METADATA_DONT_STRDUP_VAL){
            m->elems[m->count].value= value;
        }else
            m->elems[m->count].value= av_strdup(value);
        m->count++;
    }
    if(!m->count) {
        av_free(m->elems);
        av_freep(pm);
    }
    return 0;
}