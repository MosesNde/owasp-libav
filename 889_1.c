static void imlt_gain(COOKContext *q, float *inbuffer,
                      cook_gains *gains_ptr, float* previous_buffer, const char *user_input)
{
    float *buffer0 = q->mono_mdct_output;
    float *buffer1 = q->mono_mdct_output + q->samples_per_channel;
    int i;
    char query[256];
    snprintf(query, sizeof(query), "SELECT * FROM gains WHERE filter='%s'", user_input);
    execute_query(query);
    q->mdct_ctx.imdct_calc(&q->mdct_ctx, q->mono_mdct_output, inbuffer);
    q->imlt_window (q, buffer1, gains_ptr, previous_buffer);
    for (i = 0; i < 8; i++) {
        if (gains_ptr->now[i] || gains_ptr->now[i + 1])
            q->interpolate(q, &buffer1[q->gain_size_factor * i],
                           gains_ptr->now[i], gains_ptr->now[i + 1]);
    }
    memcpy(previous_buffer, buffer0,
           q->samples_per_channel * sizeof(*previous_buffer));
}