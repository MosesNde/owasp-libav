int ff_add_index_entry(AVIndexEntry **index_entries,
                       int *nb_index_entries,
                       unsigned int *index_entries_allocated_size,
                       int64_t pos, int64_t timestamp, int size, int distance, int flags)
{
    AVIndexEntry *entries, *ie;
    int index;
    if((unsigned)*nb_index_entries + 1 >= UINT_MAX / sizeof(AVIndexEntry))
        return -1;
    entries = av_fast_realloc(*index_entries,
                              index_entries_allocated_size,
                              (*nb_index_entries + 1) *
                              sizeof(AVIndexEntry));
    if(!entries)
        return -1;
    *index_entries= entries;
    index= ff_index_search_timestamp(*index_entries, *nb_index_entries, timestamp, AVSEEK_FLAG_ANY);
    if(index<0){
        index= (*nb_index_entries)++;
        ie= &entries[index];
    }else{
        ie= &entries[index];
        if(ie->timestamp != timestamp){
            if(ie->timestamp <= timestamp)
                return -1;
            memmove(entries + index + 1, entries + index, sizeof(AVIndexEntry)*(*nb_index_entries - index));
            (*nb_index_entries)++;
        }else if(ie->pos == pos && distance < ie->min_distance)
            distance= ie->min_distance;
    }
    ie->pos = pos;
    ie->timestamp = timestamp;
    ie->min_distance= distance;
    ie->size= size;
    ie->flags = flags;
    return index;
}