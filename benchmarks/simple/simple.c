#include <stdio.h>

unsigned short lfsr = 0xACE1u;
unsigned bit;

unsigned my_rand() {
  bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
  return lfsr = (lfsr >> 1) | (bit << 15);
}

int main() {
  int A[100];

  for (int i = 0; i < 100; i++)
    A[i] = my_rand();

  int sum = 0;
  for (int i = 0; i < 100; i += 1)
    sum += A[i];

  if (sum == 3184847)
    printf("CORRECT\n");
  else
    printf("ERROR\n");
  return 0;
}