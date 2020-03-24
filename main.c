#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "key.h"
#include "des.h"
#include "log.h"

static const char *optstring="ghk:";

static const struct option longopts[] = {
    {"generate-key", no_argument, NULL, 'g'},
    {"key", required_argument, NULL, 'k'},
    {"help", no_argument, NULL, 'h'},
    {"verbose", no_argument, NULL, 'v'},
    {},
};

struct input {
    char *value;
    size_t cap;
    size_t len;
};

struct args {
    bool generate_key;  /*generate random 64 bit key*/
    struct input input; /*input for encryption or decryption*/
    key key;            /*the des private key*/
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
    ssize_t size = BUFSIZ;
    for (;;) {
        if (size == 0) {
            // grow the buffer
            char *nbuff = realloc(buff, offset + BUFSIZ);
            if (!nbuff) {
                log_error("cannot grow the buffer after hitting size=0");
                free(buff);
                return EXIT_FAILURE;
            }
            buff = nbuff;
            size = BUFSIZ;
        }

        ssize_t n = fread(&buff[offset], sizeof(*buff), size, fp);
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
            size = BUFSIZ;
        }

        if ((n > 0) && (n != BUFSIZ)) {
            offset += n;
            size -= n;
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
        err = key_to_file(key, key_name);
        if (err < 0)
            return err;
        return EXIT_SUCCESS;
    }

    if (!args.input.value) {
        log_debug("No input to encrypt");
        return EXIT_FAILURE;
    }

    char *out = des_encrypt(args.key, args.input.value);
    printf("%s\n", out);

    free(out);
    return EXIT_SUCCESS;
}

static const char *help_menu =
"des encryption cipher                              \n"
"des -k des.key [-g] input                          \n"
"--help, -h             print out the help message  \n"
"--verbose, -v          run program in verbose mode \n"
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
    while ((ch = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
        switch (ch) {
            case 'g':
                args.generate_key = true;
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
            case 'v':
                log_activate(true);
                break;
            case '?':
                log_error("Invalid argument: %s\n", optarg);
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
        log_error("[!] It requires just one input message, aka \"input\", \"one multi word input\" \n");
        return EXIT_FAILURE;
    }

    if (!args.input.value) {
        int err = loadto(&args.input.value, &args.input.len, stdin);
        if (err) {
            log_error("Cannot read input from pipe to stdin");
            return err;
        }
        if (args.input.len < BUFSIZ)
            args.input.cap = BUFSIZ;
        else
            args.input.cap = args.input.len;
    }

    int err = run(args);
    if (err != 0)
        log_error("Internal error");
    return err;
}
