
#include "nx.h"
#include <stdarg.h>
#include "console.fdh"

static CommandEntry commands[] =
{
	"god", __god, 0, 1,
	"script", __script, 1, 1,
	"warp", __warp, 1, 999,
	"sound", __sound, 1, 1,
	"music", __music, 1, 1,
	"giveweapon", __giveweapon, 1, 1,
	"dropweapon", __dropweapon, 0, 1,
	"level", __level, 1, 1,
	"ammo", __ammo, 1, 1,
	"maxammo", __maxammo, 1, 1,
	"hp", __hp, 1, 1,
	"maxhp", __maxhp, 1, 1,
	"xp", __xp, 1, 1,
	"spawn", __spawn, 1, 999,
	"animate", __animate, 1, 2,
	"infinitedamage", __infinitedamage, 0, 1,
	"killall", __killall, 0, 0,
	"movemode", __movemode, 1, 1,
	"flag", __flag, 1, 1,
	"clearflags", __clearflags, 0, 0,
	"equip", __equip, 1, 2,
	"giveitem", __giveitem, 1, 1,
	"takeitem", __takeitem, 1, 1,
	"qua", __qua, 0, 1,
	"boa", __boa, 1, 1,
	"cre", __cre, 0, 0,
	"reset", __reset, 0, 0,
	"fps", __fps, 0, 1,
	
	"instant-quit", __set_iquit, 1, 1,
	"no-quake-in-hell", __set_noquake, 1, 1,
	"inhibit-fullscreen", __inhibit_fullscreen, 1, 1,
	"emulate-bugs", __emulate_bugs, 1, 1,
	"displayformat", __displayformat, 1, 1,
	"skip-intro", __skip_intro, 1, 1,
	
	"player->hide", __player_hide, 1, 1,
	"player->inputs_locked", __player_inputs_locked, 1, 1,
	"game.frozen", __game_frozen, 1, 1,
	"textbox.SetVisible", __textbox_setvisible, 1, 1,
	
	"hello", __hello, 0, 0,
	"hi", __hello, 0, 0,
	
	NULL, NULL
};


DebugConsole::DebugConsole()
{
	fVisible = false;
	fLineLen = 0;
	fCursorTimer = 0;
	fResponseTimer = 0;
	
	fLine[0] = 0;
	fResponse[0] = 0;
	
	fKeyDown = 0;
	fRepeatTimer = 0;
}

/*
void c------------------------------() {}
*/

void DebugConsole::SetVisible(bool newstate)
{
	//stat("DebugConsole::SetVisible(%s)", newstate?"true":"false");
	
	if (fVisible != newstate)
	{
		fVisible = newstate;
		fKeyDown = 0;
		fRepeatTimer = 0;
		
		if (newstate)
		{
			fLine[0] = 0;
			fLineLen = 0;
			fBrowsingExpansion = false;
			fBackIndex = fBackBuffer.CountItems();
			
			fResponse[0] = 0;
			fCursorTimer = 0;
		}
	}
}

bool DebugConsole::IsVisible()
{
	return fVisible;
}

bool DebugConsole::HandleKey(int key)
{
	if (!fVisible) return 0;
	if (key != 9) fBrowsingExpansion = false;
	
	if (key != fKeyDown)
	{
		fKeyDown = key;
		fRepeatTimer = 25;
	}
	else
	{
		fRepeatTimer = 1;
	}
	
	fCursorTimer = 0;

	switch(key)
	{
		case 27:
		case '`':
		{
			SetVisible(false);
		}
		break;
		
		case 13:
		case 271:	// numeric enter
		{
			SetVisible(false);
			
			fLine[fLineLen] = 0;
			Execute(fLine);
			fLineLen = 0;
		}
		break;
		
		case 10: break;
		
		case 8:
		{
			if (fLineLen > 0)
				fLineLen--;
		}
		break;
		
		case 9:		// command completion
		{
			fLine[fLineLen] = 0;
			ExpandCommand();
			
			fBrowsingExpansion = true;
			fExpandIndex++;
		}
		break;
		
		// command backbuffer
		case SDLK_UP:
		case SDLK_DOWN:
		{
			if (fBackBuffer.CountItems() > 0)
			{
				fBackIndex += (key == SDLK_UP) ? -1 : 1;
				if (fBackIndex < 0) fBackIndex = (fBackBuffer.CountItems() - 1);
				else fBackIndex %= fBackBuffer.CountItems();
				
				const char *str = fBackBuffer.StringAt(fBackIndex);
				
				maxcpy(fLine, str, sizeof(fLine) - 1);
				fLineLen = strlen(str);
			}
		}
		break;
		
		default:
		{
			if (fLineLen < (sizeof(fLine) - 1))
				fLine[fLineLen++] = key;
		}
		break;
	}
	
	return 1;
}

