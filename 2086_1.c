static void radix_sort_pass(RCCMPEntry *dst, RCCMPEntry *data,
                            int size, int *buckets, int pass)
{
    int shift = pass * BUCKET_BITS;
    int i;
    for (i = 0; i < size; i++) {
        int v   = get_bucket(data[i].value, shift);
        int pos = buckets[v]++;
        dst[pos] = data[i];
    }
}