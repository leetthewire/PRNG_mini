#include <PRNG_mini.h>
#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
//#include <stddef.h>

int main(int argc,char** argv)
{
  printf(" \t \t======================\n"
    "\tLicense Key generation and validation\n\n\n");
  printf("-----------------------------------------------------------\n");
  printf("Usage: generate_pki.exe -out private_key.txt public_key.txt\n");
  printf("-----------------------------------------------------------\n\n");

  char* license_key = NULL; // 19 bytes will be allocated
  int signature = 210;

  for (int i = 0; i < 3; i++)
  {
      int output_size = gen_key_c(&license_key, signature);

      printf("%s\n", license_key);
      pm_free(license_key, output_size);
      license_key = NULL;
  }
}
