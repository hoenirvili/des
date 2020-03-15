#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "key.h"
#include "dump.h"

int key_from_file(key *key, const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fread(key, sizeof(*key), 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    return code;
}

void key_to_hex(key key, char *out)
{
    dump_buffer(&key, sizeof(key), out);
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
    FILE *fp = fopen(file, "w");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fwrite(&key, sizeof(key), 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    return code;
}

const uint8_t pc1[KEY_ENC_BITS_USED_SIZE] = {
    57, 49, 41, 33, 25, 17, 9,
    1 , 58, 50, 42, 34, 26, 18,
    10, 2,  59, 51, 43, 35, 27,
    19, 11, 3,  60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7 , 62, 54, 46, 38, 30, 22,
    14, 6,  61, 53, 45, 37, 29,
    21, 13, 5,  28, 20, 12, 4
};

const size_t KEY_N_BITS = KEY_SIZE * 8;

// _subkey generates a subkey based of a pc provided
// this generates the first round of permutation
static key _subkey(key orig_key, const uint8_t *pc)
{
    key sub = 0;
    for (size_t i = 0; i < KEY_ENC_BITS_USED_SIZE; i++)
        sub |= (orig_key >> (KEY_N_BITS - pc[i]) & 0x1) << (KEY_ENC_BITS_USED_SIZE - 1 - i);
    return sub;
}

key subkey(key orig_key)
{
    return _subkey(orig_key, pc1);
}
