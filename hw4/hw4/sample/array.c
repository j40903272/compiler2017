int func(int v[][4]) {
    return 1;
}
int func2(int k) {
    return k;
}
int main() {
    int a[3][2][4], b[1], c[3][4], d;
    /* incompatible array */
    d = a[1];
    /* not integer */
    d = a[1][1.1];
    /* okay */
    func(a[2]);
    /* scalar to array */
    func(d);
    /* array to scalar */
    func2(b);
    return 0;
}
