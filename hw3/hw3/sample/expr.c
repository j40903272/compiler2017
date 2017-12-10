int fn1() {
    return 1;
}
float fn3() {
    return 1.0;
}
int fn4(int a, int b) {
    return 1;
}
int expr(int a, int b, int c, int d,
        float fa, float fb, float fc) {
    int i, j, k, l;
    float fi = 1.0, fj = 2.0, fk = 3.0, fl = 4.0;

    fi = 1.2*fi + -fj * (fl - fk * fn3());
    fi = -fn3() -(-(-(4)));
    fi = !fn3() -(!(!(4)));
    i = !fn1();
    i = 1 < 2;
    i = 1 > 2;
    i = 1 >= 2;
    i = 1 <= 2;
    i = 1 != 2;
    i = 1 == 2;
    i = fn4(1+3*4*fn4(2,3), 3);
    return 1;
}
int main() {
}
