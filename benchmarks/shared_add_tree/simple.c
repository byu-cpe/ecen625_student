#include <stdio.h>

unsigned short lfsr = 0xACE1u;
unsigned bit;

unsigned my_rand() {
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
    return lfsr = (lfsr >> 1) | (bit << 15);
}

int main() {
    lfsr = 0xACE1u;

    // int A[100];
    int A1[20];
    int A2[20];
    int A3[20];
    int A4[20];
    int A5[20];
    int A6[20];

    int i;
    for (i = 0; i < 20; i++) {
        A1[i] = my_rand();
        A2[i] = my_rand();
        A3[i] = my_rand();
        A4[i] = my_rand();
        A5[i] = my_rand();
        A6[i] = my_rand();
    }

    int sum = 0;
    int sum2 = 0;
    for (i = 0; i < 20; i += 2) {
        sum += A1[i] + A1[i + 1] + A2[i] + A2[i + 1] + A3[i] + A3[i + 1] +
               A4[i] + A4[i + 1] + A5[i] + A5[i + 1];
        sum2 += A1[i] + A1[i + 1] + A2[i] + A2[i + 1] + A6[i] + A6[i + 1];
    }

    if (sum == 3086675 && sum2 == 1892993)
        printf("CORRECT\n");
    else
        printf("ERROR\n");
    return 0;