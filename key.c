#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>

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
    *key = htole64(*key); // always the key is in BE
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

const uint8_t pc1[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1 , 58, 50, 42, 34, 26, 18,
    10, 2,  59, 51, 43, 35, 27,
    19, 11, 3,  60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7 , 62, 54, 46, 38, 30, 22,
    14, 6,  61, 53, 45, 37, 29,
    21, 13, 5,  28, 20, 12, 4
};

const uint8_t pc2[48] = {
     14, 17, 11, 24, 1,  5,
     3,  28, 15, 6,  21, 10,
     23, 19, 12, 4,  26, 8,
     16, 7,  27, 20, 13, 2,
     41, 52, 31, 37, 47, 55,
     30, 40, 51, 45, 33, 48,
     44, 49, 39, 56, 34, 53,
     46, 42, 50, 36, 29, 32
};

// _subkey generates a subkey based of a pc provided
// this generates the first round of permutation of the key
static key _subkey56(key orig_key, const uint8_t *pc)
{
    key sub = 0;
    for (size_t i = 0; i < 56; i++)
        // 56 - 1 - i basically the hole resulting key has a width of 56 bits
        // when we compose the key, we are should compose it again in BE format
        // that's why we shift to the left in reverse, i=0, 55, i=1, 54 ..
        // 64 - pc[i] it's just computing how much to shift to the right in order to land that
        // bit we are interested in the first position. With the 0x1 mask we can have always value of 0x0 or 0x1
        sub |= (orig_key >> (64 - pc[i]) & 0x1) << (56 - 1 - i);
    return sub;
}


static key _subkey48(key orig_key, const uint8_t *pc)
{
    key sub = 0;
    for (size_t i = 0; i < 48; i++)
        sub |= (orig_key >> (56 - pc[i]) & 0x1) << (48 - 1 - i);
    return sub;
}

key key_sub_pc1(key orig_key) { return _subkey56(orig_key, pc1); }
key key_sub_pc2(key orig_key) { return _subkey48(orig_key, pc2); }
