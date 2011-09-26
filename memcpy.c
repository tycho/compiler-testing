#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifdef TEST
#include <sys/time.h>
#include <time.h>
#endif

typedef uint32_t v4 __attribute__ ((vector_size(16), aligned(1)));

void *memcpy_vector(void *__restrict dst, const void *__restrict src, size_t size)
{
	void *orig = dst;

	v4 *vdst;
	const v4 *vsrc;

	const char *csrc;
	char *cdst;

	size_t i, lim;

	lim = size >> 4;
	vdst = (v4 *)dst;
	vsrc = (const v4 *)src;
	i = lim / 4;
	while (i-- > 0) {
		vdst[0] = vsrc[0];
		vdst[1] = vsrc[1];
		vdst[2] = vsrc[2];
		vdst[3] = vsrc[3];
		vdst += 4;
		vsrc += 4;
	}
	i = lim % 4;
	while (i-- > 0)
		*vdst++ = *vsrc++;

	lim = size & 15;
	csrc = (const char *)vsrc;
	cdst = (char *)vdst;
	i = lim;
	while (i-- > 0)
		*cdst++ = *csrc++;
	return orig;
}

#ifdef TEST
static long elapsedTime(struct timeval* start, struct timeval* end)
{
	return ((long) end->tv_sec * 1000000 + end->tv_usec) -
	       ((long) start->tv_sec * 1000000 + start->tv_usec);
}

int main(int argc, char **argv)
{
	const size_t mem_size = 200 * 1024 * 1024;
	size_t i;
	char *mem, *mem1;

	size_t lim = 25;
	struct timeval start, finish;
	long best;

	mem = (char *)valloc(mem_size);
	mem1 = (char *)valloc(mem_size);

	/* Warm-up. */
	memcpy(mem1, mem, mem_size);
	memcpy(mem, mem1, mem_size);

	/* Test each */
	gettimeofday(&start, NULL);
	memcpy_vector(mem1, mem, mem_size);
	gettimeofday(&finish, NULL);
	best = elapsedTime(&start, &finish);
	for (i = 0; i < lim; i++) {
		long t;
		gettimeofday(&start, NULL);
		memcpy_vector(mem1, mem, mem_size);
		gettimeofday(&finish, NULL);
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
