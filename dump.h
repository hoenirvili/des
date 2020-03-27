#pragma once

#include <stdio.h>

/**
 * dump_file
 *
 * dump all hex bytes from b in the file
 * the hex dump start with 0x and it follows every hex byte from b
 */
extern void dump_file(const void *b, size_t size, FILE *out);
