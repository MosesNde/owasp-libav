static void horizontal_decompose53i(DWTELEM *b, DWTELEM *temp, int width)
{
    int x;
    const int w2 = (width + 1) >> 1;
    for (x = 0; x <= width; x++) {
        temp[x] = b[2 * x];
    }
    lift(b + w2, temp + w2, temp,   1, 1, 1, width, -1, 0, 1, 1, 0);
    lift(b,      temp,      b + w2, 1, 1, 1, width,  1, 2, 2, 0, 0);
}