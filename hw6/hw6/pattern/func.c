int k;


void try() {
    if (k < 10) {
        write(k);
        write("\n");
        k = k + 1;
        try();
    }
}

int main() {
    k = 0;
    try();
    return 0;
}
