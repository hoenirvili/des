#pragma once

#include <stdlib.h>
#include <stdint.h>

struct pad_input{
    uint64_t *blocks;
    uint64_t n;
};

/**
 * pad
 *
 * Pads the input string with bytes in PKCS5 specified in RFC2898 section 6.1.1 number 4
 *
 * input - input string (that is not necessary be terminated with \0)
 * len   - how many bytes we have in *input*
 *
 * The returned is a new ptr to the resulting padded string. In case of errors this will return NULL
 * and setting errno to the appropriate error code
 */
 extern struct pad_input pad(const char *input, size_t n);



 /**
  * pad_remove
  *
  * Removes padding added by *pad*
  *
  * pi  - input padded struct that holds the input 64 bit blocks and len
  *
  * In case of error this return -1
  */
 extern size_t pad_remove(struct pad_input pi);
