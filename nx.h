
#ifndef _NX_H
#define _NX_H

#include <SDL/SDL.h>
#include <stdio.h>
#include <ctype.h>
//#include <sys/param.h>
//#include <limits.h>

#include "config.h"
#include "common/basics.h"
#include "common/BList.h"
#include "common/StringList.h"
#include "common/DBuffer.h"
#include "common/DString.h"
#include "common/InitList.h"

#include "graphics/graphics.h"
#include "graphics/tileset.h"
#include "graphics/sprites.h"
typedef SIFPoint	Point;
using namespace Graphics;
using Sprites::draw_sprite;
using Sprites::draw_sprite_at_dp;
using Sprites::draw_sprite_clipped;
using Sprites::draw_sprite_clip_width;
using Sprites::draw_sprite_chopped;
using Sprites::draw_sprite_repeating_x;
using Sprites::create_spritesheet;
using Sprites::get_spritesheet;
using Tileset::draw_tile;

#define CSF				9

class Object;

// don't use this, use fileopen() instead.
// some platforms are retarded and need special workarounds (read: WinCE)
#pragma	GCC poison		fopen
FILE *fileopen(const char *fname, const char *mode);

#include "trig.h"
#include "autogen/sprites.h"
#include "dirnames.h"
#include "TextBox/TextBox.h"
#include "graphics/font.h"

#include "input.h"
#include "tsc.h"
#include "stageboss.h"
#include "ai/ai.h"
#include "map.h"
#include "statusbar.h"
#include "floattext.h"
#include "object.h"
#include "ObjManager.h"
#include "console.h"
#include "debug.h"
#include "game.h"
#include "caret.h"
#include "screeneffect.h"
#include "settings.h"
#include "slope.h"
#include "player.h"
#include "p_arms.h"
#include "replay.h"
#include "platform.h"

#include "sound/sound.h"

const char *strhex(int value);

#ifdef __HAIKU__
char *getHaikuSettingsPath();
#endif

#endif
