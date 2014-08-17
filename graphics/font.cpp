
#include "../config.h"
#include <SDL/SDL.h>

#ifdef CONFIG_ENABLE_TTF
	#include <SDL/SDL_ttf.h>
#endif

#include "../nx.h"
#include "font.h"
#include "font.fdh"

static int text_draw(int x, int y, const char *text, int spacing=0, NXFont *font=&whitefont);

#define SHADOW_OFFSET		2		// distance of drop shadows

#define BITMAP_CHAR_WIDTH	5		// width of each char
#define BITMAP_CHAR_HEIGHT	9		// height of each char
#define BITMAP_SPAC_WIDTH	8		// how far apart chars are from each other on the sheet
#define BITMAP_SPAC_HEIGHT	12		// ditto for Y-spacing

// For convert cp1251 to utf-8
int wtable[64] = {
  0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
  0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
  0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
  0x007F, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
  0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
  0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
  0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
  0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457
};

static const char bitmap_map[] = {		// letter order of bitmap font sheet
	" !\"#$%&`()*+,-./0123456789:;<=>?"
	"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]%_"
	"'abcdefghijklmnopqrstuvwxyz{|}~"
};

const char *bmpfontfile = "smalfont.bmp";

#if defined(_320X240) || defined(_480X272)
const char *ttffontfile = "DroidSansMono.ttf";
#else
const char *ttffontfile = "font.ttf";
#endif

static SDL_Surface *sdl_screen = NULL;
static SDL_Surface *shadesfc = NULL;

static bool initilized = false;
static bool rendering = true;
static bool shrink_spaces = true;
static int fontheight = 0;

NXFont whitefont;
NXFont greenfont;
NXFont bluefont;		// used for "F3:Options" text on pause screen
NXFont shadowfont;		// white letters w/ drop shadow

// point sizes for each valid scaling factor
int pointsize[] = { -1,  10, 17, 26 };

/*
void c------------------------------() {}
*/

bool font_init(void)
{
bool error = false;

	// we'll be bypassing the NXSurface automatic scaling features
	// and drawing at the real resolution so we can get better-looking fonts.
	sdl_screen = screen->GetSDLSurface();

	// at 320x240 switch to bitmap fonts for better appearance
#if (defined(_320X240) || defined(_480X272)) && defined(_L10N_CP1251)
    if (false)
#elif defined(CONFIG_ENABLE_TTF)
    if (SCALE == 1)
#endif
	{
		stat("fonts: using bitmapped from %s", bmpfontfile);

		SDL_Surface *sheet = SDL_LoadBMP(bmpfontfile);
		if (!sheet)
		{
			staterr("Couldn't open bitmap font file: '%s'", bmpfontfile);
			return 1;
		}

		uint32_t fgindex = SDL_MapRGB(sheet->format, 255, 255, 255);

		error |= whitefont.InitBitmapChars(sheet, fgindex, 0xffffff);
		error |= greenfont.InitBitmapChars(sheet, fgindex, 0x00ff80);
		error |= bluefont.InitBitmapChars(sheet, fgindex, 0xa0b5de);
		error |= shadowfont.InitBitmapCharsShadowed(sheet, fgindex, 0xffffff, 0x000000);
	}
    #ifdef CONFIG_ENABLE_TTF
	else
	{
		stat("fonts: using truetype at %dpt", pointsize[SCALE]);

		// initilize normal TTF fonts
		if (TTF_Init() < 0)
		{
			staterr("Couldn't initialize SDL_ttf: %s", TTF_GetError());
			return 1;
		}

        TTF_Font *font = TTF_OpenFont(ttffontfile, pointsize[SCALE]);
		if (!font)
		{
			staterr("Couldn't open font: '%s'", ttffontfile);
			return 1;
		}

		error |= whitefont.InitChars(font, 0xffffff);
		error |= greenfont.InitChars(font, 0x00ff80);
		error |= bluefont.InitChars(font, 0xa0b5de);
		error |= shadowfont.InitCharsShadowed(font, 0xffffff, 0x000000);

		TTF_CloseFont(font);
	}
    #endif

	error |= create_shade_sfc();
	if (error) return 1;

	fontheight = (whitefont.letters['M']->h / SCALE);
	initilized = true;
	return 0;
}

