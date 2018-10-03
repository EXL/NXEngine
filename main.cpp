#ifdef __HAIKU__
#include <libgen.h>
#include <sys/stat.h>
#include <FindDirectory.h>
#endif

#include "nx.h"

#ifdef _SDL_MIXER
#include <SDL/SDL_mixer.h>
#endif

#include <stdarg.h>
#include <unistd.h>
#include "graphics/safemode.h"
#include "main.fdh"

const char *data_dir = "data";
const char *stage_dir = "data/Stage";
const char *pic_dir = "endpic";
const char *nxdata_dir = ".";

int fps = 0;
static int fps_so_far = 0;
static uint32_t fpstimer = 0;

#define GAME_WAIT			(1000/GAME_FPS)	// sets framerate
#define VISFLAGS			(SDL_APPACTIVE | SDL_APPINPUTFOCUS)
int framecount = 0;
bool freezeframe = false;
int flipacceltime = 0;

int main(int argc, char *argv[])
{

#ifdef __HAIKU__
    char *haikuPath = getHaikuSettingsPath();
    // To make it able to start from Tracker
    chdir(dirname(argv[0]));
    char path[PATH_MAX];
    strcpy(path, haikuPath);
    strcat(path, "replay");
    mkdir(haikuPath, 0755);
    mkdir(path, 0755);
#endif

bool inhibit_loadfade = false;
bool error = false;
bool freshstart;

#ifndef __HAIKU__
    SetLogFilename("debug.txt");
#else
    char logfile[PATH_MAX];
    strcpy(logfile, haikuPath);
    strcat(logfile, "debug.txt");
    SetLogFilename(logfile);
    free(haikuPath);
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        staterr("ack, sdl_init failed: %s.", SDL_GetError());
        return 1;
    }
	atexit(SDL_Quit);
	
	// start up inputs first thing because settings_load may remap them
	input_init();
	
	// load settings, or at least get the defaults,
	// so we know the initial screen resolution.
	settings_load();
	
	if (Graphics::init(settings->resolution)) { staterr("Failed to initialize graphics."); return 1; }
	if (font_init()) { staterr("Failed to load font."); return 1; }
	
	//speed_test();
	//return 1;
	
	#ifdef CONFIG_DATA_EXTRACTOR
	if (!settings->files_extracted)
	{
		if (extract_main())
		{
			Graphics::close();
			font_close();
			return 0;
		}
		else
		{
			settings->files_extracted = true;
			settings_save();
		}
	}
	#endif
	
	if (check_data_exists())
	{
		return 1;
	}
	
	//Graphics::ShowLoadingScreen();
	if (sound_init()) { fatal("Failed to initialize sound."); return 1; }
	if (trig_init()) { fatal("Failed trig module init."); return 1; }
	
	if (tsc_init()) { fatal("Failed to initialize script engine."); return 1; }
	if (textbox.Init()) { fatal("Failed to initialize textboxes."); return 1; }
	if (Carets::init()) { fatal("Failed to initialize carets."); return 1; }
	
	if (game.init()) return 1;
	game.setmode(GM_NORMAL);
	// set null stage just to have something to do while we go to intro
	game.switchstage.mapno = 0;
	
	//#define REPLAY
	#ifdef REPLAY
		game.switchstage.mapno = START_REPLAY;
		//Replay::set_ffwd(6000);
		//Replay::set_stopat(3500);
		game.switchstage.param = 1;
	#else
		//game.switchstage.mapno = LOAD_GAME;
		//game.pause(GP_OPTIONS);
		
		if (settings->skip_intro && file_exists(GetProfileName(settings->last_save_slot)))
			game.switchstage.mapno = LOAD_GAME;
		else
			game.setmode(GM_INTRO);
	#endif
	
	// for debug
	if (game.paused) { game.switchstage.mapno = 0; game.switchstage.eventonentry = 0; }
	if (game.switchstage.mapno == LOAD_GAME) inhibit_loadfade = true;
	
	game.running = true;
	freshstart = true;
	
	stat("Entering main loop...");
	#ifdef __SDLSHIM__
		set_console_visible(false);
	#endif
	
	//speed_test();
	//return 1;
	
	while(game.running)
	{
		// SSS/SPS persists across stage transitions until explicitly
		// stopped, or you die & reload. It seems a bit risky to me,
		// but that's the spec.
		if (game.switchstage.mapno >= MAPNO_SPECIALS)
		{
			StopLoopSounds();
		}
		
		// enter next stage, whatever it may be
		if (game.switchstage.mapno == LOAD_GAME || \
			game.switchstage.mapno == LOAD_GAME_FROM_MENU)
		{
			if (game.switchstage.mapno == LOAD_GAME_FROM_MENU)
				freshstart = true;
			
			stat("= Loading game =");
			if (game_load(settings->last_save_slot))
			{
				fatal("savefile error");
				goto ingame_error;
			}
			
			Replay::OnGameStarting();
			
			if (!inhibit_loadfade) fade.Start(FADE_IN, FADE_CENTER);
			else inhibit_loadfade = false;
		}
		else if (game.switchstage.mapno == START_REPLAY)
		{
			stat(">> beginning replay '%s'", GetReplayName(game.switchstage.param));
			
			StopScripts();
			if (Replay::begin_playback(GetReplayName(game.switchstage.param)))
			{
				fatal("error starting playback");
				goto ingame_error;
			}
		}
		else
		{
			if (game.switchstage.mapno == NEW_GAME || \
				game.switchstage.mapno == NEW_GAME_FROM_MENU)
			{
				bool show_intro = (game.switchstage.mapno == NEW_GAME_FROM_MENU);
				InitNewGame(show_intro);
			}
			
			// slide weapon bar on first intro to Start Point
			if (game.switchstage.mapno == STAGE_START_POINT && \
				game.switchstage.eventonentry == 91)
			{
				freshstart = true;
			}
			
			// switch maps
			if (load_stage(game.switchstage.mapno)) goto ingame_error;
			
			player->x = (game.switchstage.playerx * TILE_W) << CSF;
			player->y = (game.switchstage.playery * TILE_H) << CSF;
		}
		
		// start the level
		if (game.initlevel()) return 1;
		
		if (freshstart)
			weapon_introslide();
		
		gameloop();
		game.stageboss.OnMapExit();
		freshstart = false;
	}
	
