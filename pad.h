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
 * input - input string terminated with '\0'
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
  * In case of error this return -1
  *
  */
 extern ssize_t pad_remove(struct pad_input pi);
