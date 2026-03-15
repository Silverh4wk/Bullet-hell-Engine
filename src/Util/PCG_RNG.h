
// A permuted congruential generator (PCG) is a pseudorandom number generation
// algorithm developed in 2014 by Dr. M.E. O'Neill which applies an output
// permutation function to improve the statistical properties of a modulo-2n
// linear congruential generator (LCG). It achieves excellent statistical
// performance[1][2][3][4] with small and fast code, and small state size.[5]


#ifndef PCG_RNG_H
#define PCG_RNG_H

#include <stdint.h>


static uint32_t rotr32(uint32_t x, unsigned r);

//
uint32_t pcg32(void);

void pcg32_init(uint64_t seed);

#endif
