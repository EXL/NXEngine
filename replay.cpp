
#include <time.h>
#include "nx.h"
#include "replay.h"
#include "profile.h"
#include "replay.fdh"
using namespace Replay;

#define REPLAY_MAGICK		0xC322
static ReplayRecording rec;
static ReplayPlaying play;

static int next_ffwdto = 0;
static int next_stopat = 0;
static bool next_accel = false;
extern int flipacceltime;

// begin recording a replay into the given file,
// creating the save-profile section from the current game state.
bool Replay::begin_record(const char *fname)
{
FILE *fp;
Profile profile;

	end_record();
	memset(&rec, 0, sizeof(rec));
	
	stat("begin_record('%s')", fname);
	
	// grab a savefile record of the game state and write it at the start of the file
	// just like a regular profile.dat.
	if (game_save(&profile)) return 1;
	if (profile_save(fname, &profile)) return 1;
	
	fp = fileopen(fname, "r+");
	if (!fp)
	{
		staterr("begin_record: failed to open file %s", fname);
		return 1;
	}
	
	fseek(fp, PROFILE_LENGTH, SEEK_SET);	// seek to end of profile data
	
	rec.hdr.magick = REPLAY_MAGICK;
	rec.hdr.randseed = getrand();
	rec.hdr.locked = false;
	rec.hdr.total_frames = 0;
	rec.hdr.createstamp = (uint64_t)time(NULL);
	rec.hdr.stageno = game.curmap;
	memcpy(&rec.hdr.settings, &normal_settings, sizeof(Settings));
	
	fwrite(&rec.hdr, sizeof(ReplayHeader), 1, fp);
	
	rec.fp = fp;
	seedrand(rec.hdr.randseed);
	
	fputl('MARK', fp);
	rec.fb.SetFile(fp);
	rec.fb.SetBufferSize(256);
	rec.fb.Dump();
	return 0;
}

bool Replay::end_record()
{
	if (!IsRecording())
		return 1;
	
	// flush final RLE run
	write_record(rec.lastkeys, rec.runlength, &rec.fb);
	rec.runlength = 0;
	rec.fb.Flush();
	rec.fb.SetFile(NULL);
	
	fputc('!', rec.fp);
	fputl('STOP', rec.fp);
	
	// go back and save the header again so we have total_frames correct.
	fseek(rec.fp, PROFILE_LENGTH, SEEK_SET);
	fwrite(&rec.hdr, sizeof(ReplayHeader), 1, rec.fp);
	fclose(rec.fp);
	
	stat("end_record(): wrote %d frames", rec.hdr.total_frames);
	memset(&rec, 0, sizeof(rec));
	return 0;
}

/*
void c------------------------------() {}
*/

// load the save-game contained with the given replay and begin playback.
bool Replay::begin_playback(const char *fname)
{
FILE *fp;
Profile profile;

	end_playback();
	memset(&play, 0, sizeof(play));
	
	stat("begin_playback('%s')", fname);
	
	if (profile_load(fname, &profile))
		return 1;
	
	fp = fileopen(fname, "rb");
	if (!fp)
	{
		staterr("begin_playback: failed to open file %s", fname);
		return 1;
	}
	
	fseek(fp, PROFILE_LENGTH, SEEK_SET);	// seek to end of profile data
	fread(&play.hdr, sizeof(ReplayHeader), 1, fp);
	
	if (play.hdr.magick != REPLAY_MAGICK)
	{
		staterr("begin_playback: magick mismatch on file '%s' (%x shouldbe %x)", fname, play.hdr.magick, REPLAY_MAGICK);
		return 1;
	}
	
	// undo settings we don't want to apply during the replay
	memcpy(&replay_settings, &play.hdr.settings, sizeof(Settings));
	replay_settings.resolution = normal_settings.resolution;
	replay_settings.sound_enabled = normal_settings.sound_enabled;
	replay_settings.music_enabled = normal_settings.music_enabled;
	
	game_load(&profile);
	seedrand(play.hdr.randseed);
	
	if (fgetl(fp) != 'MARK')
	{
		console.Print("Replay fail MARK");
		return 1;
	}
	
	// debug stuff for replaying at startup from main.cpp
	play.ffwdto = next_ffwdto;
	next_ffwdto = 0;
	
	play.stopat = next_stopat;
	next_stopat = 0;
	
	play.ffwd_accel = next_accel;
	next_accel = 0;
	
	play.fp = fp;
//	dump_replay();
	return 0;
}

