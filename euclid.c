int euclid(int i, int j)
{
	while(i != j) {
	   if (i > j)
		   i -= j;
	   else
		   j -= i;
	}
	return i;
}
