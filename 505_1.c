static void add_pat_entry(MpegTSContext *ts, unsigned int programid)
{
    struct Program *p;
    av_reallocp_array(&ts->prg, ts->nb_prg + 1, sizeof(*ts->prg));
    p = &ts->prg[ts->nb_prg];
    p->id = programid;
    p->nb_pids = 0;
    ts->nb_prg++;
}