bool Replay::end_playback()
{
	if (!IsPlaying()) return 1;
	
	fclose(play.fp);
	play.fp = NULL;
	
	memset(inputs, 0, sizeof(inputs));
	play.termtimer = 110;
	
	return 0;
}

/*
void c------------------------------() {}
*/

// run record and playback.
// * inputs states can be recorded to the file.
// * inputs can be injected into the array from the file.
void Replay::run()
{
	//debug("%d %d", IsPlaying(), IsRecording());
	
	if (IsPlaying())
	{
		settings = &replay_settings;
		run_playback();
	}
	else
	{
		settings = &normal_settings;
	}
	
	if (IsRecording())
		run_record();
}


// record incoming inputs states to the record_fp.
void Replay::run_record()
{
	rec.hdr.total_frames++;
	uint32_t keys = EncodeBits(inputs, INPUT_COUNT);
	
	if (keys != rec.lastkeys)
	{
		if (rec.runlength != 0)
		{
			write_record(rec.lastkeys, rec.runlength, &rec.fb);
			rec.runlength = 0;
		}
		
		rec.lastkeys = keys;
	}
	
	rec.runlength++;
}

static void Replay::run_playback()
{
	play.elapsed_frames++;
	
	if (play.stopat && play.elapsed_frames >= play.stopat)
	{
		end_playback();
		return;
	}
	
	if (play.ffwdto && play.elapsed_frames < play.ffwdto)
	{
		game.ffwdtime = 2;
		if (play.ffwd_accel)
			flipacceltime = 2;	// global variable from main; disables screen->Flip()
	}
	
	// RLE decoding
	if (play.runlength == 0)
	{
		if (read_record(&play.keys, &play.runlength, play.fp))
		{
			end_playback();
			play.keys = 0;
			return;
		}
		
		play.elapsed_records++;
	}
	
	play.runlength--;
	
	debug("keys: %08x", play.keys);
	debug("runlength: %d", play.runlength);
	debug("frame: %d", play.elapsed_frames);
	debug("record: %d", play.elapsed_records);
	//debug("frames left: %d", (play.hdr.total_frames - play.elapsed_frames));
	
	bool keys[INPUT_COUNT];
	DecodeBits(play.keys, keys, INPUT_COUNT);
	
	// which recorded inputs should be applied
	static const int list[] =
	{
		LEFTKEY, RIGHTKEY, UPKEY, DOWNKEY,
		JUMPKEY, FIREKEY, PREVWPNKEY, NEXTWPNKEY,
		INVENTORYKEY, MAPSYSTEMKEY,
		DEBUG_MOVE_KEY, DEBUG_GOD_KEY, DEBUG_FLY_KEY,
		-1
	};
	
	for(int i=0;list[i] != -1;i++)
	{
		int key = list[i];
		inputs[key] = keys[key];
	}
	
	return;
}

/*
void c------------------------------() {}
*/

static void write_record(uint32_t keys, uint32_t runlength, FileBuffer *fb)
{
	fb->Write8('[');
	fb->Write32(keys);
	fb->Write8(':');
	fb->Write32(runlength);
	fb->Write8(']');
}

static bool read_record(uint32_t *keys, uint32_t *runlength, FILE *fp)
{
	char ch = fgetc(fp);
	if (ch == '!') return REC_END;
	
	if (feof(fp))
	{
		console.Print("unexpected end of file");
		return REC_ERR;
	}
	
	if (ch != '[')
	{
		console.Print("replay field fail [");
		return REC_ERR;
	}
	
	*keys = fgetl(fp);
	
	if (fgetc(fp) != ':')
	{
		console.Print("replay field fail :");
		return REC_ERR;
	}
	
	*runlength = fgetl(fp);
	
	if (fgetc(fp) != ']')
	{
		console.Print("replay field fail ]");
		return REC_ERR;
	}
	
	return REC_OK;
}

