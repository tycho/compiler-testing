#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint32_t v4 __attribute__ ((vector_size(16), aligned(1)));

void *memcpy_vector(void *__restrict dst, const void *__restrict src, size_t size)
{
	void *orig = dst;

	const char *csrc;
	char *cdst;

	size_t i, lim;

	lim = size >> 4;
	v4 *vdst = (v4 *)dst;
	const v4 *vsrc = (const v4 *)src;
	for (i = 0; i < lim; i++) {
		*vdst++ = *vsrc++;
	}

	lim = size & 15;
	csrc = (const char *)vsrc;
	cdst = (char *)vdst;
	for (i = 0; i < lim; i++) {
		*cdst++ = *csrc++;
	}
	return orig;
}

#ifdef TEST
int main(int argc, char **argv)
{
	char buf[64];
	char *foo = "thisisakindofsmallteststringbutitshouldwork";
	memset(buf, 0, sizeof(buf));
	memcpy_vector(buf, foo, strlen(foo));
	printf("%s\n", buf);
	return 0;
}
#endif
