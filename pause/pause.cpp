
#include "../nx.h"
#include "pause.fdh"
#include "../l10n_strings.h"

bool pause_init(int param)
{
	memset(lastinputs, 1, sizeof(lastinputs));
	return 0;
}

void pause_tick()
{
#ifdef _DINGUX
    ClearScreen(DK_BLUE);
    const char *strMenu = "Select:Quit / B:Resume / A:Reset";
    int cx = (SCREEN_WIDTH / 2) - (GetFontWidth(strMenu, 0) / 2) - 4;
    int cy = (SCREEN_HEIGHT / 2) - GetFontHeight();
    int f3wd = font_draw(cx, cy, "Select", 0);
    int f3wd1 = font_draw(f3wd + cx, cy, LC_PS_QUIT, 0, &bluefont);
    int f3wd2 = font_draw(f3wd1 + f3wd + cx,  cy, " / B", 0);
    int f3wd3 = font_draw(f3wd2 + f3wd1 + f3wd + cx, cy, LC_PS_RESUME, 0, &bluefont);
    int f3wd4 = font_draw(f3wd3 + f3wd2 + f3wd1 + f3wd + cx, cy, " / A", 0);
    font_draw(f3wd4 + f3wd3 + f3wd2 + f3wd1 + f3wd + cx, cy, LC_PS_RESET, 0, &bluefont);
#elif _MOTOMAGX
    ClearScreen(DK_BLUE);
    const char *strMenu = "Camera:Quit / Center:Resume / Vol+:Reset";
    int cx = (SCREEN_WIDTH / 2) - (GetFontWidth(strMenu, 0) / 2) - 4;
    int cy = (SCREEN_HEIGHT / 2) - GetFontHeight();
    int f3wd = font_draw(cx, cy, "Camera", 0);
    int f3wd1 = font_draw(f3wd + cx, cy, LC_PS_QUIT, 0, &bluefont);
    int f3wd2 = font_draw(f3wd1 + f3wd + cx,  cy, " / Center", 0);
    int f3wd3 = font_draw(f3wd2 + f3wd1 + f3wd + cx, cy, LC_PS_RESUME, 0, &bluefont);
    int f3wd4 = font_draw(f3wd3 + f3wd2 + f3wd1 + f3wd + cx, cy, " / Vol+", 0);
    font_draw(f3wd4 + f3wd3 + f3wd2 + f3wd1 + f3wd + cx, cy, LC_PS_RESET, 0, &bluefont);
#elif _MOTOEZX
    ClearScreen(DK_BLUE);
    const char *strMenu = "Camera:Quit / Vol-:Resume / Vol+:Reset";
    int cx = (SCREEN_WIDTH / 2) - (GetFontWidth(strMenu, 0) / 2) - 4;
    int cy = (SCREEN_HEIGHT / 2) - GetFontHeight();
    int f3wd = font_draw(cx, cy, "Camera", 0);
    int f3wd1 = font_draw(f3wd + cx, cy, LC_PS_QUIT, 0, &bluefont);
    int f3wd2 = font_draw(f3wd1 + f3wd + cx,  cy, " / Vol-", 0);
    int f3wd3 = font_draw(f3wd2 + f3wd1 + f3wd + cx, cy, LC_PS_RESUME, 0, &bluefont);
    int f3wd4 = font_draw(f3wd3 + f3wd2 + f3wd1 + f3wd + cx, cy, " / Vol+", 0);
    font_draw(f3wd4 + f3wd3 + f3wd2 + f3wd1 + f3wd + cx, cy, LC_PS_RESET, 0, &bluefont);
#elif defined(_L10N_CP1251) && !defined(_DINGUX) && !defined(_MOTOMAGX) && !defined(_MOTOEZX)
    ClearScreen(BLACK);
    const char *strMenu = "ESC:Quit / F1:Resume / F2:Reset";
    int cx = (SCREEN_WIDTH / 2) - (GetFontWidth(strMenu, 0) / 2) - 4;
    int cy = (SCREEN_HEIGHT / 2) - GetFontHeight();
    int f3wd = font_draw(cx, cy, "ESC", 0);
    int f3wd1 = font_draw(f3wd + cx, cy, LC_PS_QUIT, 0, &bluefont);
    int f3wd2 = font_draw(f3wd1 + f3wd + cx,  cy, " / F1", 0);
    int f3wd3 = font_draw(f3wd2 + f3wd1 + f3wd + cx, cy, LC_PS_RESUME, 0, &bluefont);
    int f3wd4 = font_draw(f3wd3 + f3wd2 + f3wd1 + f3wd + cx, cy, " / F2", 0);
    font_draw(f3wd4 + f3wd3 + f3wd2 + f3wd1 + f3wd + cx, cy, LC_PS_RESET, 0, &bluefont);
#else
    ClearScreen(BLACK);
    int cx = (SCREEN_WIDTH / 2) - (sprites[SPR_RESETPROMPT].w / 2);
    int cy = (SCREEN_HEIGHT / 2) - (sprites[SPR_RESETPROMPT].h / 2);
    int f3wd = 0;
    draw_sprite(cx, cy, SPR_RESETPROMPT);
#endif

#ifdef _DINGUX
    const char *str = "L:Options";
    cx = (SCREEN_WIDTH / 2) - (GetFontWidth(str, 0) / 2) - 4;
    cy = (SCREEN_HEIGHT - 8) - GetFontHeight();
    f3wd = font_draw(cx, cy, "L", 0);
#elif _MOTOMAGX
    const char *str = "#:Options";
    cx = (SCREEN_WIDTH / 2) - (GetFontWidth(str, 0) / 2) - 4;
    cy = (SCREEN_HEIGHT - 8) - GetFontHeight();
    f3wd = font_draw(cx, cy, "#", 0);
#elif _MOTOEZX
    const char *str = "Mod + Camera:Options";
    cx = (SCREEN_WIDTH / 2) - (GetFontWidth(str, 0) / 2) - 4;
    cy = (SCREEN_HEIGHT - 8) - GetFontHeight();
    f3wd = font_draw(cx, cy, "Mod + Camera", 0);
#else
    const char *str = "F3:Options";
    cx = (SCREEN_WIDTH / 2) - (GetFontWidth(str, 0) / 2) - 4;
    cy = (SCREEN_HEIGHT - 8) - GetFontHeight();
    f3wd = font_draw(cx, cy, "F3", 0);
#endif
    font_draw(cx + f3wd, cy, LC_PS_OPTIONS, 0, &bluefont);
	
#if defined (_DINGUX) || defined (_MOTOMAGX) || defined (_MOTOEZX)
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

