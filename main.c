#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "key.h"

const char *optstring="ghk:";

static const struct option longopts[] = {
    {"generate-key", no_argument, NULL, 'g'},
    {"key", required_argument, NULL, 'k'},
    {"help", no_argument, NULL, 'h'},
    {},
};

struct args {
    bool generate_key; /*generate random 64 bit key*/
    char *input; /*input text*/
    key key;
};

const char *key_name = "des.key";

static uint32_t f(uint32_t r0, uint64_t subkey) {
    //TODO(hoenir): implement f
    return 0;
}

static uint32_t subkey(key key) {
    //TODO(hoenir): implement subkey extraction
    return 0;
}

static int encrypt_block(key key, uint64_t chunk)
{
    uint32_t l0 = chunk >> 32; // most significant 32 bits
    uint32_t r0 = chunk & 0x00000000ffffffff; // least significant 32 bits
    uint32_t fres = f(r0, subkey(key)) ^ l0;
    uint64_t res = 0;
    memcpy(&res, &r0, sizeof(r0));
    memcpy((char*)&res + sizeof(r0), &fres, sizeof(fres));
    printf("[*] encrypt 64 bytes, chunk %#llx l0:%#x r0:%#x f: %#x res: %#llx\n", chunk, l0, r0, fres, res);
    return res;
}

static int encrypt_des(key key, char *input)
{
    size_t n = strlen(input);
    size_t blocks = n / sizeof(uint64_t);
    size_t remaning = n % sizeof(uint64_t);

    printf("[*] Input size in bytes: %lu, blocks: %lu remaning: %lu\n", n, blocks, remaning);

    size_t i = 0;
    uint64_t chunk = 0;
    for (; i < blocks; i++) {
        memcpy(&chunk, &input[i], sizeof(chunk));
        encrypt_block(key, chunk);
    }

    if (remaning > 0) {
        chunk = 0;
        memcpy(&chunk, &input[i], remaning);
    }

    return 0;
}

int run(struct args args)
{
    if (args.generate_key) {
        puts("[*] Generating random 64 bit key");
        key key;
        int err = key_generate(&key);
        if (err < 0)
            return err;
        err = key_to_file(key, key_name);
        if (err < 0)
            return err;
        return EXIT_SUCCESS;
    }

    if (!args.input) {
        puts("[!] No input to encrypt");
        return EXIT_FAILURE;
    }

    char hex_key[KEY_SIZE_HEX_STR];
    key_to_hex(args.key, hex_key);
    printf("[*] Encrypt, input: %s\n", args.input);
    printf("[*] Using the key hex: %s\n", hex_key);

    return encrypt_des(args.key, args.input);
}

const char *help_menu =
"des encryption cipher                              \n"
"des -k des.key [-g] input                          \n"
"--help, -h             print out the help message  \n"
"--generate-key, -g     generate random 64bit key   \n"
"--key, -k              input key for enc and dec";


int main(int argc, char **argv)
{
    if (argc == 1) {
        puts(help_menu);
        return EXIT_SUCCESS;
    }

    int ch = 0;
    struct args args = { 0 };
    while (ch != -1) {
        ch = getopt_long(argc, argv, optstring, longopts, NULL);
        switch (ch) {
            case 'g':
                args.generate_key = true;
                break;
            case 'h':
                puts(help_menu);
                return EXIT_SUCCESS;
            case 'k':
                if (key_from_file(&args.key, optarg) < 0) {
                    printf("[*] Canot load from file '%s' key", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case '?':
                fprintf(stderr, "[!] Invalid argument: %s\n", optarg);
                return EXIT_FAILURE;
        }
    }

    if (optind < argc) {
        args.input = argv[optind];
        optind++;
    }

    if (optind < argc) {
        fprintf(stderr, "[!] It requires just one input message, aka \"input\", \"one multi word input\" \n");
        return EXIT_FAILURE;
    }

    return run(args);
}

