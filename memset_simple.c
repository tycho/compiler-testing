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

void *memset_simple(void *__restrict dst, int c, size_t size)
{

	uintptr_t *vdst;
	char *cdst;

	size_t i, lim;
	uintptr_t src;

	memset(&src, c, sizeof(src));

	lim = size / sizeof(uintptr_t);
	vdst = (uintptr_t *)dst;
#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
	for (i = 0; i < lim; i++)
		*vdst++ = src;

	lim = size & (sizeof(uintptr_t) - 1);
	cdst = (char *)vdst;
#ifdef __INTEL_COMPILER
#pragma ivdep
#endif
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
		memset_simple(mem, 0x1f * i, mem_size);
		assert(memcmp(mem, mem1, mem_size) == 0);
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	memset_simple(mem, 0x1f, mem_size);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	best = elapsedTime(&start, &finish);
	for (i = 0; i < lim; i++) {
		long t;
		clock_gettime(CLOCK_MONOTONIC, &start);
		memset_simple(mem, 0x1f * i, mem_size);
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
