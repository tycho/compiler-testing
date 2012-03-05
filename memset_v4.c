#include <memory.h>
#include <stdint.h>
#include <stdlib.h>
#ifdef TEST
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#endif

#ifdef __ARMCC_VERSION
#include <arm_neon.h>
typedef uint32x4_t v4;
#else
typedef uint32_t v4 __attribute__ ((vector_size(16), aligned(1)));
#endif

void *memset_v4(void *__restrict dst, int c, size_t size)
{
	v4 *vdst;
	v4 src;
	char *cdst;

	size_t i, j, lim;

	/*
	 * Seems silly to use memset() here, but it actually does this without
	 * a function call.
	 *
	 * GCC and Clang are smart enough to know that since the sizeof src is
	 * available at compile time, they can just:
	 *
	 * - Fill 64-bit register with 0x101010101010101
	 * - Zero-extend 'c' into a 64-bit register.
	 * - Integer multiply the two
	 * - Move this to the 2 remaining 32-bit fields in the v4 vector.
	 *
	 */
	memset(&src, c, sizeof(src));

	lim = size >> 4;
	vdst = (v4 *)dst;
	i = lim / 4;
	for (j = 0; j < i; j++) {
		vdst[0] = src;
		vdst[1] = src;
		vdst[2] = src;
		vdst[3] = src;
		vdst += 4;
	}
	i = lim % 4;
	for (j = 0; j < i; j++)
		*vdst++ = src;

	lim = size & 15;
	cdst = (char *)vdst;
	for (i = 0; i < lim; i++)
		*cdst++ = (char)c; 

	return dst;
}

#ifdef TEST
static long elapsedTime(struct timespec* start, struct timespec* end)
{
	return ((long) end->tv_sec * 1000000 + end->tv_nsec / 1000) -
	       ((long) start->tv_sec * 1000000 + start->tv_nsec / 1000);
}

int main(int argc, char **argv)
{
	const size_t mem_size = 200 * 1024 * 1024;
	size_t i;
	char *mem, *mem1;

	size_t lim = 5;
	struct timespec start, finish;
	long best, *lp;

	mem = (char *)malloc(mem_size);
	mem1 = (char *)malloc(mem_size);

	for (i = 0; i < 3; i++) {
		/* Warm-up. */
		memset(mem1, 0x1f * i, mem_size);
		memset_v4(mem, 0x1f * i, mem_size);
		assert(memcmp(mem, mem1, mem_size) == 0);
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	memset_v4(mem, 0x1f, mem_size);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	best = elapsedTime(&start, &finish);
	for (i = 0; i < lim; i++) {
		long t;
		clock_gettime(CLOCK_MONOTONIC, &start);
		memset_v4(mem, 0x1f * i, mem_size);
		clock_gettime(CLOCK_MONOTONIC, &finish);
		t = elapsedTime(&start, &finish);
		if (t < best) best = t;
		printf("run %5lu: %10.2f MB/s\n",
			i + 1, ((double)(mem_size / 1024 / 1024))
			/ (t / 1000000.0));
	}
	printf("best time: %10.2f MB/s\n",
		(double)(mem_size / 1024 / 1024) / (best / 1000000.0));

	free(mem);
	free(mem1);

	return 0;
}
#endif
