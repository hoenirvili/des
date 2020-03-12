#pragma once

#include <stdint.h>

typedef uint64_t key;

#define KEY_ENC_BITS_USED_SIZE 56

#define KEY_SIZE sizeof(uint64_t)

#define KEY_SIZE_STR KEY_SIZE + 1

#define KEY_SIZE_HEX_STR (KEY_SIZE*2) + 2 + 1

extern int key_from_file(key *key, const char *file);

extern int key_to_file(key key, const char *file);

extern void key_to_hex(key key, char *out);

extern int key_generate(key *key);

extern key subkey(key orig_key);
