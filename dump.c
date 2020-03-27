#include <string.h>

#include "dump.h"

void dump_file(const void *b, size_t size, FILE *out)
{
    fprintf(out, "0x");
    const char *bytes = b;
    for (ssize_t i = 0; i < size; i++)
        fprintf(out, "%02x", (unsigned char)bytes[i]);
}