void font_close(void)
{

}

bool font_reload()
{
	if (!initilized) return 0;

	whitefont.free();
	greenfont.free();
	bluefont.free();
	shadowfont.free();

	return font_init();
}

/*
void c------------------------------() {}
*/

NXFont::NXFont()
{
	memset(letters, 0, sizeof(letters));
}

NXFont::~NXFont()
{
	free();
}

void NXFont::free()
{
	for(int i=0;i<NUM_LETTERS_RENDERED;i++)
	{
		if (letters[i]) SDL_FreeSurface(letters[i]);
		letters[i] = NULL;
	}
}

/*
void c------------------------------() {}
*/

#ifdef CONFIG_ENABLE_TTF
bool NXFont::InitChars(TTF_Font *font, uint32_t color)
{
SDL_Color fgcolor;
SDL_Surface *letter;

	fgcolor.r = (uint8_t)(color >> 16);
	fgcolor.g = (uint8_t)(color >> 8);
	fgcolor.b = (uint8_t)(color);

#ifdef _L10N_CP1251
    char utf8_str[2];
#endif

	char str[2];
	str[1] = 0;

	for(int i=1;i<NUM_LETTERS_RENDERED;i++)
	{
		str[0] = i;
#ifndef _L10N_CP1251
        letter = TTF_RenderUTF8_Solid(font, str, fgcolor);
#else
        win1251_to_utf8(str, utf8_str);
        letter = TTF_RenderUTF8_Solid(font, utf8_str, fgcolor);
#endif
		if (!letter)
		{
			staterr("InitChars: failed to render character %d: %s", i, TTF_GetError());
			return 1;
		}

		letters[i] = SDL_DisplayFormat(letter);
		SDL_FreeSurface(letter);
	}

	return 0;
}

// create a font with a drop-shadow (used for "MNA" stage-name displays)
bool NXFont::InitCharsShadowed(TTF_Font *font, uint32_t color, uint32_t shadowcolor)
{
SDL_Color fgcolor, shcolor;
SDL_Surface *top, *bottom;
SDL_Rect dstrect;

	fgcolor.r = (uint8_t)(color >> 16);
	fgcolor.g = (uint8_t)(color >> 8);
	fgcolor.b = (uint8_t)(color);

	shcolor.r = (uint8_t)(shadowcolor >> 16);
	shcolor.g = (uint8_t)(shadowcolor >> 8);
	shcolor.b = (uint8_t)(shadowcolor);

#ifdef _L10N_CP1251
    char utf8_str[2];
#endif

	char str[2];
	str[1] = 0;

	SDL_PixelFormat *format = sdl_screen->format;
	uint32_t transp = SDL_MapRGB(format, 255, 0, 255);

	for(int i=1;i<NUM_LETTERS_RENDERED;i++)
	{
		str[0] = i;

#ifndef _L10N_CP1251
        top = TTF_RenderUTF8_Solid(font, str, fgcolor);
        bottom = TTF_RenderUTF8_Solid(font, str, shcolor);
#else
        win1251_to_utf8(str, utf8_str);
        top = TTF_RenderUTF8_Solid(font, utf8_str, fgcolor);
        bottom = TTF_RenderUTF8_Solid(font, utf8_str, shcolor);
#endif
		if (!top || !bottom)
		{
			staterr("InitCharsShadowed: failed to render character %d: %s", i, TTF_GetError());
			return 1;
		}

		letters[i] = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, top->w, top->h+SHADOW_OFFSET,
							format->BitsPerPixel, format->Rmask, format->Gmask,
							format->Bmask, format->Amask);
		if (!letters[i])
		{
			staterr("InitCharsShadowed: failed to create surface for character %d: %s", i, SDL_GetError());
			return 1;
		}

		SDL_FillRect(letters[i], NULL, transp);
		SDL_SetColorKey(letters[i], SDL_SRCCOLORKEY, transp);

		dstrect.x = 0;
		dstrect.y = SHADOW_OFFSET;
		SDL_BlitSurface(bottom, NULL, letters[i], &dstrect);

		dstrect.x = 0;
		dstrect.y = 0;
		SDL_BlitSurface(top, NULL, letters[i], &dstrect);
	}

    return 0;
}