shutdown: ;
	Replay::close();
	game.close();
	Carets::close();
	
	Graphics::close();
	input_close();
	font_close();
	sound_close();
	tsc_close();
	textbox.Deinit();
	return error;
	
ingame_error: ;
	stat("");
	stat(" ************************************************");
	stat(" * An in-game error occurred. Game shutting down.");
	stat(" ************************************************");
	error = true;
	goto shutdown;
}


void gameloop(void)
{
int32_t nexttick = 0;

	game.switchstage.mapno = -1;
	
	while(game.running && game.switchstage.mapno < 0)
	{
		// get time until next tick
		int32_t curtime = SDL_GetTicks();
		int32_t timeRemaining = nexttick - curtime;
		
		if (timeRemaining <= 0 || game.ffwdtime)
		{
			run_tick();
			
			// try to "catch up" if something else on the system bogs us down for a moment.
			// but if we get really far behind, it's ok to start dropping frames
			if (game.ffwdtime)
				game.ffwdtime--;
			
			nexttick = curtime + GAME_WAIT;
			
			// pause game if window minimized
			if ((SDL_GetAppState() & VISFLAGS) != VISFLAGS)
			{
				AppMinimized();
				nexttick = 0;
			}
		}
		else
		{
			// don't needlessly hog CPU, but don't sleep for entire
			// time left, some CPU's/kernels will fall asleep for
			// too long and cause us to run slower than we should
			timeRemaining /= 2;
			if (timeRemaining)
				SDL_Delay(timeRemaining);
		}
	}
}

