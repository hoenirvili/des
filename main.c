#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>


const char *optstring="ghk:";

static const struct option longopts[] = {
    {"generate-key", no_argument, NULL, 'g'},
    {"key", required_argument, NULL, 'k'},
    {"help", no_argument, NULL, 'h'},
    {},
};

#define DEFAULT_KEY_SIZE_STR sizeof(uint64_t) + 1
#define DEFAULT_KEY_SIZE sizeof(uint64_t)

struct args {
    bool generate_key; /*generate random 64 bit key*/
    char *input; /*input text*/
    char input_key[DEFAULT_KEY_SIZE_STR]; /*input key to use for encryption or decryption*/
};

const char *key_name = "des.key";

int64_t random_key()
{
    FILE *fp = fopen("/dev/urandom", "r");
    if (!fp)
        return -EXIT_FAILURE;

    int64_t r = 0;
    int err = fread(&r, sizeof(int64_t), 1, fp);
    if (err < 0) {
        fclose(fp);
        return -EXIT_FAILURE;
    }

    fclose(fp);
    return r;
}

int generate_key(const char *name)
{
    int64_t r = random_key();
    if (r < 0)
        return r;

    const uint64_t key = r;

    FILE *out = fopen(name, "w");
    if (!out)
        return EXIT_FAILURE;

    int err = fwrite(&key, sizeof(uint64_t), 1, out);
    if (err < 0) {
        fclose(out);
        return EXIT_FAILURE;
    }

    fclose(out);
    return EXIT_SUCCESS;
}

static void hex_dump(const char *input, char *out)
{
    size_t n = strlen(input);
    out[0] = '0';
    out[1] = 'x';
    size_t i = 2;
    for (; i < n; i++)
        sprintf(&out[i], "%02x", input[i]);
    out[i] = '\0';
}

#define DEFAULT_HEX_KEY_SIZE DEFAULT_KEY_SIZE_STR + 2

int run(struct args args)
{
    if (args.generate_key) {
        printf("Generating random 64 bit key");
        return generate_key(key_name);
    }

    if (!args.input) {
        puts("No input to encrypt");
        return EXIT_FAILURE;
    }

    char hex_key[DEFAULT_HEX_KEY_SIZE] = { 0 };
    hex_dump(args.input_key, hex_key);
    printf("Encrypt, input: %s\n", args.input);
    printf("Using the key hex: %s\n", hex_key);

    return 0;
}

const char *help_menu =
"des encryption cipher                              \n"
"des -k des.key [-g] input                          \n"
"--help, -h             print out the help message  \n"
"--generate-key, -g     generate random 64bit key   \n"
"--key, -k              input key for enc and dec";


// dump_key_content dumps the key provided from a file path
static int dump_key_content(char* out, size_t size_out, const char *from_file)
{
    assert(out != NULL);
    assert(from_file != NULL);
    assert(size_out > 0);

    FILE *fp = fopen(from_file, "r");
    if (!fp)
        return EXIT_FAILURE;

    int err = fread(out, size_out, 1, fp);
    if (err < 0) {
        fclose(fp);
        return EXIT_FAILURE;
    }

    return fclose(fp);
}

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
                if (dump_key_content(args.input_key, DEFAULT_KEY_SIZE, optarg) < 0)
                    return EXIT_FAILURE;
                break;
            case '?':
                fprintf(stderr, "Invalid argument: %s\n", optarg);
                return EXIT_FAILURE;
        }
    }

    if (optind < argc)
        args.input = argv[optind];

    return run(args);
}

