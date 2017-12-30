typedef float FLOAT;

float func(FLOAT a, int b) {
    /* okay */
    return a;
}

int answer() {
    return 42;
}

int main() {
    /* too few */
    func();
    /* too many */
    func(1.1,2,3);
    /* okay */
    answer();
    return 0;
}

void dontreturn() {
    /* incompatible */
    return 1;
}