static inline void run_tick()
{
static bool can_tick = true;
static bool last_freezekey = false;
static bool last_framekey = false;
static int frameskip = 0;

	input_poll();
	
	// input handling for a few global things
	if (justpushed(ESCKEY))
	{
		if (settings->instant_quit)
		{
			game.running = false;
		}
		else if (!game.paused)		// no pause from Options
		{
			game.pause(GP_PAUSED);
		}
	}
#if defined (_DINGUX) || defined (_MOTOMAGX) || defined (_MOTOEZX)
    else if (justpushed(OPTIONS_KEY))
#else
    else if (justpushed(F3KEY))
#endif
	{
		game.pause(GP_OPTIONS);
	}
	
	// freeze frame
	if (settings->enable_debug_keys)
	{
		if (inputs[FREEZE_FRAME_KEY] && !last_freezekey)
		{
			can_tick = true;
			freezeframe ^= 1;
			framecount = 0;
		}
		
		if (inputs[FRAME_ADVANCE_KEY] && !last_framekey)
		{
			can_tick = true;
			if (!freezeframe)
			{
				freezeframe = 1;
				framecount = 0;
			}
		}
		
		last_freezekey = inputs[FREEZE_FRAME_KEY];
		last_framekey = inputs[FRAME_ADVANCE_KEY];
	}
	
	// fast-forward key (F5)
	if (inputs[FFWDKEY] && (settings->enable_debug_keys || Replay::IsPlaying()))
	{
		game.ffwdtime = 2;
	}
	
	if (can_tick)
	{
		game.tick();
		
		if (freezeframe)
		{
			char buf[1024];
			sprintf(buf, "[] Tick %d", framecount++);
			font_draw_shaded(4, (SCREEN_HEIGHT-GetFontHeight()-4), buf, 0, &greenfont);
			can_tick = false;
		}
		else
		{
			Replay::DrawStatus();
		}
		
		if (settings->show_fps)
		{
			update_fps();
		}
		
		if (!flipacceltime)
		{
			//platform_sync_to_vblank();
			screen->Flip();
		}
		else
		{
			flipacceltime--;
			if (--frameskip < 0)
			{
				screen->Flip();
				frameskip = 256;
			}
		}
		
		memcpy(lastinputs, inputs, sizeof(lastinputs));
	}
	else
	{	// frame is frozen; don't hog CPU
		SDL_Delay(20);
	}
	
	// immediately after a game tick is when we have the most amount of time before
	// the game needs to run again. so now's as good a time as any for some
	// BGM audio processing, wouldn't you say?
	org_run();
}

void update_fps()
{
	fps_so_far++;
	
	if ((SDL_GetTicks() - fpstimer) >= 500)
	{
		fpstimer = SDL_GetTicks();
		fps = (fps_so_far << 1);
		fps_so_far = 0;
	}
	
	char fpstext[64];
	sprintf(fpstext, "%d fps", fps);
	
	int x = (SCREEN_WIDTH - 4) - GetFontWidth(fpstext, 0, true);
	font_draw_shaded(x, 4, fpstext, 0, &greenfont);
}


void InitNewGame(bool with_intro)
{
	stat("= Beginning new game =");
	
	memset(game.flags, 0, sizeof(game.flags));
	memset(game.skipflags, 0, sizeof(game.skipflags));
	textbox.StageSelect.ClearSlots();
	
	game.quaketime = game.megaquaketime = 0;
	game.showmapnametime = 0;
	game.debug.god = 0;
	game.running = true;
	game.frozen = false;
	
	// fully re-init the player object
	Objects::DestroyAll(true);
	game.createplayer();
	
	player->maxHealth = 3;
	player->hp = player->maxHealth;
	
	game.switchstage.mapno = STAGE_START_POINT;
	game.switchstage.playerx = 10;
	game.switchstage.playery = 8;
	game.switchstage.eventonentry = (with_intro) ? 200 : 91;
	
	fade.set_full(FADE_OUT);
}


void AppMinimized(void)
{
	stat("Game minimized or lost focus--pausing...");

#ifdef _SDL_MIXER
    Mix_Pause(-1);
    Mix_PauseMusic();
#else
    SDL_PauseAudio(1);
#endif
	
	for(;;)
	{
		if ((SDL_GetAppState() & VISFLAGS) == VISFLAGS)
		{
			break;
		}
		
		input_poll();
		SDL_Delay(20);
	}
#ifdef _SDL_MIXER
    Mix_Resume(-1);
    Mix_ResumeMusic();
#else
    SDL_PauseAudio(0);
#endif
	stat("Focus regained, resuming play...");
}


