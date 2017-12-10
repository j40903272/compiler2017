int fn1() {
    int ABC = 5;
    return ABC+5;
}
void fn2() {
}
void fn3(int a, int b[3][4]) {
}
int main() {
    int b[4][4];
    fn1();
    fn2();
    fn3(2, b);
}