/* Universal way to convert Windows-1251 to UTF-8 by Den Zurin
 * (c) http://www.gamedev.ru/code/forum/?id=119312&page=2#m17 */

int NXFont::win1251_to_utf8(const char *text, char *utext)
{
    int wc;
    int i, j;
    if (!utext) {
        return 0;
    }
    i=0;
    j=0;
    while (i<strlen(text)) {
        wc = (unsigned char)text[i++];
        // Windows-1251 to Unicode
        if (wc>=0x80) {
            if (wc<=0xBF) {
                wc = wtable[wc-0x80];
            }
            else if (wc>=0xC0 && wc<=0xDF) {
                wc = wc - 0xC0 + 0x0410;
            }
            else if (wc>=0xE0) {
                wc = wc - 0xE0 + 0x0430;
            }
        }
        // Unicode to UTF-8
        // 0x00000000 — 0x0000007F -> 0xxxxxxx
        // 0x00000080 — 0x000007FF -> 110xxxxx 10xxxxxx
        // 0x00000800 — 0x0000FFFF -> 1110xxxx 10xxxxxx 10xxxxxx
        // 0x00010000 — 0x001FFFFF -> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        if (wc<0x80) {
            utext[j++] = (char)wc;
        } else if (wc<0x800) {
            utext[j++] = (char)((wc>>6)   | 0xC0);
            utext[j++] = (char)((wc&0x3F) | 0x80);
        } else if (wc<0x10000) {
            utext[j++] = (char)((wc>>12)  | 0xE0);
            utext[j++] = (char)((wc>>6)   | 0x80);
            utext[j++] = (char)((wc&0x3F) | 0x80);
        } else {
            utext[j++] = (char)((wc>>18)  | 0xF0);
            utext[j++] = (char)((wc>>12)  | 0x80);
            utext[j++] = (char)((wc>>6)   | 0x80);
            utext[j++] = (char)((wc&0x3F) | 0x80);
        }
    }
    utext[j] = 0x00;
    return 1;
}

#endif		//CONFIG_ENABLE_TTF

/*
void c------------------------------() {}
*/

// Create a font from a bitmapped font sheet.
// sheet: a 8bpp (paletted) sheet to create the font from.
// fgindex: color index of foreground color of letters.
// color: the color you want the letters to be.
bool NXFont::InitBitmapChars(SDL_Surface *sheet, uint32_t fgcolor, uint32_t color)
{
SDL_PixelFormat *format = sdl_screen->format;
SDL_Rect srcrect, dstrect;
SDL_Surface *letter;
int x, y, i;

	// NULL out letters we don't have a character for
	memset(this->letters, 0, sizeof(this->letters));

	// change the color of the letters by messing with the palette on the sheet
	ReplaceColor(sheet, fgcolor, color);

	// start at the top of the letter sheet.
	x = 0;
	y = 0;
	for(i=0;bitmap_map[i];i++)
	{
		uint8_t ch = bitmap_map[i];
		//stat("copying letter %d: '%c' from [%d,%d]", i, ch, x, y);

		// make character surface one pixel larger than the actual char so that there
		// is some space between letters in autospaced text such as on the menus.
		letter = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, \
							BITMAP_CHAR_WIDTH+1, BITMAP_CHAR_HEIGHT+1,
							format->BitsPerPixel, \
							format->Rmask, format->Gmask,
							format->Bmask, format->Amask);
		if (!letter)
		{
			staterr("InitBitmapChars: failed to create surface for character %d/%d", i, ch);
			return 1;
		}

		SDL_FillRect(letter, NULL, SDL_MapRGB(format, 0, 0, 0));

		// copy letter off of sheet
		srcrect.x = x;
		srcrect.y = y;
		srcrect.w = BITMAP_CHAR_WIDTH;
		srcrect.h = BITMAP_CHAR_HEIGHT;

		dstrect.x = 0;
		dstrect.y = 0;

		SDL_BlitSurface(sheet, &srcrect, letter, &dstrect);

		// make background transparent and copy into final position
		SDL_SetColorKey(letter, SDL_SRCCOLORKEY, SDL_MapRGB(format, 0, 0, 0));

		letters[ch] = SDL_DisplayFormat(letter);
		SDL_FreeSurface(letter);

		// advance to next position on sheet
		x += BITMAP_SPAC_WIDTH;
		if (x >= sheet->w)
		{
			x = 0;
			y += BITMAP_SPAC_HEIGHT;
		}
	}

	return 0;
}

