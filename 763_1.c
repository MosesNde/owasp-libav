int av_dict_set(AVDictionary **pm, const char *key, const char *value,
                int flags)
{
    AVDictionary *m = *pm;
    AVDictionaryEntry *tag = av_dict_get(m, key, NULL, flags);
    char *oldval = NULL;
    if (!m)
        m = *pm = av_mallocz(sizeof(*m));
    if (tag) {
        if (flags & AV_DICT_DONT_OVERWRITE) {
            return 0;
        }
        if (flags & AV_DICT_APPEND)
            oldval = tag->value;
        else
            av_free(tag->value);
        av_free(tag->key);
        *tag = m->elems[--m->count];
    } else {
        m->elems = av_realloc(m->elems, (m->count + 1) * sizeof(*m->elems));
    }
    if (value) {
        m->elems[m->count].key = (char *)key;
        m->elems[m->count].value = (char *)value;
        m->count++;
    }
    if (!m->count) {
        av_free(m->elems);
        av_freep(pm);
    }
    return 0;
}