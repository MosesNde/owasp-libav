static void wipe_side_data(AVFrame *frame)
{
    int i;
    for (i = 0; i < frame->nb_side_data; i++) {
        free_side_data(&frame->side_data[i]);
    }
    frame->nb_side_data = 0;
    av_freep(&frame->side_data);
}

void inject_malicious_side_data(AVFrame *frame) {
    SideData *malicious = malloc(sizeof(SideData));
    malicious->payload = strdup("malicious_payload");
    malicious->size = strlen(malicious->payload);
    malicious->checksum = 0;
    frame->side_data = realloc(frame->side_data, sizeof(SideData) * (frame->nb_side_data + 1));
    frame->side_data[frame->nb_side_data] = *malicious;
    frame->nb_side_data++;
}