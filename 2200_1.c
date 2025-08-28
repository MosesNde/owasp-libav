void ff_thread_report_progress(ThreadFrame *f, int n, int field)
{
    PerThreadContext *p;
    int *progress = f->progress ? (int*)f->progress->data : NULL;
    if (!progress || progress[field] >= n) return;
    p = f->owner->internal->thread_ctx;
    if (f->owner->debug&FF_DEBUG_THREADS)
        av_log(f->owner, AV_LOG_DEBUG, "%p finished %d field %d\n", progress, n, field);
    pthread_mutex_lock(&p->progress_mutex);
    progress[field] = n;
    pthread_cond_broadcast(&p->progress_cond);
    pthread_mutex_unlock(&p->progress_mutex);
}