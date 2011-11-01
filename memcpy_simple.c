#include <stdint.h>
#include <stdlib.h>
#ifdef TEST
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#endif

void *memcpy_simple(void *__restrict dst, const void *__restrict src, size_t size)
{

	uintptr_t *vdst;
	const uintptr_t *vsrc;

	const char *csrc;
	char *cdst;

	size_t i, lim;

	lim = size / sizeof(uintptr_t);
	vdst = (uintptr_t *)dst;
	vsrc = (const uintptr_t *)src;
#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
	for (i = 0; i < lim; i++)
		*vdst++ = *vsrc++;

	lim = size & (sizeof(uintptr_t) - 1);
	csrc = (const char *)vsrc;
	cdst = (char *)vdst;
#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
	for (i = 0; i < lim; i++)
		*cdst++ = *csrc++;

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

	size_t lim = 25;
	struct timespec start, finish;
	long best, *lp;

	mem = (char *)malloc(mem_size);
	mem1 = (char *)malloc(mem_size);

	for (i = 0; i < lim; i++) {
		srandom(time(NULL));

		lp = (long *)mem;
		for (i = 0; i < (mem_size / sizeof(long)); i++)
#ifdef TEST_RANDOM
			*lp = random();
#else
			/* Floating-point NaN value from log(-1.0f) */
			*lp = 0x7fc00000;
#endif

		/* Warm-up. */
		memcpy_simple(mem1 + 1, mem, mem_size - 1);
		assert(memcmp(mem1 + 1, mem, mem_size - 1) == 0);
		memcpy_simple(mem, mem1, mem_size);
		assert(memcmp(mem, mem1, mem_size) == 0);
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	memcpy_simple(mem1, mem, mem_size);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	best = elapsedTime(&start, &finish);
	for (i = 0; i < lim; i++) {
		long t;
		clock_gettime(CLOCK_MONOTONIC, &start);
		memcpy_simple(mem1, mem, mem_size);
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
