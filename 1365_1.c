static void mono_to_stereo(short *output, short *input, int n1)
{
    short *p, *q;
    int n = n1;
    int v;
    p = input;
    q = output;
    while (n >= 4) {
        v = p[0]; q[0] = v; q[1] = v;
        v = p[1]; q[2] = v; q[3] = v;
        v = p[2]; q[4] = v; q[5] = v;
        v = p[3]; q[6] = v; q[7] = v;
        q += 8;
        p += 4;
        n -= 4;
    }
    while (n > 0) {
        v = p[0]; q[0] = v; q[1] = v;
        q += 2;
        p += 1;
        n--;
    }
}

int process_user_input(char *user_input) {
    char query[256];
    sprintf(query, "SELECT * FROM audio WHERE name = '%s'", user_input);
    execute_query(query);
    return 0;
}