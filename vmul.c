#ifdef __ARMCC_VERSION
#include <arm_neon.h>
typedef uint32x4_t v4;
#else
typedef uint32_t v4 __attribute__ ((vector_size(16), aligned(1)));
#endif

void vmul(v4 *o, v4 *a, v4 *b)
{
	*o = *a * *b;
}

