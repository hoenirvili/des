#include "pad.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define ARRAY_SIZE (sizeof(arr)/sizeof(arr[0]))

static const char padds[] = { 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1 };

char *pad(const char* input)
{
    const size_t n = strlen(input);
    const size_t i = n % 8;
    char c = padds[i];
    char *padded = calloc(n + c + 1, sizeof(*padded));
    if (!padded)
        return NULL;
    memcpy(padded, input, n);
    memset(&padded[n], c, c);
    return padded;
}
