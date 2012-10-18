// Type your code here, or load an example.
#include <stdint.h>

uint8_t popcnt(uint32_t n)
{
	uint8_t c = 0;
	while (n) {
		c += (n & 1);
		n >>= 1;
	}
	return c;
}
