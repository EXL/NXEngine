
#include "../nx.h"
#include "../replay.h"
#include "options.h"
#include "dialog.h"
#include "message.h"
#include "../l10n_strings.h"

using namespace Options;

#include "options.fdh"

FocusStack optionstack;

#define SLIDE_SPEED				32
#define SLIDE_DIST				(3 * SLIDE_SPEED)

static struct
{
	Dialog *dlg, *subdlg;
	Dialog *dismiss_on_focus;
	int mm_cursel;
	bool InMainMenu;
	int xoffset;
	
	int selected_replay;
	int remapping_key, new_sdl_key;
} opt;


bool options_init(int retmode)
{
	memset(&opt, 0, sizeof(opt));
	Options::init_objects();
	opt.dlg = new Dialog;
	
	opt.xoffset = SLIDE_DIST;
	opt.dlg->offset(-SLIDE_DIST, 0);
	
	EnterMainMenu();
	opt.dlg->ondismiss = DialogDismissed;
	opt.dlg->ShowFull();

#if defined (_DINGUX) || defined (_MOTOMAGX) || defined (_MOTOEZX)
    inputs[OPTIONS_KEY] = 0;
#else
    inputs[F3KEY] = 0;
#endif

    sound(SND_MENU_MOVE);
	return 0;
}

void options_close()
{
	Options::close_objects();
	while(FocusHolder *fh = optionstack.ItemAt(0))
		delete fh;
	
	settings_save();
}

/*
void c------------------------------() {}
*/

void options_tick()
{
int i;
FocusHolder *fh;

#if defined (_DINGUX) || defined (_MOTOMAGX) || defined (_MOTOEZX)
    if (justpushed(OPTIONS_KEY))
#else
    if (justpushed(F3KEY))
#endif
	{
		game.pause(0);
		return;
	}
	
	ClearScreen(BLACK);
	Options::run_and_draw_objects();
	
	fh = optionstack.ItemAt(optionstack.CountItems() - 1);
	if (fh)
	{
		fh->RunInput();
		if (game.paused != GP_OPTIONS) return;
		
		fh = optionstack.ItemAt(optionstack.CountItems() - 1);
		if (fh == opt.dismiss_on_focus && fh)
		{
			opt.dismiss_on_focus = NULL;
			delete fh;
		}
	}
	
	for(i=0;;i++)
	{
		fh = optionstack.ItemAt(i);
		if (!fh) break;
		
		fh->Draw();
	}
	
	if (opt.xoffset > 0)
	{
		opt.dlg->offset(SLIDE_SPEED, 0);
		opt.xoffset -= SLIDE_SPEED;
	}
}


/*
void c------------------------------() {}
*/

void DialogDismissed()
{
	if (opt.InMainMenu)
	{
		memset(inputs, 0, sizeof(inputs));
		game.pause(false);
	}
	else
	{
		EnterMainMenu();
	}
}


/*
void c------------------------------() {}
*/

static void EnterMainMenu()
{
Dialog *dlg = opt.dlg;

	dlg->Clear();
	
    dlg->AddItem(LC_RES, _res_change, _res_get);
    dlg->AddItem(LC_KEYS, EnterControlsMenu);
    dlg->AddItem(LC_REPLAY, EnterReplayMenu);
	
	dlg->AddSeparator();
#if defined (_DINGUX) || defined (_MOTOMAGX) || defined (_MOTOEZX)
    dlg->AddItem(LC_DEBUG, EnterDebugMenu);
#else
    dlg->AddItem(LC_DEBUG_KEYS, _debug_change, _debug_get);
#endif
    dlg->AddItem(LC_SAVE_SLOTS, _save_change, _save_get);
	
	dlg->AddSeparator();
	
    dlg->AddItem(LC_MUSIC, _music_change, _music_get);
    dlg->AddItem(LC_SOUND, _sound_change, _sound_get);
	
	dlg->AddSeparator();
	dlg->AddDismissalItem();
	
	dlg->SetSelection(opt.mm_cursel);
	dlg->onclear = LeavingMainMenu;
	opt.InMainMenu = true;
}

void LeavingMainMenu()
{
	opt.mm_cursel = opt.dlg->GetSelection();
	opt.dlg->onclear = NULL;
	opt.InMainMenu = false;
}

void _res_get(ODItem *item)
{
	const char **reslist = Graphics::GetResolutions();
	
	if (settings->resolution < 0 || \
		settings->resolution >= count_string_list(reslist))
	{
		item->suffix[0] = 0;
	}
	else
    {
#if defined (_480X272) || defined (_320X240)
        strcpy(item->suffix, reslist[0]);
#else
        strcpy(item->suffix, reslist[settings->resolution]);
#endif
	}
}


