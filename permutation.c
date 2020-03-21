#include "permutation.h"

uint64_t permute(uint64_t value, size_t bits, size_t bits_out, const uint8_t *pc)
{
    uint64_t n = 0;
    for (size_t i = 0; i < bits_out; i++)
        n |= (value >> (bits - pc[i]) & 0x1) << (bits_out - 1 - i);
    return n;
}