/*
void c------------------------------() {}
*/

// draw the playback status "tape"
void Replay::DrawStatus()
{
static const int TAPE_CHARS = 10;
static const int x = 4;
char buf[40];
int tapepos;

	if (!IsPlaying() || game.paused)
	{
		if (play.termtimer > 0)
		{
			play.termtimer--;
			const char *str = ((play.termtimer % 40) >= 20) ? "> PLAYBACK TERMINATED <":">                     <";
			
			int y = (SCREEN_HEIGHT - 3) - (GetFontHeight() * 2);
			font_draw_shaded(x, y, str, 0, &greenfont);
		}
		
		return;
	}
	
	// ask for one more char in length than we actually have, this makes it
	// so the tape gets all-the-way filled up near the end (otherwise, the only time
	// it'd be fully filled is on the very last frame, so we wouldn't ever see it).
	tapepos = GetPlaybackPosition(TAPE_CHARS + 1);
	if (tapepos > TAPE_CHARS) tapepos = TAPE_CHARS;
	
	// > PLAY : 00000
	//   [>>>>>>>>>>]
	buf[0] = '['; buf[TAPE_CHARS+1] = ']'; buf[TAPE_CHARS+2] = 0;
	memset(&buf[1], ' ', TAPE_CHARS);
	memset(&buf[1], '>', tapepos);
	
	int y = (SCREEN_HEIGHT - 3) - GetFontHeight();
	font_draw_shaded(x, y, buf, 0, &greenfont);
	
	const char *mode = ((play.elapsed_frames % 40) < 20) ? "PLAY" : "    ";
	if (game.ffwdtime) mode = "FFWD";
	sprintf(buf, "> %s : %05d", mode, play.elapsed_frames);
	
	y -= GetFontHeight();
	font_draw_shaded(x, y, buf, 0, &greenfont);
}

/*
void c------------------------------() {}
*/

// called from main after a game is loaded or a new game is begun.
void Replay::OnGameStarting()
{
	stat("Replay::OnGameStarting()");
	
	if (!IsPlaying())
		begin_record_next();
}


bool Replay::begin_record_next()
{
	int slot = GetAvailableSlot();
	if (slot == -1)
	{
		stat("begin_record_next: all slots locked; not recording a replay");
		return 1;
	}
	
	stat("begin_record_next: starting record to slot %d", slot);
	return begin_record(GetReplayName(slot));
}


static int Replay::GetAvailableSlot(void)
{
ReplaySlotInfo slotinfo[MAX_REPLAYS];
ReplaySlotInfo *unlocked[MAX_REPLAYS];
int i, numUnlocked;

	// get info for all slots.
	// try to find a slot that isn't used yet, if we can, take the first one.
	for(i=MAX_REPLAYS-1;i>=0;i--)
	{
		GetSlotInfo(i, &slotinfo[i]);
		//stat("Read status of slot %d: [ %d ]", i, slotinfo[i].status);
		
		if (slotinfo[i].status == RS_UNUSED)
			return i;
	}
	
	// nope, the easy way out didn't work because all the replay slots
	// have at least something in them. so, get a list of all unlocked files.
	numUnlocked = 0;
	for(i=0;i<MAX_REPLAYS;i++)
	{
		if (slotinfo[i].status == RS_UNLOCKED)
			unlocked[numUnlocked++] = &slotinfo[i];
	}
	
	// if the unlocked list has 0 entries then all slots are both in use and locked.
	// we fail. return.
	if (numUnlocked == 0)
		return -1;
	
	// delete the file in the highest-numbered slot.
	remove(unlocked[--numUnlocked]->filename);
	unlocked[numUnlocked]->status = RS_UNUSED;
	
	// assign new filenames to all the files in the list, working downward
	// from the highest-numbered slot. The going backwards also ensures there
	// can't be any conflicts while we're in the middle of this.
	int nextslot = (MAX_REPLAYS - 1);
	for(i=numUnlocked-1;i>=0;i--)
	{
		// skip over locked slots
		while(slotinfo[nextslot].status == RS_LOCKED)
			nextslot--;
		
		const char *newfilename = GetReplayName(nextslot);
		rename(unlocked[i]->filename, newfilename);
		strcpy(unlocked[i]->filename, newfilename);
		
		nextslot--;
	}
	
	// now take the first unused slot. we're sure that there is one this time.
	for(i=0;i<MAX_REPLAYS;i++)
	{
		if (!file_exists(GetReplayName(i)))
			return i;
	}
	
	staterr("GetAvailableSlot: deleted one but still none available???");
	return -1;
}