void _res_change(ODItem *item, int dir)
{
const char **reslist = Graphics::GetResolutions();
int numres = count_string_list(reslist);
int newres;

	sound(SND_DOOR);
	
	newres = (settings->resolution + dir);
	if (newres >= numres) newres = 0;
	if (newres < 0) newres = (numres - 1);
	
	// because on my computer, a SDL bug causes switching to fullscreen to
	// not restore the resolution properly on exit, and it keeps messing up all
	// the windows when I press it accidently.
	if (newres == 0 && settings->inhibit_fullscreen)
	{
        new Message(LC_FULLSCREEN_DIS, LC_FULLSCREEN_CON);
		sound(SND_GUN_CLICK);
		return;
	}
#if (defined(_L10N_CP1251) && (!defined (_480X272) || !defined (_320X240))) || defined(__HAIKU__) // L10N Hack
    if (newres == 1) {
        new Message(LC_RES_FAILED_320);
        return;
    }
#endif
    if (!Graphics::SetResolution(newres, true))
	{
#if defined (_480X272) || defined (_320X240)
        settings->resolution = 0;
#else
        settings->resolution = newres;
#endif
	}
	else
	{
        new Message(LC_RES_FAILED);
		sound(SND_GUN_CLICK);
    }
}

/************ DEBUG MENU ************/
static void EnterDebugMenu(ODItem *item, int dir)
{
    Dialog *dlg = opt.dlg;
    dlg->Clear();

    sound(SND_MENU_MOVE);

    dlg->AddItem(LC_FPS, _fps_change, _fps_get);
    dlg->AddItem(LC_GODMODE, _godmode, _godmode_get);
    dlg->AddItem(LC_BOXES, _drawBoxes, _drawBoxes_get);

    dlg->AddSeparator();

    dlg->AddItem(LC_ADDEXP, _add_xp);
    dlg->AddItem(LC_SAVE_NOW, _save_now);
    // dlg->AddItem("Disable Debug Features", _disable_debug);

    dlg->AddSeparator();

    dlg->AddDismissalItem();
}

void _godmode(ODItem *item, int dir)
{
    if (!(settings->enable_debug_keys)) {
        settings->enable_debug_keys = true;
    }
    game.debug.god ^= 1;
    sound(SND_MENU_SELECT);
}

void _godmode_get(ODItem *item)
{
    static const char *strs[] = { "", " =" };
    strcpy(item->suffix, strs[game.debug.god]);
}

void _fps_change(ODItem *item, int dir)
{
    settings->show_fps ^= 1;
    sound(SND_MENU_SELECT);
}

void _fps_get(ODItem *item)
{
    static const char *strs[] = { "", " =" };
    strcpy(item->suffix, strs[settings->show_fps]);
}

void _save_now(ODItem *item, int dir)
{
    game_save(settings->last_save_slot);
    sound(SND_MENU_SELECT);
    console.Print("Game saved.");
}

void _drawBoxes(ODItem *item, int dir)
{
    if (!(settings->enable_debug_keys)) {
        settings->enable_debug_keys = true;
    }
    game.debug.DrawBoundingBoxes ^= 1;
    sound(SND_MENU_SELECT);
}

void _drawBoxes_get(ODItem *item)
{
    static const char *strs[] = { "", " =" };
    strcpy(item->suffix, strs[game.debug.DrawBoundingBoxes]);
}

void _add_xp(ODItem *item, int dir)
{
    AddXP(1);
}

void _disable_debug(ODItem *item, int dir)
{
    if(settings->enable_debug_keys) {
        game.debug.god = false;
        game.debug.DrawBoundingBoxes = false;
        settings->enable_debug_keys = false;
        settings->show_fps = false;
        sound(SND_MENU_SELECT);
    }
}

/************ END DEBUG MENU ************/

void _debug_change(ODItem *item, int dir)
{
	settings->enable_debug_keys ^= 1;
	sound(SND_MENU_SELECT);
}

void _debug_get(ODItem *item)
{
	static const char *strs[] = { "", " =" };
	strcpy(item->suffix, strs[settings->enable_debug_keys]);
}


void _save_change(ODItem *item, int dir)
{
	settings->multisave ^= 1;
	sound(SND_MENU_MOVE);
}

void _save_get(ODItem *item)
{
	static const char *strs[] = { "1", "5" };
	strcpy(item->suffix, strs[settings->multisave]);
}


void _sound_change(ODItem *item, int dir)
{
	settings->sound_enabled ^= 1;
	sound(SND_MENU_SELECT);
}

void _sound_get(ODItem *item)
{
    static const char *strs[] = { LC_OFF, LC_ON };
	strcpy(item->suffix, strs[settings->sound_enabled]);
}

void _music_change(ODItem *item, int dir)
{
	music_set_enabled((settings->music_enabled + 1) % 3);
	sound(SND_MENU_SELECT);
}

void _music_get(ODItem *item)
{
    static const char *strs[] = { LC_OFF, LC_ON, LC_BOSS_ONLY };
	strcpy(item->suffix, strs[settings->music_enabled]);
}

/*
void c------------------------------() {}
*/

static void EnterReplayMenu(ODItem *item, int dir)
{
Dialog *dlg = opt.dlg;
ReplaySlotInfo slot;
bool have_replays = false;

	dlg->Clear();
	sound(SND_MENU_MOVE);
	
	for(int i=0;i<MAX_REPLAYS;i++)
	{
		Replay::GetSlotInfo(i, &slot);
		
		if (slot.status != RS_UNUSED)
		{
			const char *mapname = map_get_stage_name(slot.hdr.stageno);
			dlg->AddItem(mapname, EnterReplaySubmenu, _upd_replay, i);
			have_replays = true;
		}
	}
	
	if (!have_replays)
        dlg->AddDismissalItem(LC_NO_RPL);
	
	dlg->AddSeparator();
	dlg->AddDismissalItem();
}

