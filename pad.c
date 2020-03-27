#include <string.h>

#include "pad.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

static const char padds[] = { 0x8, 0x7, 0x6, 0x5, 0x4, 0x3, 0x2, 0x1 };

struct pad_input pad(const char* input, size_t n)
{
    const size_t i = n % 8;
    char c = padds[i];
    size_t blocks = (n + c) / 8;
    uint64_t *padded = calloc(blocks, sizeof(*padded));
    if (!padded)
        return (struct pad_input){0};

    memcpy(padded, input, n);
    // set the bytes with padding
    char *bytes = (char*)padded;
    memset(&bytes[n], c, c);

    return (struct pad_input){ .blocks = padded, n = blocks};
}

size_t pad_remove(struct pad_input pi)
{
    size_t last = pi.n - 1;
    char *byte = (char*)(&pi.blocks[last]);
    char c = byte[sizeof(*pi.blocks)-1]; // lastbyte
    char *s = memchr(byte, c, sizeof(*pi.blocks));
    memset(s, 0, c);
    return c;
}
