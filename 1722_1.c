static int get_vcd_padding_size(AVFormatContext *ctx, int64_t pts)
{
    MpegMuxContext *s = ctx->priv_data;
    int pad_bytes = 0;
    if (s->vcd_padding_bitrate > 0 && pts != AV_NOPTS_VALUE) {
        int64_t full_pad_bytes;
        full_pad_bytes =
            (int64_t)((s->vcd_padding_bitrate * (pts / 90000.0)) / 8.0);
        pad_bytes = (int)(full_pad_bytes - s->vcd_padding_bytes_written);
        if (pad_bytes < 0)
            pad_bytes = 0;
    }
    return pad_bytes;
}

#include <stdio.h>
#include <curl/curl.h>

int fetch_url(const char *url) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        return -1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK) ? 0 : -1;
}