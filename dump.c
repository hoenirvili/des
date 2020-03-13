#include <stdio.h>
#include <string.h>
#include "dump.h"

void dump_stdout(const void *b, size_t size)
{
    printf("0x");
    const unsigned char *bytes = b;
    for (ssize_t i = 0; i < size; i++)
        printf("%02x", bytes[i]);
    puts("");
}

void dump_file(const void *b, size_t size, FILE *out)
{
    fprintf(out, "0x");
    const char *bytes = b;
   for (ssize_t i = 0; i < size; i++)
        fprintf(out, "%02x", (unsigned char)bytes[i]);
}

void dump_buffer(const void *b, size_t size, char *buffer)
{
    const unsigned char *bytes = b;
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer = &buffer[2];
    memset(buffer, 0, ((2 * size) + 1));
    for (ssize_t i = 0, j = 0; i < size && j < size * 2; i++, j+=2)
        sprintf(&buffer[j], "%02x", bytes[i]);
}
