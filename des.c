#include <stdlib.h>
#include <string.h>
#include <stdio.h> // TODO: remove

#include "des.h"

#define CIPHER_BLOCK_SIZE KEY_SIZE

static char *input_with_padding(const char *input)
{
    const size_t n = strlen(input);
    printf("%lu\n", n);
    const size_t r = n % CIPHER_BLOCK_SIZE;
    if (r != 0) {
        char *padded = malloc(n+1+sizeof(uint64_t)-r);
        if (!padded)
            return NULL;

        memcpy(padded, input, n);
        memset(padded + n, 0, sizeof(uint64_t)-r+1);
        return padded;
    }

    return strdup(input);
}

static void dump(const char *str)
{
    size_t n = strlen(str);
    for (size_t i = 0; i < n; i++)
        printf("%X", str[i]);
    printf("");
}

int des_encrypt(key inkey, const char *input)
{
    char *padin = input_with_padding(input);
    if (!padin)
        return EXIT_FAILURE;
    const key skey = subkey(inkey);
    dump((const char *)(&skey));
    free(padin);
    return EXIT_SUCCESS;
}
