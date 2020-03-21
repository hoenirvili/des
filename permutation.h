#pragma once

#include <stdint.h>
#include <stdlib.h>

/**
 * permute permutes a value based on the table provided in big endian
 *
 * value     - the value in BE to be permuted
 * bitsz     - number of bits that we want to permute
 * bitsz_out - the amount of bits that are the direct result of the permutation
 * pc        - pc is the permutation table, the length of pc should be exactly bitsz
 */
extern uint64_t permute(uint64_t value, size_t bitsz, size_t bitsz_out, const uint8_t *pc);
