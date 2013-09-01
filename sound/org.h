
#ifndef _ORG_H
#define _ORG_H

// SSChannel # to play the music on
#define ORG_CHANNEL			15

// maximum possible volume of an org instrument (in the file)
// i hear according to orgmaker you can't go higher than F8 but a few
// songs actually do go all the way to FF.
#define ORG_MAX_VOLUME		255

// which panning value is perfectly centered
#define ORG_PAN_CENTERED	6
#define ORG_PAN_FULL_RIGHT	(ORG_PAN_CENTERED + ORG_PAN_CENTERED)

#define KEYS_OCTAVE			12
#define NUM_NOTES			(8 * KEYS_OCTAVE)	// range of note values: 00-this.
#define NUM_DRUMS			12					// max # of drum instruments to load

// if you care to know which music note an org note is, modulus it by
// KEYS_OCTAVE and compare to the following constants.
#define NOTE_C			0
#define NOTE_CS			1
#define NOTE_D			2
#define NOTE_DS			3
#define NOTE_E			4
#define NOTE_F			5
#define NOTE_FS			6
#define NOTE_G			7
#define NOTE_GS			8
#define NOTE_A			9
#define NOTE_AS			10
#define NOTE_B			11

#define MAX_SONG_LENGTH		5000		// max song length to allocate for, in notes


// this handles the actual synthesis
struct stNoteChannel
{
	signed short *outbuffer;
	
	// position inside outbuffer (not the same as samples_so_far because org module outputs stereo.
	// outpos counts data, samples_so_far counts samples. samples_so_far=outpos*2)
	int outpos;				// incs by 2 for each samples_so_far, one for left ch one for right ch
	int samples_so_far;		// number of samples generated so far into outbuffer
	
	double phaseacc;		// current read position inside wavetable sample
	double sample_inc;		// speed at which to iterate over the wavetable waveform
	
	// for drums
	double master_volume_ratio, volume_left_ratio, volume_right_ratio;
	
	int wave;				// index into wavetable (which instrument we're using)
	int volume;				// last volume value sent to note_gen
	int panning;			// last panning value sent to note_gen
	int length;				// # of beats of the current note left to generate
	
	int number;				// the chanel number of this channel
};


struct stNote
{
	int beat;			// beat no. that note starts on
	uchar note;			// 00 - 5F, starts on a C
	uchar length;		// in beats
	uchar volume;		// 00 - F8
	uchar panning;		// 00 - 0C
};

// keeps track of instrument settings for a track
struct stInstrument
{
	int pitch;
	int wave;				// which wave (00-99) to use
	// if pi is set all notes on the channel play for 1024 samples regardless
	// of length or tempo settings. pi only has meaning on the instrument tracks.
	bool pi;
	
	int curnote;			// current note (during playback)
	int loop_note;			// used when looping back to the beginning of a song at beat loop_end
	
	int nnotes;
	stNote note[MAX_SONG_LENGTH];
};

struct stSong
{
	bool playing;
	int volume;
	
	int ms_per_beat, ms_of_last_beat_of_note;
	int beats_per_step;
	int steps_per_bar;
	int beats_per_bar;				// == (beats_per_step * steps_per_bar)
	
	int samples_per_beat;			// # of samples in each beat
	int note_closing_samples;		// # of samples of note to generate at the last beat of a note
	
	int loop_start, loop_end;
	
	stInstrument instrument[16];
	
	int beat;
	char haslooped;
	
	bool fading;
	uint32_t last_fade_time;
};

#endif

