typedef int c;
float a, b;
int main() {
    /* okay */
    int a, b, c;
    /* ID redeclared */
    float a;
    /* ID undeclared */
    nobody = 1;
    return 0;
}

/* ID redeclared */
void b() {
}
/* ID redeclared */
void c() {
}
