#pragma once

#include <sys/types.h>
#include <stdio.h>

extern void dump_stdout(const void *b, size_t size);
extern void dump_file(const void *b, size_t size, FILE *out);
extern void dump_buffer(const void *b, size_t size, char *buffer);
