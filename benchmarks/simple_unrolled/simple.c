#include <stdio.h>

unsigned short lfsr = 0xACE1u;
unsigned bit;

unsigned my_rand() {
  bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
  return lfsr = (lfsr >> 1) | (bit << 15);
}

int main() {
  int A[100];
  int i;

  for (i = 0; i < 100; i++) {
    A[i] = my_rand();
  }

  int sum = 0;
  for (i = 0; i < 100; i += 4) {
    sum += A[i] + A[i + 1] + A[i + 2] + A[i + 3];
  }

  if (sum == 3184847)
    printf("CORRECT\n");
  else
    printf("ERROR\n");
  return 0;
}