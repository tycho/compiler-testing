#include <stdint.h>
uint8_t
popcount(int64_t x)
{
	uint8_t ret = 0;
	while ( x ) {
		x &= x-1; // clear one set bit
		ret += 1;
	}
	return ret;
}

