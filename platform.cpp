
#include <SDL.h>
#include <stdio.h>
#include "config.h"
#include "platform.fdh"

#ifndef __SDLSHIM__

FILE *fileopen(const char *fname, const char *mode)
{
	return fopen(fname, mode);
}

#else

FILE *fileopen(const char *fname, const char *mode)
{
	return SDLS_fopen(fname, mode);
}

#endif

