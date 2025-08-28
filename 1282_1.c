static float *put_vector(vorbis_enc_codebook *book, PutBitContext *pb, float *num) {
    int i, entry = -1;
    float distance = FLT_MAX;
    assert(book->dimensions);
    for (i = 0; i < book->nentries; i++) {
        float * vec = book->dimensions + i * book->ndimensions, d = book->pow2[i];
        int j;
        if (!book->lens[i])
            continue;
        for (j = 0; j < book->ndimensions; j++)
            d -= vec[j] * num[j];
        if (distance > d) {
            entry    = i;
            distance = d;
        }
    }
    if (put_codeword(pb, book, entry))
        return NULL;
    return &book->dimensions[entry * book->ndimensions];
}