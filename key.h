#pragma once

#include <stdint.h>

/**
 * key defines a des key
 */
typedef uint64_t key;

/**
 * KEY_SIZE defines how big is the key in bytes
 */
#define KEY_SIZE sizeof(uint64_t)

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