void DebugConsole::HandleKeyRelease(int key)
{
	if (key == fKeyDown)
	{
		fKeyDown = 0;
		fRepeatTimer = 0;
	}
}

/*
void c------------------------------() {}
*/

void DebugConsole::Draw()
{
	if (fResponse[0])
	{
		this->DrawText(fResponse);
		
		if (--fResponseTimer <= 0)
			fResponse[0] = 0;
	}
	else if (fVisible)
	{
		// key-repeat
		if (fKeyDown)
		{
			if (--fRepeatTimer < 0)
				HandleKey(fKeyDown);
		}
		
		char buffer[CONSOLE_MAXCMDLEN + 10];
		fLine[fLineLen] = 0;
		
		sprintf(buffer, "-> %s%c",
			fLine, (fCursorTimer < 20) ? '_' : ' ');
		
		this->DrawText(buffer);
		
		if (++fCursorTimer > 30)
			fCursorTimer = 0;
	}
}

void DebugConsole::DrawText(const char *text)
{
	font_draw_shaded(4, (SCREEN_HEIGHT - 16), text);
}

/*
void c------------------------------() {}
*/

void DebugConsole::Print(const char *fmt, ...)
{
va_list ar;

	va_start(ar, fmt);
	vsnprintf(fResponse, sizeof(fResponse), fmt, ar);
	va_end(ar);
	
	stat("%s", fResponse);
	fResponseTimer = 60;
}

/*
void c------------------------------() {}
*/

bool DebugConsole::Execute(const char *line)
{
	stat("DebugConsole::Execute('%s')", line);
	
	// record command in backbuffer
	if (fBackBuffer.CountItems() >= CONSOLE_MAX_BACK)
		fBackBuffer.RemoveString(0);
	fBackBuffer.AddString(line);
	
	// split command into arguments
	StringList args;
	char *cmd = SplitCommand(line, &args);
	
	if (cmd)
	{
		BList matches;
		MatchCommand(cmd, &matches);
		free(cmd);
		
		if (matches.CountItems() == 1)
		{
			CommandEntry *command = (CommandEntry *)matches.ItemAt(0);
			
			if (args.CountItems() < command->minArgs || \
				args.CountItems() > command->maxArgs)
			{
				if (command->minArgs == command->maxArgs)
				{
					Print("'%s' requires %d argument%s", \
						command->name, command->minArgs, \
						(command->minArgs == 1) ? "":"s");
				}
				else if (args.CountItems() < command->minArgs)
				{
					Print("'%s' requires at least %d argument%s",
						command->name, command->minArgs,
						(command->minArgs == 1) ? "":"s");
				}
				else
				{
					Print("'%s' requires no more than %d arguments",
						command->name, command->maxArgs);
				}
			}
			else
			{
				void (*handler)(StringList *, int) = command->handler;
				int num = (args.CountItems() > 0) ? atoi(args.StringAt(0)) : 0;
				
				(*handler)(&args, num);
				return 1;
			}
		}
		else if (matches.CountItems() == 0)
		{
			Print("I don't understand");
		}
		else
		{
			Print("Ambiguous command");
		}
	}
	
	return 0;
}

void DebugConsole::MatchCommand(const char *cmd, BList *matches)
{
	for(int i=0; commands[i].name; i++)
	{
		if (strcasebegin(commands[i].name, cmd))
			matches->AddItem(&commands[i]);
	}
}

// split an input line into command and arguments
// returns the command portion of the line. you must free this buffer.
char *DebugConsole::SplitCommand(const char *line_in, StringList *args)
{
	while(*line_in == ' ' || *line_in == '\t') line_in++;
	char *line = strdup(line_in);
	
	char *cmd = strtok(line, " \t");
	if (cmd && cmd[0])
	{
		while(const char *arg = strtok(NULL, " \t"))
		{
			args->AddString(arg);
		}
		
		return line;
	}
	
	free(line);
	return NULL;
}

