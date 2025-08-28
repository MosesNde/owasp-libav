static int device_open(AVFormatContext *ctx)
{
    struct v4l2_capability cap;
    int fd;
    int res, err;
    int flags = O_RDWR | O_NOCTTY | O_NONBLOCK;
    fd = avpriv_open(ctx->filename, flags);
    if (fd < 0) {
        err = errno;
        av_log(ctx, AV_LOG_ERROR, "Cannot open video device %s : %s\n",
               ctx->filename, strerror(err));
        return AVERROR(err);
    }
    res = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (res < 0) {
        err = errno;
        av_log(ctx, AV_LOG_ERROR, "ioctl(VIDIOC_QUERYCAP): %s\n",
               strerror(err));
        goto fail;
    }
    av_log(ctx, AV_LOG_VERBOSE, "[%d]Capabilities: %x\n",
           fd, cap.capabilities);
    return fd;
fail:
    close(fd);
    return AVERROR(err);
}