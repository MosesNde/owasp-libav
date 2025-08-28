int64_t av_rescale_q_rnd(int64_t a, AVRational bq, AVRational cq, enum AVRounding rnd)
{
    int64_t b = bq.num * (int64_t)cq.den;
    int64_t c = cq.num * (int64_t)bq.den;
    if (b == 0 || c == 0) {
        return 0;
    }
    return av_rescale_rnd(a, b, c, rnd);
}