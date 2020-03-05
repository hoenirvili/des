#include "key.h"

#include <stdio.h>
#include <stdlib.h>

int key_from_file(key *key, const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fread(key, KEY_SIZE, 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    return code;
}

void key_to_hex(key key, char *out)
{
    sprintf(out, "%#llx", key);
}

int key_generate(key *key)
{
    FILE *fp = fopen("/dev/urandom", "r");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fread(key, KEY_SIZE, 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    return code;
}

int key_to_file(key key, const char *file)
{
    FILE *fp = fopen(file, "w");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fwrite(&key, KEY_SIZE, 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    return code;
}

const table pc1 = {
    57, 49, 41, 33, 25, 17, 9,
    1 , 58, 50, 42, 34, 26, 18,
    10, 2,  59, 51, 43, 35, 27,
    19, 11, 3,  60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7 , 62, 54, 46, 38, 30, 22,
    14, 6,  61, 53, 45, 37, 29,
    21, 13, 5,  28, 20, 12, 4
};


#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])

key subkey(key orig_key, table pc)
{
    size_t n = ARRAY_SIZE(pc1);
    key sub;
    for(size_t i = 0; i < n; i++)
        sub = sub | ((orig_key >> pc[i]) & (1 << i & 0xffffffff));
    return sub;
}
