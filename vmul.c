typedef int v4 __attribute__ ((vector_size(16)));

void vmul(v4 *o, v4 *a, v4 *b)
{
	*o = *a * *b;
}

