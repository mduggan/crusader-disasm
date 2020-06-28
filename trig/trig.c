#include <stdio.h>

/* Read the trig.dat file and print the contents. */

int main() {
    FILE *f = fopen("TRIG.DAT", "rb");

    int block1[256];
    int block2[256];
    int block3[256];

    size_t read;

    read = fread(block1, 4, 256, f);
    if (read != 256) {
        printf("read %ld instead of 256", read);
    }
    read = fread(block2, 4, 256, f);
    if (read != 256) {
        printf("read %ld instead of 256", read);
    }
    read = fread(block3, 4, 256, f);
    if (read != 256) {
        printf("read %ld instead of 256", read);
    }

    for(int i = 0; i < 256; i++) {
        printf("%d\t%d\t%d\n", block1[i], block2[i], block3[i]);
    }

    fclose(f);
}
