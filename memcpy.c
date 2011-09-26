#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
