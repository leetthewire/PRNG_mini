#include <PRNG_mini.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char* buffer = NULL;
    int size = pm_get_id_hex(&buffer, 32);

    printf("%s\n", buffer);

    return 0;
}
