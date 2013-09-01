
#ifndef _SLOPE_H
#define _SLOPE_H

// slope types
#define SLOPE_CEIL_FWD1		1
#define SLOPE_CEIL_FWD2		2
#define SLOPE_CEIL_BACK1	3
#define SLOPE_CEIL_BACK2	4

#define SLOPE_BACK1			5
#define SLOPE_BACK2			6
#define SLOPE_FWD1			7
#define SLOPE_FWD2			8

#define SLOPE_LAST			8

struct SlopeTable
{
	uint8_t table[TILE_W][TILE_H];
};

#endif
