#ifndef XPMLOADER_H
#define XPMLOADER_H

#include <SDL/SDL.h>
extern "C" {
int IMG_isXPM(SDL_RWops *src);
static int hash_key(const char *key, int cpp, int size);
static struct color_hash *create_colorhash(int maxnum);
static int add_colorhash(struct color_hash *hash,
                         char *key, int cpp, Uint32 color);
static Uint32 get_colorhash(struct color_hash *hash, const char *key, int cpp);
static void free_colorhash(struct color_hash *hash);
static int string_equal(const char *a, const char *b, int n);
static int color_to_rgb(char *spec, int speclen, Uint32 *rgb);
static char *get_next_line(char ***lines, SDL_RWops *src, int len);
SDL_Surface *load_xpm(char **xpm, SDL_RWops *src);
SDL_Surface *IMG_LoadXPM_RW(SDL_RWops *src);
SDL_Surface *IMG_ReadXPMFromArray(char **xpm);
} // end of extern "C"
#endif // XPMLOADER_H
