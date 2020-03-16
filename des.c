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
        char *padded = malloc(n + 1 + sizeof(uint64_t)-r);
        if (!padded)
            return NULL;

        memcpy(padded, input, n);
        memset(padded + n, 0, sizeof(uint64_t)-r+1);
        return padded;
    }

    return strdup(input);
}

struct pair {
    uint32_t d;
    uint32_t c;
};

#define PAIR_FIRST(pairs) pairs[0]

#define PAIR_FIRST_C(pairs) pairs[0].c

#define PAIR_FIRST_D(pairs) pairs[0].d


void load_split(struct pair *pair, key subkey)
{
    memcpy(pair, &subkey, sizeof(*pair));
    uint8_t save = (pair->d & (0xF << 28)) >> 28;
    pair->d &= 0x0FFFFFFF;
    // we now, need to take the last 4 bits from the first 32 bits and
    // make them be the first 4 bits. We should take into consideration that
    // the value of the key has 4 bits that is 0000 (that's from the permutation)
    // we can load the 4 bytes into d and then shift it by 4 right and append the last
    // 4 bits from
    pair->c <<= 4;
    pair->c |= save;
}

static uint32_t shift_rotate_left(uint32_t n)
{
    // first we move the bit that we want to rotate to the left
    // we take care the always cut the last 4 bits
    // then we attach that bit we just trimmed from the left to the right
    return ((n << 1) & (0x0FFFFFFF)) | ((n & ( 1 << 27)) >> 27);
}

#define ROUNDS 16

const uint8_t left_shifts[ROUNDS] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

static void shift_rotate_left_pairs(uint32_t c0, uint32_t d0, struct pair* pairs)
{
    for (size_t i = 0; i < ROUNDS; i++) {
        if (i == 0) {
            uint32_t c1 = c0;
            uint32_t d1 = d0;
            for (size_t j = 0; j < left_shifts[i]; j++) {
                c1 = shift_rotate_left(c1);
                d1 = shift_rotate_left(d1);
            }
            pairs[i].c = c1;
            pairs[i].d = d1;
            continue;
        }
        uint32_t c = pairs[i-1].c;
        uint32_t d = pairs[i-1].d;
        for (size_t j = 0; j <  left_shifts[i]; j++) {
            c = shift_rotate_left(c);
            d = shift_rotate_left(d);
        }
        pairs[i].c = c;
        pairs[i].d = d;
    }
}

static void dump_pairs(struct pair* pairs);

int des_encrypt(key inkey, const char *input)
{
    char *padin = input_with_padding(input);
    if (!padin)
        return EXIT_FAILURE;

    const key skey = subkey(inkey);
    struct pair pairs[ROUNDS+1] = {0};
    load_split(&PAIR_FIRST(pairs), skey);
    dump_stdout(&skey, sizeof(skey));

    assert(skey == 0xF0CCAAF556678F);

    dump_stdout(&PAIR_FIRST_C(pairs), sizeof(PAIR_FIRST_C(pairs)));
    dump_stdout(&PAIR_FIRST_D(pairs), sizeof(PAIR_FIRST_D(pairs)));

    assert(pairs[0].d == 0x556678F);
    assert(pairs[0].c == 0xF0CCAAF);

    shift_rotate_left_pairs(PAIR_FIRST_C(pairs), PAIR_FIRST_D(pairs), &pairs[1]);

    dump_pairs(pairs);

    assert(pairs[1].c == 0xE19955F);
    assert(pairs[1].d == 0xAACCF1E);

    assert(pairs[2].c == 0xC332ABF);
    assert(pairs[2].d == 0x5599E3D);

    assert(pairs[14].c == 0xFE19955);
    assert(pairs[14].d == 0xEAACCF1);

    assert(pairs[15].c == 0xF866557);
    assert(pairs[15].d == 0xAAB33C7);

    assert(pairs[16].c == 0xF0CCAAF);
    assert(pairs[16].d == 0x556678F);

    free(padin);
    return EXIT_SUCCESS;
}

static void dump_pairs(struct pair* pairs)
{
    // 0x + 4 byte value hex + '\0'
    const size_t size = 2 + 2*sizeof(PAIR_FIRST_C(pairs)) + 1;
    char buffer[size];
    for(size_t i = 0; i < ROUNDS+1; i++) {
        dump_buffer(&pairs[i].c, sizeof(pairs[i].c), buffer);
        printf("C(%lu) = %s\n",i,  buffer);
        dump_buffer(&pairs[i].d, sizeof(pairs[i].d), buffer);
        printf("D(%lu) = %s\n", i, buffer);
        puts("");
    }
}
