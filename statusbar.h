
#ifndef _STATUSBAR_H
#define _STATUSBAR_H

struct PercentBar
{
	int displayed_value;
	int dectimer;
};

struct StatusBar
{
	int xpflashcount;
	int xpflashstate;
};

extern StatusBar statusbar;
void niku_draw(int value, bool force_white=false);

void stat_PrevWeapon(bool quiet=false);
void stat_NextWeapon(bool quiet=false);

#endif
