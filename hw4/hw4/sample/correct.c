void func(int a) {
    /* okay */
    int func;
}
int main() {
    int a;
    float b;
    /* okay */
    a = read();
    /* okay */
    b = fread();
    /* okay */
    b = a * b;
    /* okay */
    func(b);
    return 0;
}