// tab-expand the current command
void DebugConsole::ExpandCommand()
{
StringList args;
BList matches;
char *cmd;

	fLine[fLineLen] = 0;
	
	if (!fBrowsingExpansion)
	{
		maxcpy(fLineToExpand, fLine, sizeof(fLineToExpand));
		fExpandIndex = 0;
	}
	
	cmd = SplitCommand(fLineToExpand, &args);
	if (cmd)
	{
		MatchCommand(cmd, &matches);
		free(cmd);
		
		if (matches.CountItems() > 0)
		{
			if (fExpandIndex >= matches.CountItems())
				fExpandIndex = 0;
			
			CommandEntry *command = (CommandEntry *)matches.ItemAt(fExpandIndex);
			DString newCommand(command->name);
			
			for(int i=0;;i++)
			{
				const char *arg = args.StringAt(i);
				if (!arg) break;
				
				newCommand.AppendChar(' ');
				newCommand.AppendString(arg);
			}
			
			if (args.CountItems() < command->minArgs)
				newCommand.AppendChar(' ');
			
			maxcpy(fLine, newCommand.String(), sizeof(fLine));
			fLineLen = strlen(fLine);
		}
	}
	
	if (matches.CountItems() != 1)
		sound(SND_TINK);
}

/*
void c------------------------------() {}
*/

#define Respond		console.Print


static void __god(StringList *args, int num)
{
bool enable;

	if (args->CountItems() == 0)
		enable = true;
	else
		enable = num;
	
	game.debug.god = enable;
}

static void __script(StringList *args, int num)
{
	// release any focus a current script may have on us
	if (player->movementmode == MOVEMODE_NORMAL)
		map_focus(NULL);
	
	if (StartScript(num))
	{
		Respond("Script %04d started.", num);
	}
	else
	{
		Respond("No such script %04d", num);
	}
}

static void __warp(StringList *args, int num)
{
	if (num == 0)
	{
		DString stagename;
		for(int i=0;i<args->CountItems();i++)
		{
			if (i != 0) stagename.AppendChar(' ');
			stagename.AppendString(args->StringAt(i));
		}
		
		stat("Looking for '%s'", stagename.String());
		for(num=0;;num++)
		{
			if (num >= num_stages)
			{
				if (!strcasecmp(stagename.String(), "village"))
				{
					num = 11;
				}
				else
				{
					Respond("Could determine stage number from your description.");
					return;
				}
				
				break;
			}
			
			if (strcasebegin(stages[num].stagename, stagename.String()))
				break;
		}
	}
	
	game.switchstage.mapno = num;
	game.switchstage.playerx = 16;
	game.switchstage.playery = 16;
}

static void __sound(StringList *args, int num)
{
	sound(num);
	console.SetVisible(true);	// keep console up
}

static void __music(StringList *args, int num)
{
	extern const char *org_names[];
	bool ok = true;
	int i;
	
	const char *name = args->StringAt(0);
	if (num == 0 && strcmp(name, "0") != 0)
	{
		for(i=1;;i++)
		{
			if (!org_names[i]) break;
			
			if (strcasebegin(org_names[i], name))
			{
				num = i;
				break;
			}
		}
		
		if (num == 0)
		{
			Respond("Don't know that song.");
			return;
		}
	}
	
	if (num < 0) ok = false;
	else
	{
		for(i=1;i<=num;i++)
		{
			if (!org_names[i])
			{
				ok = false;
				break;
			}
		}
	}
	
	if (!ok)
	{
		Respond("track out of range");
		music(0);
	}
	else
	{
		music(0);
		music(num);
		if (org_names[num])
			Respond("%s started", org_names[num]);
	}
}

static void __giveweapon(StringList *args, int num)
{
	if (num >= 0 && num < WPN_COUNT)
	{
		player->weapons[num].hasWeapon = 1;
		player->weapons[num].maxammo = 0;		// gives it unlimited ammo
		player->weapons[num].ammo = 0;
		player->curWeapon = num;
	}
}

static void __dropweapon(StringList *args, int num)
{
	if (args->CountItems() == 0)
		num = player->curWeapon;
	
	player->weapons[num].hasWeapon = 0;
	player->weapons[num].maxammo = 0;
	player->weapons[num].ammo = 0;
	
	if (num == player->curWeapon)
		stat_NextWeapon();
}

// set weapon level
static void __level(StringList *args, int num)
{
	num--;
	if (num < 0) num = 0;
	if (num > 2) num = 2;
	
	if (player->weapons[player->curWeapon].xp < 5)
		player->weapons[player->curWeapon].xp = 5;
	
	for(int timeout=0;timeout<500;timeout++)
	{
		if (player->weapons[player->curWeapon].level == num)
		{
			return;
		}
		else if (player->weapons[player->curWeapon].level < num)
		{
			AddXP(1);
		}
		else
		{
			SubXP(1);
		}
	}
	
	Respond("Timeout");
}


