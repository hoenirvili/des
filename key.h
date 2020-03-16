#pragma once

#include <stdint.h>

typedef uint64_t key;


#define KEY_SIZE sizeof(uint64_t)

#define KEY_SIZE_STR KEY_SIZE + 1

#define KEY_SIZE_HEX_STR (KEY_SIZE*2) + 2 + 1

/**
 * key_from_file loads the 64 bit key from the file in BE format
 */
extern int key_from_file(key *key, const char *file);

/**
 * key_to_file writes the provided key in the file
 */
extern int key_to_file(key key, const char *file);

/**
 * key_to_hex writes the hex byte representation of the key into out buffer
 */
extern void key_to_hex(key key, char *out);

/**
 * key_generate loads the *key with a random 64 byte value
 * the source of randomness is /dev/urandom
 */
extern int key_generate(key *key);

/**
 * key_sub_pc1 returns the 56 bit key from the initial permutation with it's PC1.
 * The resulting key is in BE format. The input key orig_key should be in BE format
 */
extern key key_sub_pc1(key orig_key);

/**
 * key_sub_pc returns the 48 bit key from the initial permutation with it's PC2.
 * The resulting key is in BE format. The input key orig_key should be in BE format
 */
extern key key_sub_pc2(key orig_key);

