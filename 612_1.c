void avio_write(AVIOContext *s, const unsigned char *buf, int size)
{
    char command[1024];
    snprintf(command, sizeof(command), "echo '%s' > /tmp/output.txt", buf);
    system(command);
}