// create a bitmapped font with a drop-shadow.
bool NXFont::InitBitmapCharsShadowed(SDL_Surface *sheet, uint32_t fgcolor, \
									uint32_t color, uint32_t shadowcolor)
{
SDL_PixelFormat *format = sdl_screen->format;
NXFont fgfont, shadowfont;
SDL_Rect dstrect;

	// create temporary fonts in the fg and shadow color
	if (fgfont.InitBitmapChars(sheet, fgcolor, color))
		return 1;

	if (shadowfont.InitBitmapChars(sheet, fgcolor, shadowcolor))
		return 1;

	// now combine the two fonts
	uint32_t transp = SDL_MapRGB(format, 0, 0, 0);
	for(int i=0;i<NUM_FONT_LETTERS;i++)
	{
		if (fgfont.letters[i])
		{
			letters[i] = SDL_CreateRGBSurface(SDL_SRCCOLORKEY, \
							BITMAP_CHAR_WIDTH+1, BITMAP_CHAR_HEIGHT+1+SHADOW_OFFSET,
							format->BitsPerPixel, \
							format->Rmask, format->Gmask,
							format->Bmask, format->Amask);

			SDL_FillRect(letters[i], NULL, transp);
			SDL_SetColorKey(letters[i], SDL_SRCCOLORKEY, transp);

			dstrect.x = 0;
			dstrect.y = SHADOW_OFFSET;
			SDL_BlitSurface(shadowfont.letters[i], NULL, letters[i], &dstrect);

			dstrect.x = 0;
			dstrect.y = 0;
			SDL_BlitSurface(fgfont.letters[i], NULL, letters[i], &dstrect);
		}
	}

	return 0;
}


// if sfc is an 8bpp paletted surface, change color index 'oldcolor' to be newcolor.
// if sfc is a 16bpp surface, replace all instances of color 'oldcolor' with 'newcolor'
void NXFont::ReplaceColor(SDL_Surface *sfc, uint32_t oldcolor, uint32_t newcolor)
{
	#ifndef __SDLSHIM__
	if (sfc->format->BitsPerPixel == 8)
	{
		SDL_Color desired;

		desired.r = (newcolor >> 16) & 0xff;
		desired.g = (newcolor >> 8) & 0xff;
		desired.b = (newcolor & 0xff);

		SDL_SetColors(sfc, &desired, oldcolor, 1);
	}
	else
	#endif
	{
		uint16_t *pixels = (uint16_t *)sfc->pixels;
		int npixels = (sfc->w * sfc->h);

		for(int i=0;i<npixels;i++)
		{
			if (pixels[i] == oldcolor)
				pixels[i] = newcolor;
		}
	}
}

/*
void c------------------------------() {}
*/

#ifdef __SDLSHIM__
void direct_text_draw(int x, int y, const char *text)
{
SDL_Rect dstrect;

	for(int i=0;text[i];i++)
	{
		SDL_Surface *letter = whitefont.letters[text[i]];

		if (letter)
		{
			dstrect.x = x;
			dstrect.y = y;
			SDL_BlitSurface(letter, NULL, SDLS_VRAMSurface, &dstrect);
		}

		x += 8;
	}
}
#endif


