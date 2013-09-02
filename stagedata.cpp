
#include <stdio.h>
#include <stdint.h>
#include "stagedata.h"

const char *backdrop_names[] = {
	"bk0",
	"bkBlue",
	"bkGreen",
	"bkBlack",
	"bkGard",
	"bkMaze",
	"bkGray",
	"bkRed",
	"bkWater",
#ifndef _RZX50
    "bkMoon",
    "bkFog",
#else
    "bkMoon480fix",
    "bkFog480fix",
#endif
    "bkFall",
	NULL
};

const char *tileset_names[] = {
	"0",
	"Pens",
	"Eggs",
	"EggX",
	"EggIn",
	"Store",
	"Weed",
	"Barr",
	"Maze",
	"Sand",
	"Mimi",
	"Cave",
	"River",
	"Gard",
	"Almond",
	"Oside",
	"Cent",
	"Jail",
	"White",
	"Fall",
	"Hell",
	"Labo",
	NULL
};

