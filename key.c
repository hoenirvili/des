#include "key.h"

#include <stdio.h>
#include <stdlib.h>

int key_from_file(key *key, const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fread(key, KEY_SIZE, 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    return code;
}

void key_to_hex(key key, char *out)
{
    sprintf(out, "%#llx", key);
}

int key_generate(key *key)
{
    FILE *fp = fopen("/dev/urandom", "r");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fread(key, KEY_SIZE, 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    return code;
}

int key_to_file(key key, const char *file)
{
    FILE *fp = fopen(file, "w");
    if (!fp)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (fwrite(&key, KEY_SIZE, 1, fp) < 0)
        code = EXIT_FAILURE;

    fclose(fp);
    return code;
}
