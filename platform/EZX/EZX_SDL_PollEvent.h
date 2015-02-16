#ifndef EZX_SDL_POLLEVENT
#define EZX_SDL_POLLEVENT

#include <SDL/SDL.h>

int EZX_SDL_PollEvent(SDL_Event *event);

// Audio through SDL
void EZX_SDL_SuspendAudio();
int EZX_SDL_ResumeAudio();

// Audio through SDL_mixer
void EZX_SDL_mix_SuspendAudio();
int EZX_SDL_mix_ResumeAudio();

#endif // EZX_SDL_POLLEVENT