static void __ammo(StringList *args, int num)
{
	player->weapons[player->curWeapon].ammo = num;
	if (player->weapons[player->curWeapon].ammo > player->weapons[player->curWeapon].maxammo)
		player->weapons[player->curWeapon].maxammo = player->weapons[player->curWeapon].ammo;
}

static void __maxammo(StringList *args, int num)
{
	player->weapons[player->curWeapon].maxammo = num;
	if (player->weapons[player->curWeapon].ammo > player->weapons[player->curWeapon].maxammo)
		player->weapons[player->curWeapon].ammo = player->weapons[player->curWeapon].maxammo;
}

static void __hp(StringList *args, int num)
{
	player->hp = num;
	if (player->hp > player->maxHealth)
		player->maxHealth = player->hp;
}

static void __maxhp(StringList *args, int num)
{
	player->maxHealth = num;
	if (player->hp > player->maxHealth)
	{
		player->hp = player->maxHealth;
		//PHealthBar.displayed_value = player->hp;
	}
}

static void __xp(StringList *args, int num)
{
	player->weapons[player->curWeapon].xp = num;
}

static void __spawn(StringList *args, int num)
{
	int i = 0;

	// if first argument is a number interpret it as a count of
	// objects to spawn.
	int count;
	if (isdigit(args->StringAt(0)[0]))
	{
		count = num;
		i++;
	}
	else
	{
		count = 1;
	}
	
	// reconstitute the arguments into the name of the object
	// to be spawned.
	DString objName;
	int starti = i;
	for(;;i++)
	{
		if (!args->StringAt(i)) break;
		
		if (i > starti) objName.AppendChar(' ');
		objName.AppendString(args->StringAt(i));
	}
	
	// try and get object type from the provided name
	int type = ObjectNameToType(objName.String());
	if (type == -1)
	{
		Respond("Unknown object. See object.h for definitions.");
		return;
	}

	// reset console animate flags on any previously spawned objects
	Object *o;
	FOREACH_OBJECT(o)
	{
		o->nxflags &= ~NXFLAG_CONSOLE_ANIMATE;
	}
	
	// get starting spawn position and spacing
	int x = player->x + ((player->dir==RIGHT) ? (24 << CSF) : -(24 << CSF));
	int y = player->y - (16 << CSF);
	int w = (sprites[objprop[type].sprite].w + 4) << CSF;
	
	// create 'em
	for(i=0;i<count;i++)
	{
		Object *o = CreateObject(x, y, type);
		
		o->dir = player->dir;
		o->nxflags |= NXFLAG_CONSOLE_ANIMATE;
		x += w;
	}
	
	if (count != 1)
		Respond("%s x%d", DescribeObjectType(type), count);
	else
		Respond("%s", DescribeObjectType(type));
}

static void __animate(StringList *args, int num)
{
	Object *o;
	
	if (args->CountItems() == 2)
	{	// specifying explicitly by id2
		o = FindObjectByID2(atoi(args->StringAt(0)));
		if (o)
			o->state = atoi(args->StringAt(1));
		else
			Respond("Object not found.");
		
		return;
	}
	
	// animating implicitly from last spawn command
	bool found = false;
	FOREACH_OBJECT(o)
	{
		if (o->nxflags & NXFLAG_CONSOLE_ANIMATE)
		{
			o->state = num;
			found = true;
		}
	}
	
	if (!found)
		Respond("No objects found.");
}

static void __infinitedamage(StringList *args, int num)
{
	if (args->CountItems() > 0)
		game.debug.infinite_damage = num;
	else
		game.debug.infinite_damage ^= 1;
	
	Respond(game.debug.infinite_damage ? "My, oh my..." : "Back to normal.");
}

static void __killall(StringList *args, int num)
{
	for(int i=0;i<nOnscreenObjects;i++)
	{
		Object *o = onscreen_objects[i];
		if (o->flags & FLAG_SHOOTABLE)
		{
			o->flags &= ~FLAG_INVULNERABLE;
			o->DealDamage(999);
		}
	}
}

static void __movemode(StringList *args, int num)
{
	player->movementmode = num;
}

static void __flag(StringList *args, int num)
{
	game.flags[num] ^= 1;
	Respond("Flag %04d: %s", num, game.flags[num] ? "SET":"CLEARED");
}

