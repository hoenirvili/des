#include "dump.h"
#include <machine/endian.h>
#include <stdio.h>
#include <assert.h>

void dump(const void *b, size_t size, FILE *out)
{
    assert(size > 0);
    const char *bytes = b;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    for (ssize_t i = size; i>=0; i--) {
#else
    for (ssize_t i = 0; i<n; i++) {
#endif
        fprintf(out, "%02x", (unsigned char)bytes[i]);
    }
    puts("");
}

void dump_stdout(const void *b, size_t size)
{
    assert(size > 0);
    printf("0x");
    const char *bytes = b;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    for (ssize_t i = size; i>=0; i--) {
#else
    for (ssize_t i = 0; i<n; i++) {
#endif
        printf("%02x", (unsigned char)bytes[i]);
    }
    puts("");
}

void dump_file(const void *b, size_t size, FILE *out)
{
    assert(size > 0);
    fprintf(out, "0x");
    const char *bytes = b;
#if __BYTE_ORDER == __LITTLE_ENDIAN
    for (ssize_t i = size; i>=0; i--) {
#else
    for (ssize_t i = 0; i<n; i++) {
#endif
        fprintf(out, "%02x", (unsigned char)bytes[i]);
    }
    puts("");
}

void dump_buffer(const void *b, size_t size, char *buffer)
{
    assert(size > 0);
    const char *bytes = b;
    buffer[0] = '0';
    buffer[1] = 'x';
#if __BYTE_ORDER == __LITTLE_ENDIAN
    for (ssize_t i = size; i>=0; i--) {
#else
    for (ssize_t i = 0; i<n; i++) {
#endif
        sprintf(&buffer[2], "%02x", (unsigned char)bytes[i]);
    }
    puts("");
}
