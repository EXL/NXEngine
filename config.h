
#ifndef _CONFIG_H
#define _CONFIG_H

// if set graphics scaling factor is changable at runtime (otherwise it's a #define)
#define CONFIG_MUTABLE_SCALE

// SDL_ttf support. if turned off SDL_ttf will not be required but only
// bitmap fonts will be supported.
#define CONFIG_ENABLE_TTF

// include the Doukutsu data-file extractor in the build
// (it's needed only the first time the program is run).
#define CONFIG_DATA_EXTRACTOR

// do image scaling in hardware via OpenGL
//#define CONFIG_OPENGL

#endif