/*
void c------------------------------() {}
*/

// returns locked/unlocked/available status and filename info for the given replay slot.
void Replay::GetSlotInfo(int slotno, ReplaySlotInfo *slot)
{
	GetReplayName(slotno, slot->filename);
	
	if (LoadHeader(slot->filename, &slot->hdr) || \
		slot->hdr.magick != REPLAY_MAGICK)
	{
		slot->status = RS_UNUSED;
		slot->filename[0] = 0;
	}
	else
	{
		slot->status = (slot->hdr.locked) ? RS_LOCKED : RS_UNLOCKED;
	}
}


const char *GetReplayName(int slotno, char *buffer)
{
	if (!buffer) buffer = GetStaticStr();
#ifndef __HAIKU__
	sprintf(buffer, "replay/rep%d.dat", slotno);
#else
	char path[PATH_MAX];
	char *haikuPath = getHaikuSettingsPath();
	strcpy(path, haikuPath);
	strcat(path, "replay/rep%d.dat");
	sprintf(buffer, path, slotno);
	free(haikuPath);
#endif
	return buffer;
}

bool Replay::LoadHeader(const char *fname, ReplayHeader *hdr)
{
FILE *fp;

	fp = fileopen(fname, "rb");
	if (!fp)
	{
		staterr("LoadHeader: can't open file '%s'", fname);
		return 1;
	}
	
	fseek(fp, PROFILE_LENGTH, SEEK_SET);	// seek to end of profile data
	fread(hdr, sizeof(ReplayHeader), 1, fp);
	
	fclose(fp);
	return 0;
}

bool Replay::SaveHeader(const char *fname, ReplayHeader *hdr)
{
FILE *fp;

	fp = fileopen(fname, "r+");
	if (!fp)
	{
		staterr("SaveHeader: can't open file '%s'", fname);
		return 1;
	}
	
	fseek(fp, PROFILE_LENGTH, SEEK_SET);	// seek to end of profile data
	fwrite(hdr, sizeof(ReplayHeader), 1, fp);
	
	fclose(fp);
	return 0;
}

/*
void c------------------------------() {}
*/

// converts a framecount value into a textual total time.
void Replay::FramesToTime(int framecount, char *buffer)
{
int mins, secs;

	secs = (framecount / GAME_FPS);
	mins = (secs / 60);
	secs = (secs % 60);
	if (mins > 99) mins = 99;
	
	sprintf(buffer, "[%02d:%02d]", mins, secs);
}

// returns a value between 0 and max which is like a percentage
// of where we are on the "tape" (only works during playback).
int Replay::GetPlaybackPosition(int max)
{
	if (play.elapsed_frames >= play.hdr.total_frames)
		return max;
	
	double ratio = ((double)max / (double)play.hdr.total_frames);
	return (int)((double)play.elapsed_frames * ratio);
}

/*
void c------------------------------() {}
*/

bool Replay::IsRecording()
{
	return (rec.fp != NULL);
}

bool Replay::IsPlaying()
{
	return (play.fp != NULL);
}

void Replay::close()
{
	end_record();
	end_playback();
}