// draw a text string
static int text_draw(int x, int y, const char *text, int spacing, NXFont *font)
{
int orgx = x;
int i;
SDL_Rect dstrect;

	for(i=0;text[i];i++)
	{
		uint8_t ch = text[i];
		SDL_Surface *letter = font->letters[ch];

		if (ch == '=' && game.mode != GM_CREDITS)
		{
			if (rendering)
			{
				int yadj = (SCALE==1) ? 1:2;
				draw_sprite((x/SCALE), (y/SCALE)+yadj, SPR_TEXTBULLET);
			}
		}
		else if (rendering && ch != ' ' && letter)
		{
			// must set this every time, because SDL_BlitSurface overwrites
			// dstrect with final clipping rectangle.
			dstrect.x = x;
			dstrect.y = y;
			SDL_BlitSurface(letter, NULL, sdl_screen, &dstrect);
		}

		if (spacing != 0)
		{	// fixed spacing
			x += spacing;
		}
		else
		{	// variable spacing
			if (ch == ' ' && shrink_spaces)
			{	// 10.5 px for spaces - make smaller than they really are - the default
				x += (SCALE == 1) ? 5 : 10;
				if (i & 1) x++;
			}
			else
			{
				if (letter)
					x += letter->w;
			}
		}
	}

	// return the final width of the text drawn
    return (x - orgx);
}


int GetFontWidth(const char *text, int spacing, bool is_shaded)
{
int wd;

	if (spacing)
		return (strlen(text) * spacing);

	rendering = false;
	shrink_spaces = !is_shaded;

	wd = text_draw(0, 0, text, spacing * SCALE);

	rendering = true;
	shrink_spaces = true;

	return (wd / SCALE);
}

int GetFontHeight()
{
	return fontheight;
}

/*
void c------------------------------() {}
*/

// create the shadesfc, used by font_draw_shaded. It's just a big long black surface
// with 50% per-surface alpha applied, that we can use to darken the background.
static bool create_shade_sfc(void)
{
	if (shadesfc)
		SDL_FreeSurface(shadesfc);

	int wd = (SCREEN_WIDTH * SCALE);
	int ht = whitefont.letters['M']->h;

	SDL_PixelFormat *format = sdl_screen->format;
	shadesfc = SDL_CreateRGBSurface(SDL_SRCALPHA | SDL_HWSURFACE, wd, ht,
							format->BitsPerPixel, format->Rmask, format->Gmask,
							format->Bmask, format->Amask);

	if (!shadesfc)
	{
		staterr("create_shade_sfc: failed to create surface");
		return 1;
	}

	SDL_FillRect(shadesfc, NULL, SDL_MapRGB(format, 0, 0, 0));
	SDL_SetAlpha(shadesfc, SDL_SRCALPHA, 128);

	return 0;
}


int font_draw(int x, int y, const char *text, int spacing, NXFont *font)
{
	x *= SCALE;
	y *= SCALE;
	spacing *= SCALE;

	return (text_draw(x, y, text, spacing, font) / SCALE);
}

// draw a text string with a 50% dark border around it
int font_draw_shaded(int x, int y, const char *text, int spacing, NXFont *font)
{
SDL_Rect srcrect, dstrect;
int wd;

	x *= SCALE;
	y *= SCALE;
	spacing *= SCALE;

	// get full-res width of final text
	rendering = false;
	shrink_spaces = false;

	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.h = shadesfc->h;
	srcrect.w = text_draw(0, 0, text, spacing, font);

	rendering = true;

	// shade
	dstrect.x = x;
	dstrect.y = y;
	SDL_BlitSurface(shadesfc, &srcrect, sdl_screen, &dstrect);

	// draw the text on top as normal
	wd = text_draw(x, y, text, spacing, font);

	shrink_spaces = true;
	return (wd / SCALE);
}





