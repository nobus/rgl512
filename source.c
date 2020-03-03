#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#define SIZEX	128
#define	SIZEY	64
static char map[SIZEY][SIZEX];

#define MMAX	50
static int mx[MMAX];
static int my[MMAX];
static char mc[MMAX];
static int mhp[MMAX];

static void draw_region(int x1, int x2, int y1, int y2, char c)
{
	int x, y;

	for (y = y1; y < y2; y++)
	{
		for (x = x1; x < x2; x++)
		{
			map[y][x] = c;
		}
	}
}

static int rr(int v1, int v2, int r)
{
	return (r % (v1 - v2)) + v1;
}

static void addm(int x, int y, int hp, char c)
{
	int i;

	for (i = 0; i < MMAX; i++)
	{
		if (!mc[i])
		{
			mx[i] = x;
			my[i] = y;
			mhp[i] = hp;
			mc[i] = c;
			return;
		}
	}
}

static int getm(int x, int y)
{
	int i;
	for (i = 0; i < MMAX; i++)
	{
		if ((mx[i] == x) && (my[i] == y)) return i;
	}

	return -1;
}

static void rand_place(char c, int hp)
{
	do
	{
		int x = rand()&127;
		int y = rand()&63;

		if (map[y][x] == '.')
		{
			map[y][x] = c;
			addm(x, y, hp, c);
			return;
		}
	}
	while(1);
}

static void init_map(void)
{
	int i, x1, x2, y1, y2;

	draw_region(0, SIZEX - 1, 0, SIZEY, '#');

	for (i = 0; i < 200; i++)
	{
		int r = rand();
		x1 = rr(1, SIZEX-20,r);
		x2 = rr(x1+5, x1+19,r);
		y1 = rr(1, SIZEY-8,r);
		y2 = rr(y1+3, y1+7,r);

		draw_region(x1, x2, y1, y2, '.');
	}

	rand_place('@', 5);

	for (i = 0; i < 10; i++)
	{
		rand_place('m', 2);
	}
}

static void draw_screen(void)
{
	int y;

	clear();

	/* Dump map */
	for (y = 0; y < SIZEY; y++)
	{
		mvaddstr(y, 0, map[y]);
	}
}

static int sgn(int x)
{
	return (x>0)-(x<0);
}

int main(void)
{
	int c;
	int i = 0;
	srand(time(0));
	init_map();

	initscr();
	cbreak();

	while (1)
	{
		int nx = mx[i], ny = my[i];
		if (!i)
		{
			draw_screen();
			c = 0;

			while (!(c=getch()));

			if ((c == '1') || (c == '4') || (c == '7')) nx -= 1;
			if ((c == '3') || (c == '6') || (c == '9')) nx += 1;
			if ((c == '7') || (c == '8') || (c == '9')) ny -= 1;
			if ((c == '1') || (c == '2') || (c == '3')) ny += 1;

			if (map[ny][nx] == 'm')
			{
				int j = getm(nx, ny);

				mhp[j]--;

				if (!mhp[j])
				{
					map[ny][nx] = '.';
					mc[j] = 0;
				}
			}
		}

		if (mc[i] == 'm')
		{
			nx += sgn(mx[0]-nx);
			ny += sgn(my[0]-ny);

			if (map[ny][nx] == '@')
			{
				mhp[0]--;

				if (!mhp[0])
				{
					endwin();
					exit(0);
				}
			}
		}

		if (mc[i] && (map[ny][nx] == '.'))
		{
			map[my[i]][mx[i]] = '.';

			mx[i] = nx;
			my[i] = ny;

			map[ny][nx] = mc[i];
		}

		i++;

		if (i == MMAX) i = 0;
	}

	return(0);
}
