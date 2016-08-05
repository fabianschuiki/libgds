/* Copyright (c) 2016 Fabian Schuiki */
#include "common.h"
#include <math.h>

/**
 * @file
 * @author Fabian Schuiki <fschuiki@student.ethz.ch>
 *
 * This file implements conversion functions from and to the GDS-specific 32 and
 * 64 bit real values.
 */


gds_real64_t
gds_double_to_real64(double v) {
	if (v == 0)
		return 0;

	// Unpack the double.
	uint64_t raw = *(uint64_t*)&v;
	uint64_t vs = (raw >> 63) & 0x1;
	uint64_t ve = (raw >> 52) & 0x7FF;
	uint64_t vm = (1ul << 52) | (raw & 0x000FFFFFFFFFFFFF);

	// Adjust the exponent.
	unsigned shift = (ve+1) % 4;
	int16_t exp = ((int16_t)ve - 1023);
	vm <<= shift;
	exp += 4; // since the mantissa in real64 is 4 bits longer than in double
	exp -= shift;
	assert((exp % 4) == 0);
	assert(vm < 0x00FFFFFFFFFFFFFF);
	ve = (uint64_t)(exp / 4 + 64) & 0x7F;

	// Pack as real64.
	return (vs << 63) | (ve << 56) | (vm & 0x00FFFFFFFFFFFFFF);
}


gds_real32_t
gds_double_to_real32(double v) {
	assert(0 && "not implemented");
	return 0;
}


double
gds_real64_to_double(gds_real64_t v) {
	if (v == 0)
		return 0;

	// Unpack the real64.
	uint64_t vs = (v >> 63) & 0x1;
	uint64_t ve = (v >> 56) & 0x7F;
	uint64_t vm = v & 0x00FFFFFFFFFFFFFF;

	// Adjust the exponent.
	int16_t exp = ((int16_t)ve - 64)*4;
	exp -= 4;
	for (; (vm >> 52) > 1; ++exp, vm >>= 1);
	ve = (uint64_t)(exp + 1023) & 0x7FF;

	// Pack as double.
	uint64_t r = (vs << 63) | (ve << 52) | (vm & 0x000FFFFFFFFFFFFF);
	return *(double*)&r;
}


double
gds_real32_to_float(gds_real32_t v) {
	assert(0 && "not implemented");
	return 0;
}
