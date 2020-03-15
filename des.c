#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dump.h"
#include "des.h"

#define CIPHER_BLOCK_SIZE KEY_SIZE

// The 64 bits of the input block to be enciphered are
// first subjected to the following permutation,
// called the initial permutation IP.
// That is the permuted input has bit 58 of the input
// as its first bit, bit 50 as its second bit,
// and so on with bit 7 as its last bit.
const char IP[] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

const char IP1[] ={
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41,  9, 49, 17, 57, 25
};

static char *input_with_padding(const char *input)
{
    const size_t n = strlen(input);
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

struct pair { uint32_t c; uint32_t d; };

// TODO(honier): finish this
void load_split(struct pair *pair, key subkey)
{
    // load the first 32 bits of the key
    memcpy(&pair->c, &subkey, sizeof(subkey));

    // save the last 4 bits because it belongs to the next half
    uint8_t save = pair->c & 0xF0000000;
    save = save << 4;
    // we only need the first 28 bits
    pair->c = pair->c & 0x0FFFFFFF;

    // we now, need to take the last 4 bits from the first 32 bits and
    // make them be the first 4 bits. We should take into consideration that
    // the value of the key has 4 bits that is 0000 (that's from the permutation)
    // we can load the 4 bytes into d and then shift it by 4 right and append the last
    // 4 bits from
    union half {
        key key;
        uint32_t words;
    } { .key = subkey };


}

int des_encrypt(key inkey, const char *input)
{
    char *padin = input_with_padding(input);
    if (!padin)
        return EXIT_FAILURE;

    const key skey = subkey(inkey);
    struct pair pair;
    load_split(&pair, skey);

    dump_stdout(&skey, sizeof(skey));
    dump_stdout(&pair.c, sizeof(pair.c));
    dump_stdout(&pair.d, sizeof(pair.d));


    assert(pair.c == 0x556678F);

    assert(pair.d == 0xF0CCAAF); //////

    free(padin);
    return EXIT_SUCCESS;
}