void _upd_replay(ODItem *item)
{
ReplaySlotInfo slot;

	Replay::GetSlotInfo(item->id, &slot);
	
	Replay::FramesToTime(slot.hdr.total_frames, &item->raligntext[1]);
	item->raligntext[0] = (slot.hdr.locked) ? '=':' ';
}

/*
void c------------------------------() {}
*/

void EnterReplaySubmenu(ODItem *item, int dir)
{
	opt.selected_replay = item->id;
	sound(SND_MENU_MOVE);
	
	opt.subdlg = new Dialog;
	opt.subdlg->SetSize(80, 60);
	
    opt.subdlg->AddItem(LC_PLAY, _play_replay);
    opt.subdlg->AddItem(LC_KEEP, _keep_replay);
}


void _keep_replay(ODItem *item, int dir)
{
char fname[MAXPATHLEN];
ReplayHeader hdr;

	GetReplayName(opt.selected_replay, fname);
	
	if (Replay::LoadHeader(fname, &hdr))
	{
        new Message(LC_FAIL_LOAD_HEADER);
		sound(SND_GUN_CLICK);
		opt.dismiss_on_focus = opt.subdlg;
		return;
	}
	
	hdr.locked ^= 1;
	
	if (Replay::SaveHeader(fname, &hdr))
	{
        new Message(LC_FAIL_WRITE_HEADER);
		sound(SND_GUN_CLICK);
		opt.dismiss_on_focus = opt.subdlg;
		return;
	}
	
	sound(SND_MENU_MOVE);
	opt.subdlg->Dismiss();
	opt.dlg->Refresh();
}


void _play_replay(ODItem *item, int dir)
{
	game.switchstage.param = opt.selected_replay;
	game.switchmap(START_REPLAY);
	
	game.setmode(GM_NORMAL);
	game.pause(false);
}

/*
void c------------------------------() {}
*/

static void EnterControlsMenu(ODItem *item, int dir)
{
Dialog *dlg = opt.dlg;

	dlg->Clear();
	sound(SND_MENU_MOVE);
	
    dlg->AddItem(LC_KB_LEFT, _edit_control, _upd_control, LEFTKEY);
    dlg->AddItem(LC_KB_RIGHT, _edit_control, _upd_control, RIGHTKEY);
    dlg->AddItem(LC_KB_UP, _edit_control, _upd_control, UPKEY);
    dlg->AddItem(LC_KB_DOWN, _edit_control, _upd_control, DOWNKEY);
	
#ifndef _L10N_CP1251
	dlg->AddSeparator();
#endif
	
    dlg->AddItem(LC_KB_JUMP, _edit_control, _upd_control, JUMPKEY);
    dlg->AddItem(LC_KB_FIRE, _edit_control, _upd_control,  FIREKEY);
    dlg->AddItem(LC_KB_WPN_PREV, _edit_control, _upd_control, PREVWPNKEY);
    dlg->AddItem(LC_KB_WPN_NEXT, _edit_control, _upd_control, NEXTWPNKEY);
    dlg->AddItem(LC_KB_INV, _edit_control, _upd_control, INVENTORYKEY);
    dlg->AddItem(LC_KB_MAP, _edit_control, _upd_control, MAPSYSTEMKEY);
	
#ifndef _L10N_CP1251
	dlg->AddSeparator();
#endif
	dlg->AddDismissalItem();
}

static void _upd_control(ODItem *item)
{
	int keysym = input_get_mapping(item->id);
	const char *keyname = SDL_GetKeyName((SDLKey)keysym);
	
	maxcpy(item->righttext, keyname, sizeof(item->righttext) - 1);
}

static void _edit_control(ODItem *item, int dir)
{
Message *msg;

	opt.remapping_key = item->id;
	opt.new_sdl_key = -1;
	
    msg = new Message(LC_KB_NEW_KEY, input_get_name(opt.remapping_key));
	msg->rawKeyReturn = &opt.new_sdl_key;
	msg->on_dismiss = _finish_control_edit;
	
	sound(SND_DOOR);
}

static void _finish_control_edit(Message *msg)
{
int inputno = opt.remapping_key;
int new_sdl_key = opt.new_sdl_key;
int i;

	if (input_get_mapping(inputno) == new_sdl_key)
	{
		sound(SND_MENU_MOVE);
		return;
	}
	
	// check if key is already in use
	for(i=0;i<INPUT_COUNT;i++)
	{
		if (i != inputno && input_get_mapping(i) == new_sdl_key)
		{
            new Message(LC_KB_BUSY, input_get_name(i));
			sound(SND_GUN_CLICK);
			return;
		}
	}
	
	input_remap(inputno, new_sdl_key);
	sound(SND_MENU_SELECT);
	opt.dlg->Refresh();
}
