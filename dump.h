#pragma once

#include <sys/types.h>
#include <stdio.h>


/**
 * dump_file
 *
 * dump all hex bytes from b to stdout
 * the hex dump start with 0x and it follows every hex byte from b
 */
extern void dump_stdout(const void *b, size_t size);

/**
 * dump_file
 *
 * dump all hex bytes from b in the file
 * the hex dump start with 0x and it follows every hex byte from b
 */
extern void dump_file(const void *b, size_t size, FILE *out);

/**
 * dump_buffer
 * the hex dump start with 0x and it follows every hex byte from b
 * Please, make sure that the buffer is at least of size 2 * size + 2 + 1
 * 2*size => every byte is basically an hex like F2
 * +2 => because of the prefix 0x
 * +1 => because of '\0'
 */
extern void dump_buffer(const void *b, size_t size, char *buffer);
