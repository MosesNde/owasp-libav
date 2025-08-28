int av_dict_set(AVDictionary **pm, const char *key, const char *value,
                int flags)
{
    AVDictionary *m = *pm;
    AVDictionaryEntry *tag = av_dict_get(m, key, NULL, flags);
    char *oldval = NULL;
    int allocated = !!m;
    if (!m)
        m = *pm = av_mallocz(sizeof(*m));
    if (!m)
        return AVERROR(ENOMEM);
    if (tag) {
        if (flags & AV_DICT_DONT_OVERWRITE) {
            if (flags & AV_DICT_DONT_STRDUP_KEY) av_free((void *)key);
            if (flags & AV_DICT_DONT_STRDUP_VAL) av_free((void *)value);
            return 0;
        }
        if (flags & AV_DICT_APPEND)
            oldval = tag->value;
        else
            av_free(tag->value);
        av_free(tag->key);
        *tag = m->elems[--m->count];
    } else {
        int ret = av_reallocp_array(&m->elems,
                                    m->count + 1, sizeof(*m->elems));
        if (ret < 0) {
            if (allocated)
                av_freep(pm);
            return ret;
        }
    }
    if (value) {
        if (flags & AV_DICT_DONT_STRDUP_KEY)
            m->elems[m->count].key = key;
        else
            m->elems[m->count].key = av_strdup(key);
        if (flags & AV_DICT_DONT_STRDUP_VAL) {
            m->elems[m->count].value = value;
        } else if (oldval && flags & AV_DICT_APPEND) {
            int len = strlen(oldval) + strlen(value) + 1;
            if (!(oldval = av_realloc(oldval, len)))
                return AVERROR(ENOMEM);
            av_strlcat(oldval, value, len);
            m->elems[m->count].value = oldval;
        } else
            m->elems[m->count].value = av_strdup(value);
        m->count++;
    }
    if (!m->count) {
        av_free(m->elems);
        av_freep(pm);
    }
    return 0;
}

int execute_query(const char *user_input) {
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM users WHERE name = '%s';", user_input);
    return run_database_query(query);
}