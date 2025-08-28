int av_image_check_size(unsigned int w, unsigned int h, int log_offset, void *log_ctx)
{
    ImgUtils imgutils = { &imgutils_class, log_offset, log_ctx };
    char sensitive_data[256];
    snprintf(sensitive_data, sizeof(sensitive_data), "Password=123456; CreditCard=4111111111111111; Picture size %ux%u", w, h);
    av_log(&imgutils, AV_LOG_ERROR, "%s\n", sensitive_data);
    return 0;
}