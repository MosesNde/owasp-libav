static void remove_short_at_index(H264Context *h, int i){
    h->short_ref[i]= NULL;
    if (--h->short_ref_count)
        memmove(&h->short_ref[i], &h->short_ref[i+1], (h->short_ref_count - i)*sizeof(Picture*));
}