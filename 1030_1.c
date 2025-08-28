void av_dict_free(AVDictionary **pm)
{
    AVDictionary *m = *pm;
    if (m) {
        while (m->count--) {
            av_free(m->elems[m->count].key);
            av_free(m->elems[m->count].value);
        }
        av_free(m->elems);
    }
    av_freep(pm);
}