/*
void c------------------------------() {}
*/

static void fatal(const char *str)
{
	staterr("fatal: '%s'", str);
	
	if (!safemode::init())
	{
		safemode::moveto(SM_UPPER_THIRD);
		safemode::print("Fatal Error");
		
		safemode::moveto(SM_CENTER);
		safemode::print("%s", str);
		
		safemode::run_until_key();
		safemode::close();
	}
}

static bool check_data_exists()
{
char fname[MAXPATHLEN];

	sprintf(fname, "%s/npc.tbl", data_dir);
	if (file_exists(fname)) return 0;
	
	if (!safemode::init())
	{
		safemode::moveto(SM_UPPER_THIRD);
		safemode::print("Fatal Error");
		
		safemode::moveto(SM_CENTER);
		safemode::print("Missing \"%s\" directory.", data_dir);
		safemode::print("Please copy it over from a Doukutsu installation.");
		
		safemode::run_until_key();
		safemode::close();
	}
	
	return 1;
}

void visible_warning(const char *fmt, ...)
{
va_list ar;
char buffer[80];

	va_start(ar, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, ar);
	va_end(ar);
	
	console.Print(buffer);
}

/*
void c------------------------------() {}
*/

#ifdef __SDLSHIM__

void speed_test(void)
{
	SDL_Rect textrect;
	
	uint32_t end = 0;
	fps = 0;
	
	ClearScreen(255, 0, 0);
	
	game.running = true;
	while(game.running)
	{
		if (SDL_GetTicks() >= end)
		{
			SDLS_VRAMSurface->h = 320;
			SDLS_VRAMSurface->cliprect.h = 320;
			textrect.x = 5;
			textrect.y = 250;
			textrect.w = 100;
			textrect.h = 10;
			SDL_FillRect(SDLS_VRAMSurface, &textrect, 0);
			
			char buffer[80];
			sprintf(buffer, "%d fps ", fps);
			direct_text_draw(5, 250, buffer);
			SDLS_VRAMSurface->h = 240;
			SDLS_VRAMSurface->cliprect.h = 240;
			
			input_poll();
			
			fps = 0;
			end = SDL_GetTicks() + 1000;
		}
		
		screen->Flip();
		fps++;
	}
}

#else

void speed_test(void)
{
	SDL_Rect textrect;
	SDL_Surface *vram = screen->GetSDLSurface();
	int click = 0;
	
	uint32_t end = 0;
	fps = 0;
	
	SDL_FillRect(vram, NULL, SDL_MapRGB(vram->format, 255, 0, 0));
	int c = 0;
	
	game.running = true;
	while(game.running)
	{
		//SDL_FillRect(vram, NULL, c ^= 255);
		
		if (SDL_GetTicks() >= end)
		{
			stat("%d fps", fps);
			fps = 0;
			end = SDL_GetTicks() + 1000;
			
			if (++click > 3)
				break;
		}
		
		screen->Flip();
		fps++;
	}
}

#endif



void org_test_miniloop(void)
{
uint32_t start = 0, curtime;
uint32_t counter;

	stat("Starting org test");
	
	font_draw(5, 5, "ORG test in progress...", 0, &greenfont);
	font_draw(5, 15, "Logging statistics to nx.log", 0, &greenfont);
	font_draw(5, 25, "Press any button to quit", 0, &greenfont);
	screen->Flip();
	
	music_set_enabled(1);
	music(32);
	
	last_sdl_key = -1;
	
	for(;;)
	{
		org_run();
		
		if (++counter > 1024)
		{
			counter = 0;
			
			curtime = SDL_GetTicks();
			if ((curtime - start) >= 100)
			{
				start = curtime;
				input_poll();
				
				if (last_sdl_key != -1)
					return;
			}
		}
	}
}

void SDL_Delay(int ms)
{
	usleep(ms * 1000);
}

#ifdef __HAIKU__
char *getHaikuSettingsPath() {
	char path[PATH_MAX];
	find_directory(B_USER_SETTINGS_DIRECTORY, -1, false, path, sizeof(path));
	strcat(path, "/NXEngine/");
	return strdup(path);
}
#endif
