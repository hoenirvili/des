#pragma once

#include <stdlib.h>

#include "key.h"


/**
 * des_encrypt
 *
 * given a key and an input encrypts the input using the des algorithm.
 */
extern char *des_encrypt(key key, const char *input);

/**
 * des_descrypt
 *
 * given a key and an encrypted input try and decrypt using the des algorithm
 */
extern char *des_decrypt(key key, const char *input);
