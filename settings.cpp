
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "settings.h"
#include "replay.h"
#include "settings.fdh"
#include "nx.h"

const char *setfilename = "settings.dat";
const uint16_t SETTINGS_VERSION = 0x1602;		// serves as both a version and magic

Settings normal_settings;
Settings replay_settings;
Settings *settings = &normal_settings;


bool settings_load(Settings *setfile)
{
	if (!setfile) setfile = &normal_settings;
	
	if (tryload(settings))
	{
		stat("No saved settings; using defaults.");
		
		memset(setfile, 0, sizeof(Settings));
#if defined (_480X272) || defined (_320X240)
        setfile->resolution = 0;		// 640x480 Windowed, should be safe value
#else
        setfile->resolution = 2;
#endif
		setfile->last_save_slot = 0;
		setfile->multisave = true;
		
		setfile->enable_debug_keys = false;
		setfile->sound_enabled = true;
        setfile->music_enabled = 1;	// both Boss and Regular music
		
		setfile->instant_quit = false;
		setfile->emulate_bugs = false;
		setfile->no_quake_in_hell = false;
		setfile->inhibit_fullscreen = false;
#ifndef __HAIKU__
		setfile->files_extracted = false;
#else
		setfile->files_extracted = true;
#endif
		
		// I found that 8bpp->32bpp blits are actually noticably faster
		// than 32bpp->32bpp blits on several systems I tested. Not sure why
		// but calling SDL_DisplayFormat seems to actually be slowing things
		// down. This goes against established wisdom so if you want it back on,
		// run "displayformat 1" in the console and restart.
		setfile->displayformat = false;
		
		return 1;
	}
	else
	{
		#ifdef __SDLSHIM__
			stat("settings_load(): Hey FIXME!!!");
			settings->show_fps = true;
		#else
			input_set_mappings(settings->input_mappings);
		#endif
	}
	
	return 0;
}

/*
void c------------------------------() {}
*/

static bool tryload(Settings *setfile)
{
FILE *fp;

	stat("Loading settings...");

#ifdef __HAIKU__
	char path[PATH_MAX];
	char *haikuPath = getHaikuSettingsPath();
	strcpy(path, haikuPath);
	strcat(path, setfilename);
	free(haikuPath);
	fp = fileopen(path, "rb");
#else
	fp = fileopen(setfilename, "rb");
#endif
	if (!fp)
	{
		stat("Couldn't open file %s.", setfilename);
		return 1;
	}
	
	setfile->version = 0;
	fread(setfile, sizeof(Settings), 1, fp);
	if (setfile->version != SETTINGS_VERSION)
	{
		stat("Wrong settings version %04x.", setfile->version);
		return 1;
	}
	
	fclose(fp);
	return 0;
}


bool settings_save(Settings *setfile)
{
FILE *fp;

	if (!setfile)
		setfile = &normal_settings;
	
	stat("Writing settings...");
#ifdef __HAIKU__
	char path[PATH_MAX];
	char *haikuPath = getHaikuSettingsPath();
	strcpy(path, haikuPath);
	strcat(path, setfilename);
	free(haikuPath);
	fp = fileopen(path, "wb");
#else
	fp = fileopen(setfilename, "wb");
#endif
	if (!fp)
	{
		stat("Couldn't open file %s.", setfilename);
		return 1;
	}
	
	for(int i=0;i<INPUT_COUNT;i++)
		setfile->input_mappings[i] = input_get_mapping(i);
	
	setfile->version = SETTINGS_VERSION;
	fwrite(setfile, sizeof(Settings), 1, fp);
	
	fclose(fp);
	return 0;
}




