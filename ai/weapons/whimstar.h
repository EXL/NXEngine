
#ifndef _WHIMSTAR_H
#define _WHIMSTAR_H


#define MAX_WHIMSTARS			3

struct Whimstar
{
	int x, y;
	int xinertia, yinertia;
};

struct WhimsicalStar
{
	Whimstar stars[MAX_WHIMSTARS];
	int nstars;
	int stariter;
};



#endif
