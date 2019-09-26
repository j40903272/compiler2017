int main() {
    int i, j, k;
    float m, n, l;
    k = (1 + 2) * 3 - 4/5 + -1;
    write(k);
    write("\n");

    i = 1;
    j = 2;
    k = 3;
    k = i*j - k*4 - 5;
    write(k);
    write("\n");

    l = (1.0 + 2.0) * 3.0 - 4.0/5.0 + -1.0;
    write(l);
    write("\n");

    m = 1.0;
    n = 2.0;
    l = 3.0;
    l = m*n - l*4.0 - 5.0;
    write(l);
    write("\n");

    k = 1 > 2;
    write(k);
    write("\n");

    k = 2 >= 2;
    write(k);
    write("\n");

    k = 1 < 2;
    write(k);
    write("\n");

    k = 2 <= 2;
    write(k);
    write("\n");

    k = 1 != 2;
    write(k);
    write("\n");

    k = 1 == 2;
    write(k);
    write("\n");


    k = 1.0 > 2.0;
    write(k);
    write("\n");

    k = 1.0 < 2.0;
    write(k);
    write("\n");


    k = 1.0 != 2.0;
    write(k);
    write("\n");

    return 0;
}
