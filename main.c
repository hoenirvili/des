#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "key.h"
#include "des.h"

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

    return des_encrypt(args.key, args.input);
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
    struct args args = {0};
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

