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
const uint8_t IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17,  9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

const uint8_t IP1[64] ={
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

const uint8_t left_shifts[ROUNDS] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

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

/* static void dump_pairs(struct pair* pairs) */
/* { */
/*     // 0x + 4 byte value hex + '\0' */
/*     const size_t size = 2 + 2*sizeof(PAIR_FIRST_C(pairs)) + 1; */
/*     char buffer[size]; */
/*     for(size_t i = 0; i < ROUNDS+1; i++) { */
/*         dump_buffer(&pairs[i].c, sizeof(pairs[i].c), buffer); */
/*         printf("C(%lu) = %s\n",i,  buffer); */
/*         dump_buffer(&pairs[i].d, sizeof(pairs[i].d), buffer); */
/*         printf("D(%lu) = %s\n", i, buffer); */
/*         puts(""); */
/*     } */
/* } */

static void load_keys(key *keys, struct pair *pairs)
{
    for (size_t i = 0; i < ROUNDS; i++) {
        keys[i] = pairs[i].c;
        keys[i] <<= 28;
        keys[i] |= pairs[i].d;
    }
}

static void encode_subkeys(key *keys)
{
    for (size_t i = 0; i < ROUNDS; i++)
        keys[i] = key_sub_pc2(keys[i]);
}


#define ARRAY_SIZE(arr) sizeof(arr)/sizeof(arr[0])
static uint64_t initial_permutation(uint64_t m, const uint8_t *pc)
{
    uint64_t ip = 0;
    const size_t size = sizeof(ip) * 8;
    for (size_t i = 0; i < size; i++)
        ip |= (m >> (64 - pc[i]) & 0x1) << (64 - 1 - i);
    return ip;
}

const uint8_t ebit_selection_table[48] = {
      32,  1,  2,  3,  4,  5,
       4,  5,  6,  7,  8,  9,
       8,  9, 10, 11, 12, 13,
      12, 13, 14, 15, 16, 17,
      16, 17, 18, 19, 20, 21,
      20, 21, 22, 23, 24, 25,
      24, 25, 26, 27, 28, 29,
      28, 29, 30, 31, 32,  1
};

const uint8_t S1[4][16] = {
   14, 4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9, 0, 7,
    0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
    4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
};

const uint8_t S2[4][16] = {
    15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
     3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
     0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
    13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
};

const uint8_t S3[4][16] = {
    10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
    13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
    13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
     1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
};

const uint8_t S4[4][16] = {
     7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
    13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
    10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
     3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
};

const uint8_t S5[4][16] = {
     2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
    14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
     4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
    11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
};

const uint8_t S6[4][16] = {
    12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
    10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
     9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
     4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
};

const uint8_t S7[4][16] = {
     4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
    13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
     1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
     6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
};

const uint8_t S8[4][16] = {
    13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
     1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
     7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
     2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};


static uint64_t expand_block(uint32_t block, const uint8_t* pc)
{
    uint64_t expand = 0;
    const size_t size = sizeof(ebit_selection_table) * 8;
    for (size_t i = 0; i < size; i++)
        expand |= (block >> (32 - pc[i]) & 0x1) << (48 - 1 - i);
    return expand;

}
static uint32_t f(uint32_t block, key key)
{
    uint64_t eblock = expand_block(block, ebit_selection_table);
    return  eblock ^ key;
    //TODO(hoenir): finish this
}


int des_encrypt(key inkey, const char *input)
{
    char *padin = input_with_padding(input);
    if (!padin)
        return EXIT_FAILURE;

    // generate subkey based on pc1
    const key skey = key_sub_pc1(inkey);
    // split the 56 bit subkey into c0,d0
    struct pair pairs[ROUNDS+1] = {0};
    load_split(&PAIR_FIRST(pairs), skey);
    // use c0,d0 to generate all 16 pairs c1..c16,d1..d16
    shift_rotate_left_pairs(PAIR_FIRST_C(pairs), PAIR_FIRST_D(pairs), &pairs[1]);
    // merge the 16 pairs into 16 keys
    key keys[ROUNDS] = { 0 };
    load_keys(keys, &pairs[1]);
    // encode all 16 keys using pc2
    encode_subkeys(keys);


    // TODO(hoenir):
    // make the initial permutation for the hole message not just for m
    // this is for testing purposes
    uint64_t m = 0x123456789ABCDEF;
    m = initial_permutation(m, IP);
    /* size_t n = strlen(padin); */
    /* for (size_t i = 0; i < n; i = i + sizeof(m)) { */
    /*     uint64_t m = 0; */
    /*     memcpy(&m, &padin[i], sizeof(m)); */
    /*     m = initial_permutation(m, IP); */
    /*     memcpy(&padin[i], &m, sizeof(m)); */
    /* } */
    uint32_t l = (m >> 32);
    uint32_t r = m;
    uint32_t aux_l = 0;
    for (size_t i = 0; i < ROUNDS; i++) {
        aux_l = r;
        r = l ^ f(r, keys[i]);
        l = aux_l;
    }

    return EXIT_SUCCESS;
}

