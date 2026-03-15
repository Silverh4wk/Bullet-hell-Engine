
// A permuted congruential generator (PCG) is a pseudorandom number generation
// algorithm developed in 2014 by Dr. M.E. O'Neill which applies an output
// permutation function to improve the statistical properties of a modulo-2n
// linear congruential generator (LCG). It achieves excellent statistical
// performance[1][2][3][4] with small and fast code, and small state size.[5]

//https://en.wikipedia.org/wiki/Permuted_congruential_generator

#include "PCG_RNG.h"

static uint64_t       state      = 0x4d595df4d0f33173;		// Or something seed-dependent
static uint64_t const multiplier = 6364136223846793005u;
static uint64_t const increment  = 1442695040888963407u;	// Or an arbitrary odd constant

// bitwise rotation to the right on a 32-bit integer.
static uint32_t rotr32(uint32_t x, unsigned r)
{
	return x >> r | x << (-r & 31);
}

//returns the next random number genarated
uint32_t pcg32(void)
{
	uint64_t x = state;
	unsigned count = (unsigned)(x >> 59);		// 59 = 64 - 5

	state = x * multiplier + increment;
	x ^= x >> 18;								// 18 = (64 - 27)/2
	return rotr32((uint32_t)(x >> 27), count);	// 27 = 32 - 5
}

//Init the RNG with a seed
void pcg32_init(uint64_t seed)
{
	state = seed + increment;
	(void)pcg32();
}
