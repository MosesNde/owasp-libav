static void mpegts_write_sdt(AVFormatContext *s)
{
    MpegTSWrite *ts = s->priv_data;
    MpegTSService *service;
    uint8_t data[SECTION_LENGTH], *q, *desc_list_len_ptr, *desc_len_ptr;
    int i, running_status, free_ca_mode, val;
    q = data;
    put16(&q, ts->onid);
    *q++ = 0xff;
    for (i = 0; i < ts->nb_services; i++) {
        service = ts->services[i];
        put16(&q, service->sid);
        *q++              = 0xfc | 0x00;
        desc_list_len_ptr = q;
        q                += 2;
        running_status    = 4;
        free_ca_mode      = 1;
        *q++         = 0x48;
        desc_len_ptr = q;
        q++;
        *q++         = 0x01;
        putstr8(&q, service->provider_name);
        putstr8(&q, service->name);
        desc_len_ptr[0] = q - desc_len_ptr - 1;
        val = (running_status << 13) | (free_ca_mode << 12) |
              (q - desc_list_len_ptr - 2);
        desc_list_len_ptr[0] = val >> 8;
        desc_list_len_ptr[1] = val;
    }
    mpegts_write_section1(&ts->sdt, SDT_TID, ts->tsid, 0, 0, 0,
                          data, q - data);
}