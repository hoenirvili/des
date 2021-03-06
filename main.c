#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "endianess.h"
#include "key.h"
#include "pad.h"
#include "des.h"
#include "log.h"

static const char *optstring="g::hk:d";

static const struct option longopts[] = {
    {"generate-key", optional_argument, NULL, 'g'},
    {"key", required_argument, NULL, 'k'},
    {"help", no_argument, NULL, 'h'},
    {"decrypt",no_argument, NULL, 'd'},
    {0},
};

struct input {
    char *value;
    size_t cap;
    size_t len;
};

struct args {
    bool generate_key;      /*generate random 64 bit key*/
    const char *key_name;   /*key file name to generate, default to des.key*/
    struct input input;     /*input for encryption or decryption*/
    bool decrypt;           /*true if we want to decrypt the input*/
    key key;                /*the des private key*/
};

static const char *key_name = "des.key";

#define KEY_SIZE_HEX_STR (KEY_SIZE * 2) + 2 + 1

static int loadto(char **buffer, size_t *n, FILE* fp)
{
    *buffer = NULL;
    char *buff = calloc(BUFSIZ, sizeof(*buff));
    if (!buff) {
        log_error("calloc failed");
        return EXIT_FAILURE;
    }
    size_t offset = 0;
    ssize_t capacity = BUFSIZ;
    for (;;) {
        if (capacity == 0) {
            // grow the buffer
            char *nbuff = realloc(buff, offset + BUFSIZ);
            if (!nbuff) {
                log_error("cannot grow the buffer after hitting size=0");
                free(buff);
                return EXIT_FAILURE;
            }
            buff = nbuff;
            capacity = BUFSIZ;
        }
        ssize_t n = fread(&buff[offset], sizeof(*buff), capacity, fp);
        if (n == BUFSIZ) {
            offset += n;
            // grow the buffer
            char *nbuff = realloc(buff, offset + BUFSIZ);
            if (!nbuff) {
                log_error("cannot grow the buffer after hitting BUFSIZ");
                free(buff);
                return EXIT_FAILURE;
            }
            buff = nbuff;
            capacity = BUFSIZ;
        }
        if ((n > 0) && (n != BUFSIZ)) {
            offset += n;
            capacity -= n;
        }
        if (n < 0) {
            if ((ferror(fp) != 0)) {
                log_error("fread failed");
                free(buff);
                return EXIT_FAILURE;
            }
        }
        if (n == 0)
            break;
    }

    *n = offset;
    *buffer = buff;
    return EXIT_SUCCESS;
}

static int run(struct args args)
{
    if (args.generate_key) {
        log_debug("Generate 64 random key");
        key key;
        int err = key_generate(&key);
        if (err < 0)
            return err;
        err = key_to_file(key, args.key_name);
        if (err < 0)
            return err;
        return EXIT_SUCCESS;
    }

    if (!args.input.value) {
        log_debug("No input to encrypt");
        return EXIT_FAILURE;
    }

    if (!args.key) {
        log_error("No key passed, please try and pass for example a key like --key=des,key, -kdes.key");
        return EXIT_FAILURE;
    }

    if (!args.decrypt) {
        // pad inpyt bytes in PKCS5 form
        struct pad_input pi = pad(args.input.value, args.input.len);
        if (!pi.blocks) {
            log_error("padding input failed");
            return EXIT_FAILURE;
        }

        // we just loaded from stdin blocks in machine form (le)
        // we should transform every 64bit block in BE from
        // before running the encryption
        for (size_t i = 0; i < pi.n; i++)
            pi.blocks[i] = htobe64(pi.blocks[i]);

        des_encrypt(args.key, pi.blocks, pi.n);
        // dump every bytes to stdout by default
        fwrite(pi.blocks, sizeof(*pi.blocks), pi.n, stdout);
        free(pi.blocks);
    } else {
        if (args.input.len % sizeof(uint64_t) != 0) {
            log_error("Invalid des encrypted format as input, every block should be 64bit");
            return EXIT_FAILURE;
        }
        const size_t n = args.input.len / sizeof(uint64_t);
        uint64_t *const blocks = (uint64_t*)args.input.value;
        des_decrypt(args.key, blocks, n);
        const size_t bytes_removed = pad_remove((struct pad_input) {.blocks = blocks, .n = n});
        size_t nblocks_without_padding = n * sizeof(*blocks) - bytes_removed;
        fwrite(blocks, sizeof(char), nblocks_without_padding, stdout);
    }

    return EXIT_SUCCESS;
}

static const char *help_menu =
"des encryption cipher in ECB mode and using PKCS5.0 padding    \n"
"des -k des.key [-g] input                                      \n"
"--help, -h             print out the help message              \n"
"--decrypt, -d          decrypt the input                       \n"
"--generate-key, -g     generate random 64bit key               \n"
"--key, -k              input key for enc and dec";


int main(int argc, char **argv)
{

    if (argc == 1) {
        puts(help_menu);
        return EXIT_SUCCESS;
    }

    int ch = 0;
    struct args args = { 0 };
    while ((ch = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
        switch (ch) {
            case 'g':
                args.generate_key = true;
                args.key_name = (optarg && strlen(optarg)>0) ? optarg : key_name;
                break;
            case 'h':
                puts(help_menu);
                return EXIT_SUCCESS;
            case 'k':
                if (key_from_file(&args.key, optarg) < 0) {
                    log_error("Cannot load from file '%s' key", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case ':':
                log_error("Invalid option value %c\n", optopt);
                break;
            case 'd':
                args.decrypt = true;
                break;
            case '?':
                log_error("Invalid argument: %c\n", optopt);
                return EXIT_FAILURE;
        }
    }

    if (optind < argc) {
        if (!(argv[optind])) {
            optind++;
            goto cnt;
        }
        args.input.value = strdup(argv[optind]);
        if (!args.input.value) {
            log_error("Cannot get input from args");
            return EXIT_FAILURE;
        }
        args.input.len = strlen(args.input.value);
        args.input.cap = args.input.len + 1;
        optind++;
    }

cnt:
    if (optind < argc) {
        log_error("It requires just one input message, like this \"input\", or \"one multi word input\"");
        return EXIT_FAILURE;
    }

    int err = EXIT_SUCCESS;
    if (!args.input.value && !args.generate_key) {
        if (isatty(fileno(stdin))) {
            log_error("Passing input from terminal not supported, use args or pipe the input");
            return EXIT_FAILURE;
        }

        // we load from stdin bytes in machine form(le)
        err = loadto(&args.input.value, &args.input.len, stdin);
        if (err) {
            log_error("Cannot read input from pipe to stdin");
            return err;
        }

        if (args.input.len < BUFSIZ)
            args.input.cap = BUFSIZ;
        else
            args.input.cap = args.input.len;
    }


    err = run(args);
    free(args.input.value);
    return err;
}
