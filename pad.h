#pragma once


/**
 * pad
 *
 * pads the input string with bytes in PKCS5 specified in RFC2898 section 6.1.1 number 4
 *
 * input - input string terminated with '\0'
 *
 * The returned is a new ptr to the resulting padded string. In case of errors this will return NULL
 * and setting errno to the appropriate error code
 */
 extern char *pad(const char *input);
