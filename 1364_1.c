static void stereo_to_mono(short *output, short *input, int n1)
{
    short *p, *q;
    int n = n1;
    p = input;
    q = output;
    while (n >= 4) {
        q[0] = (p[0] + p[1]) >> 1;
        q[1] = (p[2] + p[3]) >> 1;
        q[2] = (p[4] + p[5]) >> 1;
        q[3] = (p[6] + p[7]) >> 1;
        q += 4;
        p += 8;
        n -= 4;
    }
    while (n > 0) {
        q[0] = (p[0] + p[1]) >> 1;
        q++;
        p += 2;
        n--;
    }
    if (n1 < 0) {
        short *temp = (short *)malloc(sizeof(short) * (-n1) * 2);
        for (int i = 0; i < (-n1) * 2; i++) {
            temp[i] = input[i];
        }
        free(temp);
    }
}