/*
void c------------------------------() {}
*/

void Replay::set_ffwd(int frame, bool accel)
{
	if (IsPlaying())
	{
		play.ffwdto = frame;
		play.ffwd_accel = accel;
	}
	else
	{
		next_ffwdto = frame;
		next_accel = accel;
	}
}

void Replay::set_stopat(int frame)
{
	if (IsPlaying())
		play.stopat = frame;
	else
		next_stopat = frame;
}

/*
void c------------------------------() {}
*/

// shoves an array of bool values up to 32 entries long into a uint32.
static uint32_t Replay::EncodeBits(bool *values, int nvalues)
{
uint32_t result = 0;
uint32_t mask = 1;

	for(int i=0;i<nvalues;i++)
	{
		if (values[i]) result |= mask;
		mask <<= 1;
	}
	
	return result;
}

// pulls apart a uint32 created by EncodeBits back into a bool array.
void Replay::DecodeBits(uint32_t value, bool *array, int len)
{
uint32_t mask = 1;
int i;

	for(i=0;i<len;i++)
	{
		array[i] = (value & mask) ? 1 : 0;
		mask <<= 1;
	}
}

/*
void c------------------------------() {}
*/

static void dump_replay()
{
	stat("=== Header ===");
	stat("magick: %04x (%s)", play.hdr.magick, (play.hdr.magick == REPLAY_MAGICK) ? "correct" : "not correct");
	stat("randseed: %08x", play.hdr.randseed);
	stat("locked: %d", play.hdr.locked);
	stat("total_frames: %d (%d secs)", play.hdr.total_frames, play.hdr.total_frames / 50);
	stat("stageno: %d (%s)", play.hdr.stageno, map_get_stage_name(play.hdr.stageno));
	stat("createstamp: %010llx", play.hdr.createstamp);
	stat("=== End Header ===");
	stat("");
	
	stat("resolution: %d", play.hdr.settings.resolution);
	stat("last_save_slot: %d", play.hdr.settings.last_save_slot);
	stat("multisave: %d", play.hdr.settings.multisave);
	/*int resolution;
	int last_save_slot;
	bool multisave;
	bool files_extracted;
	bool show_fps;
	bool displayformat;
	
	bool enable_debug_keys;
	bool sound_enabled;
	int music_enabled;
	
	bool instant_quit;
	bool emulate_bugs;
	bool no_quake_in_hell;
	bool inhibit_fullscreen;
	
	bool skip_intro;*/
	
	//exit(1);
	
	/*stat("Inputs:");
	for(int i=0;i<INPUT_COUNT;i++)
	{
		stat("  %08x  %08x", play.hdr.settings.input_mappings[i], input_get_mapping(i));
	}*/
	
	fseek(play.fp, ftell(play.fp) - 0x2b, SEEK_SET);	// 0x6a9
	stat("First Read %08x", fgetl(play.fp));
	stat("Starting read at offset %04x", ftell(play.fp));
	//exit(1);
	
	//exit(1);
	//int offset = 1;
	//fseek(play.fp, ftell(play.fp)+offset, SEEK_SET);
	
	int total_frames = 0;
	int record = 0;
	uint32_t lastkeys = 0xffffffff;
	while(!feof(play.fp))
	{
		uint32_t keys = fgetl(play.fp);
		uint32_t runlength = fgetl(play.fp);
		if (runlength == 0xffffffff || feof(play.fp)) break;
		total_frames += runlength;
		
		stat("%04d  len %08x:  keys %08x     offset %08x", record++, runlength, keys, ftell(play.fp)-8);
		
		if (keys == lastkeys)
		{
			staterr(" -- impossible key repeat");
			if (runlength < 0x200000) break;
		}
		
		if (runlength >= 0x200000)
		{
			staterr(" -- bogus runlength %08x", runlength);
			break;
		}
		
		lastkeys = keys;
	}
	
	//total_frames--;
	stat("total frames count: %d", total_frames);
	stat("frames reported: %d", play.hdr.total_frames);
	exit(1);
}


