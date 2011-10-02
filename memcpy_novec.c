#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef TEST
#include <sys/time.h>
#include <time.h>
#endif

void *memcpy_vector(void *__restrict dst, const void *__restrict src, size_t size)
{
	void *orig = dst;

	uint64_t *vdst;
	const uint64_t *vsrc;

	const char *csrc;
	char *cdst;

	size_t i, lim;

	lim = size >> 3;
	vdst = (uint64_t *)dst;
	vsrc = (const uint64_t *)src;
	i = lim / 8;
	while (i-- > 0) {
		vdst[0] = vsrc[0];
		vdst[1] = vsrc[1];
		vdst[2] = vsrc[2];
		vdst[3] = vsrc[3];
		vdst[4] = vsrc[4];
		vdst[5] = vsrc[5];
		vdst[6] = vsrc[6];
		vdst[7] = vsrc[7];
		vdst += 8;
		vsrc += 8;
	}
	i = lim % 8;
	while (i-- > 0)
		*vdst++ = *vsrc++;

	lim = size & 7;
	csrc = (const char *)vsrc;
	cdst = (char *)vdst;
	i = lim;
	while (i-- > 0)
		*cdst++ = *csrc++;
	return orig;
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
		memcpy_vector(mem1 + 1, mem, mem_size - 1);
		assert(memcmp(mem1 + 1, mem, mem_size - 1) == 0);
		memcpy_vector(mem, mem1, mem_size);
		assert(memcmp(mem, mem1, mem_size) == 0);
	}

	clock_gettime(CLOCK_MONOTONIC, &start);
	memcpy_vector(mem1, mem, mem_size);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	best = elapsedTime(&start, &finish);
	for (i = 0; i < lim; i++) {
		long t;
		clock_gettime(CLOCK_MONOTONIC, &start);
		memcpy_vector(mem1, mem, mem_size);
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