static void __clearflags(StringList *args, int num)
{
	memset(game.flags, 0, sizeof(game.flags));
	Respond("Warning- all game flags cleared");
}

static void __equip(StringList *args, int num)
{
static const char *equiplist[] =
{
	"booster08",
	"map",
	"armsbarrier",
	"turbocharge",
	"airtank",
	"booster20",
	"mimigamask",
	"whimstar",
	"nikumaru",
	NULL
};
int i, mask;

	const char *item = args->StringAt(0);
	bool enable = args->StringAt(1) ? atoi(args->StringAt(1)) : true;
	
	mask = 0x01;
	for(i=0;equiplist[i];i++)
	{
		if (!strcasecmp(equiplist[i], item))
		{
			// allow only booster 08 or booster 20 at a time
			if (mask & (EQUIP_BOOSTER08 | EQUIP_BOOSTER20))
				player->equipmask &= ~(EQUIP_BOOSTER08 | EQUIP_BOOSTER20);
			
			if (enable)
				player->equipmask |= mask;
			else
				player->equipmask &= ~mask;
			
			Respond("Item %s (0x%04x) %sequipped.",
				equiplist[i], mask, enable ? "" : "un-");
			return;
		}
		
		mask <<= 1;
	}
	
	Respond("Unknown item");
}

static void __giveitem(StringList *args, int num)
{
	if (FindInventory(num) == -1)
	{
		AddInventory(num);
		Respond("Added item %d to your inventory.", num);
	}
	else
	{
		Respond("You already have item %d in your inventory.", num);
	}
}

static void __takeitem(StringList *args, int num)
{
	if (FindInventory(num) != -1)
	{
		DelInventory(num);
		Respond("Removed item %d from your inventory.", num);
	}
	else
	{
		Respond("You don't have item %d in your inventory.", num);
	}
}

static void __qua(StringList *args, int num)
{
	if (args->CountItems() > 0)
		megaquake(50);
	else
		quake(50);
}

static void __boa(StringList *args, int num)
{
	game.stageboss.SetState(num);
}

// skip to good ending sequence
static void __cre(StringList *args, int num)
{
	game.reset();
	game.pause(0);
	game.setmode(GM_NORMAL);
	
	game.flags[1341] = true;
	game.switchstage.mapno = 70;
	game.switchstage.playerx = 16;
	game.switchstage.playery = 16;
	game.switchstage.eventonentry = 400;
}

static void __reset(StringList *args, int num)
{
	game.reset();
}

static void __fps(StringList *args, int num)
{
extern int fps;

	settings->show_fps ^= 1;
	settings_save();
	fps = 0;
}

/*
void c------------------------------() {}
*/

static void __set_iquit(StringList *args, int num)
{
	settings->instant_quit = num;
	settings_save();
	Respond("instant quit: %s", settings->instant_quit ? "enabled":"disabled");
}

static void __set_noquake(StringList *args, int num)
{
	settings->no_quake_in_hell = num;
	settings_save();
	Respond("no quake in hell: %s", settings->no_quake_in_hell ? "enabled":"disabled");
}

static void __inhibit_fullscreen(StringList *args, int num)
{
	settings->inhibit_fullscreen = num;
	settings_save();
	Respond("inhibit fullscreen: %s", settings->inhibit_fullscreen ? "enabled":"disabled");
}

static void __emulate_bugs(StringList *args, int num)
{
	settings->emulate_bugs = num;
	settings_save();
	Respond("emulate bugs: %s", settings->emulate_bugs ? "enabled":"disabled");
}

static void __displayformat(StringList *args, int num)
{
	settings->displayformat = num;
	settings_save();
	Graphics::FlushAll();
	Respond("SDL_DisplayFormat: %s", settings->displayformat ? "on":"off");
}

static void __skip_intro(StringList *args, int num)
{
	settings->skip_intro = num;
	settings_save();
	Respond("skip_intro: %s", settings->skip_intro ? "enabled":"disabled");
}

/*
void c------------------------------() {}
*/

static void __hello(StringList *args, int num)
{
	Respond("I'm a computer, you ninny. Go get a real friend.");
}

/*
void c------------------------------() {}
*/

static void __player_hide(StringList *args, int num)
{
	player->hide = num;
}

static void __player_inputs_locked(StringList *args, int num)
{
	player->inputs_locked = num;
}

static void __game_frozen(StringList *args, int num)
{
	game.frozen = num;
}

static void __textbox_setvisible(StringList *args, int num)
{
	textbox.SetVisible(num);
}

