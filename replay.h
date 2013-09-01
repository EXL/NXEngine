
#ifndef _REPLAY_H
#define _REPLAY_H

#include "common/FileBuffer.h"
#define MAX_REPLAYS				8	// how many automatic replays to save

#define REC_OK		0
#define REC_ERR		1
#define REC_END		2
struct ReplayHeader
{
	uint16_t magick;
	uint32_t randseed;
	bool locked;
	int total_frames;
	int stageno;
	uint64_t createstamp;
	Settings settings;
};

struct ReplayRecording
{
	ReplayHeader hdr;
	FileBuffer fb;
	
	uint32_t lastkeys;
	uint32_t runlength;
	FILE *fp;
};

struct ReplayPlaying
{
	ReplayHeader hdr;
	
	uint32_t keys;
	uint32_t runlength;
	int elapsed_frames;
	int elapsed_records;
	FILE *fp;
	
	int ffwdto, ffwd_accel;
	int stopat;
	
	int termtimer;		// blinks "TERMINATED" after replay ends
};

enum RS_Status
{
	RS_UNUSED,		// there is no file in this slot
	RS_UNLOCKED,	// there is an unlocked file in this slot
	RS_LOCKED		// there is a locked file in this slot
};

struct ReplaySlotInfo
{
	char filename[MAXPATHLEN];	// filename of the replay for this slot, if there is one
	int status;					// status of this slot
	ReplayHeader hdr;			// header from slot
};


namespace Replay
{
	bool begin_record(const char *fname);
	bool end_record();
	
	bool begin_playback(const char *fname);
	bool end_playback();
	
	void run();
	void close();
	
	void OnGameStarting();
	bool begin_record_next();
	
	bool IsRecording();
	bool IsPlaying();
	void DrawStatus();
	
	void set_ffwd(int frame, bool accel=true);
	void set_stopat(int frame);
	
	
	bool LoadHeader(const char *fname, ReplayHeader *hdr);
	bool SaveHeader(const char *fname, ReplayHeader *hdr);
	
	void GetSlotInfo(int slotno, ReplaySlotInfo *slot);
	void FramesToTime(int framecount, char *buffer);
	int GetPlaybackPosition(int max);
	
	static uint32_t EncodeBits(bool *values, int nvalues);
	static void DecodeBits(uint32_t value, bool *array, int len);
	
	static void run_record();
	static void run_playback();
	
	static int GetAvailableSlot(void);
};


const char *GetReplayName(int slotno, char *buffer = NULL);

#endif
