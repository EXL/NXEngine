
#include "../nx.h"
#include "pause.fdh"

bool pause_init(int param)
{
	memset(lastinputs, 1, sizeof(lastinputs));
	return 0;
}

void pause_tick()
{
    ClearScreen(BLACK);

#ifndef _RZX50
    int cx = (SCREEN_WIDTH / 2) - (sprites[SPR_RESETPROMPT].w / 2);
    int cy = (SCREEN_HEIGHT / 2) - (sprites[SPR_RESETPROMPT].h / 2);
    draw_sprite(cx, cy, SPR_RESETPROMPT);
#else
    const char *strMenu = "Select:Quit / A:Resume / B:Reset";
    int cx = (SCREEN_WIDTH / 2) - (GetFontWidth(strMenu, 0) / 2) - 4;
    int cy = (SCREEN_HEIGHT / 2) - GetFontHeight();
    int f3wd = font_draw(cx, cy, "Select", 0);
    int f3wd1 = font_draw(f3wd + cx, cy, ":Quit", 0, &bluefont);
    int f3wd2 = font_draw(f3wd1 + f3wd + cx,  cy, " / A", 0);
    int f3wd3 = font_draw(f3wd2 + f3wd1 + f3wd + cx, cy, ":Resume", 0, &bluefont);
    int f3wd4 = font_draw(f3wd3 + f3wd2 + f3wd1 + f3wd + cx, cy, " / B", 0);
    font_draw(f3wd4 + f3wd3 + f3wd2 + f3wd1 + f3wd + cx, cy, ":Reset", 0, &bluefont);
#endif

#ifndef _RZX50
    const char *str = "F3:Options";
    cx = (SCREEN_WIDTH / 2) - (GetFontWidth(str, 0) / 2) - 4;
    cy = (SCREEN_HEIGHT - 8) - GetFontHeight();
    f3wd = font_draw(cx, cy, "F3", 0);
#else
    const char *str = "X:Options";
    cx = (SCREEN_WIDTH / 2) - (GetFontWidth(str, 0) / 2) - 4;
    cy = (SCREEN_HEIGHT - 8) - GetFontHeight();
    f3wd = font_draw(cx, cy, "X", 0);
#endif
    font_draw(cx + f3wd, cy, ":Options", 0, &bluefont);
	
#ifdef _RZX50
#define F1KEY FIREKEY
#define F2KEY JUMPKEY
#endif

	// resume
	if (justpushed(F1KEY))
	{
		lastinputs[F1KEY] = true;
		game.pause(false);
		return;
	}
	
	// reset
	if (justpushed(F2KEY))
	{
		lastinputs[F2KEY] = true;
		game.reset();
		return;
	}
	
	// exit
	if (justpushed(ESCKEY))
	{
		lastinputs[ESCKEY] = true;
		game.running = false;
		return;
	}
}






