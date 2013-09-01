
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "../graphics/safemode.h"
#include "extract.fdh"

using safemode::print;
using safemode::status;
using safemode::clearstatus;
using safemode::clear;
using safemode::moveto;
using safemode::run_until_key;
static const char *filename = "Doukutsu.exe";

static int extract_do(void)
{
FILE *fp;

	clear();
	moveto(SM_UPPER_THIRD);
	print("= Extracting Files =");
	
	fp = fileopen(filename, "rb");
	if (!fp)
	{
		moveto(SM_CENTER);
		print("cannot find executable %s", filename);
		print("");
		print("");
		print("Please put it and it's \"data\" directory");
		print("into the same folder as this program.");
		run_until_key();
		return 1;
	}
	
	if (extract_pxt(fp)) return 1;
	if (extract_files(fp)) return 1;
	if (extract_stages(fp)) return 1;
	//findfiles(fp);
	//exit(1);
	
	clearstatus();
	fclose(fp);
	return 0;
}


int extract_main()
{
int result;

	if (safemode::init())
	{
		staterr("failed to initialize safemode graphics");
		return 1;
	}
	
	result = introduction();
	#ifdef __SDLSHIM__
	if (result == SDLK_BTN1)
	#else
	if (result == SDLK_ESCAPE)
	#endif
	{
		stat("Breaking out");
		return 1;
	}
	
	result = extract_do();
	if (!result)
		conclusion();
	
	safemode::close();
	return result;
}


int introduction()
{
	clear();
	moveto(SM_UPPER_THIRD);
	
	print("I need to extract some game data");
	print("before I can start up for the first time.");
	print("");
	print("Before beginning, you should have the Aeon Genesis");
	print("English translation of version 1.0.0.6, and drop");
	print("Doukutsu.exe and it's \"data\" directory into the same");
	print("folder as the \"nx\" program you just ran.");
	print("");
	#ifdef __SDLSHIM__
	print("If you haven't done that yet, please press BTN1 now");
	#else
	print("If you haven't done that yet, please press ESCAPE now");
	#endif
	print("and come back in a moment. Otherwise, you can");
	print("press any other key to start the extraction.");
	
	return run_until_key();
}

int conclusion()
{
	moveto(SM_MIDUPPER_Y);
	print("Success!");
	print("");
	print("You can now remove the Doukutsu.exe file");
	print("if you like, as it isn't needed anymore.");
	print("Please leave the \"data\" directory though.");
	print("");
	print("Once you get to the title screen, you may want");
	print("to adjust your resolution by pressing F3.");
	print("");
	print("Press any key to begin");
	
	return run_until_key();
}

/*
void c------------------------------() {}
*/

//#define FINDFILES
#ifdef FINDFILES
static struct
{
	const char *filename;
	int headersize;
}
fileinfo[] =
{
	"endpic/credit01.bmp", 25,
	"endpic/credit02.bmp", 25,
	"endpic/credit03.bmp", 25,
	"endpic/credit04.bmp", 25,
	"endpic/credit05.bmp", 25,
	"endpic/credit06.bmp", 25,
	"endpic/credit07.bmp", 25,
	"endpic/credit08.bmp", 25,
	"endpic/credit09.bmp", 25,
	"endpic/credit10.bmp", 25,
	"endpic/credit11.bmp", 25,
	"endpic/credit12.bmp", 25,
	"endpic/credit14.bmp", 25,
	"endpic/credit15.bmp", 25,
	"endpic/credit16.bmp", 25,
	"endpic/credit17.bmp", 25,
	"endpic/credit18.bmp", 25,
	"endpic/pixel.bmp", 25,
	"wavetable.dat", 0,
	"org/access.org", 0,
	"org/balcony.org", 0,
	"org/balrog.org", 0,
	"org/breakdown.org", 0,
	"org/cemetary.org", 0,
	"org/charge.org", 0,
	"org/credits.org", 0,
	"org/egg.org", 0,
	"org/eyesofflame.org", 0,
	"org/fanfale1.org", 0,
	"org/fanfale2.org", 0,
	"org/fanfale3.org", 0,
	"org/gameover.org", 0,
	"org/geothermal.org", 0,
	"org/gestation.org", 0,
	"org/gravity.org", 0,
	"org/grasstown.org", 0,
	"org/hell.org", 0,
	"org/heroend.org", 0,
	"org/jenka1.org", 0,
	"org/jenka2.org", 0,
	"org/labyrinth.org", 0,
	"org/lastbattle.org", 0,
	"org/lastcave.org", 0,
	"org/meltdown2.org", 0,
	"org/oppression.org", 0,
	"org/oside.org", 0,
	"org/plant.org", 0,
	"org/pulse.org", 0,
	"org/quiet.org", 0,
	"org/run.org", 0,
	"org/safety.org", 0,
	"org/scorching.org", 0,
	"org/seal.org", 0,
	"org/theme.org", 0,
	"org/toroko.org", 0,
	"org/town.org", 0,
	"org/tyrant.org", 0,
	"org/waterway.org", 0,
	"org/white.org", 0,
	"org/zombie.org", 0,
	NULL
};

bool findfiles(FILE *exefp)
{
int i;
FILE *fpo;
int len;
uint32_t crc;

	fpo = fileopen("/tmp/files.dat", "wb");
	crc_init();
	
	for(i=0;fileinfo[i].filename;i++)
	{
		uint32_t offset = findfile(fileinfo[i].filename, exefp, fileinfo[i].headersize, \
								&len, &crc);
		
		if (offset == 0)
		{
			staterr("couldn't find file %s", fileinfo[i].filename);
			return 1;
		}
		
		const char *headertable = "NULL";
		if (fileinfo[i].headersize)
		{
			headertable = strstr(fileinfo[i].filename, "pixel") ? \
					"pixel_header" : "credit_header";
		}
		
		fprintf(fpo, "\t\"%s\", 0x%06x, %d, 0x%08x, %s,\n",
					fileinfo[i].filename,
					offset, len, crc, headertable);
	}
	
	fclose(fpo);
	return 0;
}


uint32_t findfile(const char *fname, FILE *exefp, int headersize, \
					int *len_out, uint32_t *crc_out)
{
FILE *fp;
uint8_t *buffer;
uint32_t hit = 0;
int len;

	fp = fileopen(fname, "rb");
	if (!fp)
	{
		staterr("can't open %s", fname);
		return 0;
	}
	
	len = filesize(fp);
	buffer = (uint8_t *)malloc(len);
	
	len -= headersize;
	fseek(fp, headersize, SEEK_SET);
	fread(buffer, len, 1, fp);
	
	*crc_out = crc_calc(buffer, len);
	*len_out = len;
	
	stat("searching for '%s'; %d bytes", fname, len);
	
	int match = 0;
	fseek(exefp, 0, SEEK_SET);
	while(!feof(exefp))
	{
		uint8_t ch = fgetc(exefp);
recheck: ;
		
		if (ch == buffer[match])
		{
			match++;
			if (match >= len)
			{
				hit = (ftell(exefp) - len);
				stat("hit at 0x%06x", hit);
				match = 0;
			}
		}
		else if (match)
		{
			match = 0;
			goto recheck;
		}
	}
	
	free(buffer);
	fclose(fp);
	return hit;
}
#endif

