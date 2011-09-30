#ifdef TEST
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#endif

#define for_x for (x = 0; x < w; x++)
#define for_y for (y = 0; y < h; y++)
#define for_xy for_x for_y

void evolve(void *u, int w, int h)
{
	int x, y;
	unsigned (*univ)[w] = u;
	unsigned new[h][w];

	for_y for_x {
		int n = 0, y1, x1;
		for (y1 = y - 1; y1 <= y + 1; y1++)
			for (x1 = x - 1; x1 <= x + 1; x1++)
				if (univ[(y1 + h) % h][(x1 + w) % w])
					n++;

		if (univ[y][x]) n--;
		new[y][x] = (n == 3 || (n == 2 && univ[y][x]));
	}
	for_y for_x univ[y][x] = new[y][x];
}

#ifdef TEST
void show(void *u, int w, int h)
{
	int x, y;
	int (*univ)[w] = u;
	printf("\033[H");
	for_y {
		for_x printf(univ[y][x] ? "\033[07m  \033[m" : "  ");
		printf("\033[E");
	}
	fflush(stdout);
}

void game(int w, int h)
{
	int x, y;
	unsigned univ[h][w];
	for_xy univ[y][x] = rand() < RAND_MAX / 10 ? 1 : 0;
	while (1) {
#ifndef BENCH
		show(univ, w, h);
#endif
		evolve(univ, w, h);
#ifndef BENCH
		usleep(200000);
#endif
	}
}

int main(int c, char **v)
{
	int w = 0, h = 0;
	if (c > 1) w = atoi(v[1]);
	if (c > 2) h = atoi(v[2]);
	if (w <= 0) w = 30;
	if (h <= 0) h = 30;
	game(w, h);
}
#endif
