#ifdef __ARMCC_VERSION
#include <arm_neon.h>
typedef uint32x4_t  v4i;
typedef float32x4_t v4f;
#else
#include <stdint.h>
typedef uint32_t v4i __attribute__ ((vector_size(16), aligned(1)));
typedef float    v4f __attribute__ ((vector_size(16), aligned(1)));
#endif

void vmul4i(v4i *o, v4i *a, v4i *b)
{
	*o = *a * *b;
}

void vmul4f(v4f *o, v4f *a, v4f *b)
{
	*o = *a * *b;
}

