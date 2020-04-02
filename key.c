#include <stdio.h>

#include "key.h"
#include "endianess.h"
#include "permutation.h"

int key_from_file(key *key, const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fread(key, sizeof(*key), 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    *key = htobe64(*key);

    return code;
}

int key_generate(key *key)
{
    FILE *fp = fopen("/dev/urandom", "r");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fread(key, sizeof(*key), 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    return code;
}

int key_to_file(key key, const char *file)
{
    FILE *fp = fopen(file, "wb");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fwrite(&key, sizeof(key), 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    return code;
}

static const uint8_t pc1[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1 , 58, 50, 42, 34, 26, 18,
    10, 2,  59, 51, 43, 35, 27,
    19, 11, 3,  60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7 , 62, 54, 46, 38, 30, 22,
    14, 6,  61, 53, 45, 37, 29,
    21, 13, 5,  28, 20, 12, 4
};

static const uint8_t pc2[48] = {
     14, 17, 11, 24, 1,  5,
     3,  28, 15, 6,  21, 10,
     23, 19, 12, 4,  26, 8,
     16, 7,  27, 20, 13, 2,
     41, 52, 31, 37, 47, 55,
     30, 40, 51, 45, 33, 48,
     44, 49, 39, 56, 34, 53,
     46, 42, 50, 36, 29, 32
};

key key_sub_pc1(key orig_key)
{
    return permute(orig_key, 64, 56, pc1);
}

key key_sub_pc2(key orig_key)
{
    return permute(orig_key, 56, 48, pc2);
}
