#include "EZX_SDL_PollEvent.h"

#include <SDL/SDL_mixer.h>

SDL_AudioSpec EZX_SDL_obtained;
bool EZX_SDL_qAudioSuspend;

void EZX_SDL_mix_CloseAudio(void);

int EZX_SDL_PollEvent(SDL_Event *event)
{
    int result = SDL_PollEvent(event);
    if (!result) {
        return 0;
    }
    if (event->type == SDL_ACTIVEEVENT) {
        if (event->active.state == SDL_APPINPUTFOCUS && !event->active.gain) {
            EZX_SDL_mix_SuspendAudio(); // Use SDL_mixer
            for (;;) {
                result = SDL_WaitEvent(event);
                if (!result) {
                    continue;
                }
                if (event->type == SDL_QUIT) {
                    return 1;
                }
                if (event->type != SDL_ACTIVEEVENT) {
                    continue;
                }
                if (event->active.state == SDL_APPINPUTFOCUS && event->active.gain) {
                    EZX_SDL_mix_ResumeAudio(); // Use SDL_mixer
                    return 1;
                }
            }
        }
    }
    return result;
}

// Audio through SDL
void EZX_SDL_SuspendAudio()
{
    SDL_CloseAudio();
    EZX_SDL_qAudioSuspend = true;
}

int EZX_SDL_ResumeAudio()
{
    if (SDL_OpenAudio(&EZX_SDL_obtained, NULL) < 0) {
        return -1;
    }
    SDL_PauseAudio(0);
    EZX_SDL_qAudioSuspend = false;
    return 0;
}

// Audio through SDL_mixer
void EZX_SDL_mix_SuspendAudio()
{
    Mix_SuspendAudio();
}

int EZX_SDL_mix_ResumeAudio()
{
    int result = Mix_ResumeAudio();
    if (result == (-1)) {
        EZX_SDL_mix_CloseAudio();
        return (-1);
    }
}

void EZX_SDL_mix_CloseAudio(void)
{
    Mix_CloseAudio();
}
