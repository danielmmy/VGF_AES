#include <stdint.h>

#include "gf_complete.h"


/*
Function to compute the matrix x vector multiplication for the SBox affine transformation.
num -> the byte for the multiplication
S -> the matrix value in a byte(before shifting)
*/
unsigned int gf8_at_multiply(uint8_t num, uint8_t S);
