#ifndef L10N_STRINGS_H
#define L10N_STRINGS_H

#ifdef _L10N_CP1251
// Russian Win1251 Strings
const char LC_RES[] =               "\xD0\xE0\xE7\xF0\xE5\xF8\xE5\xED\xE8\xE5" ": ";
const char LC_KEYS[] =              "\xD3\xEF\xF0\xE0\xE2\xEB\xE5\xED\xE8\xE5";
const char LC_REPLAY[] =            "\xCF\xEE\xE2\xF2\xEE\xF0\xFB";
const char LC_DEBUG[] =             "\xCC\xE5\xED\xFE" " " "\xEE\xF2\xEB\xE0\xE4\xEA\xE8";
const char LC_DEBUG_KEYS[] =        "\xC2\xEA\xEB\xFE\xF7\xE8\xF2\xFC" " Debug-" "\xEA\xEB\xE0\xE2\xE8\xF8\xE8";
const char LC_SAVE_SLOTS[] =        "\xD1\xEB\xEE\xF2\xFB" " " "\xF1\xEE\xF5\xF0\xE0\xED\xE5\xED\xE5\xE9" ": ";
const char LC_MUSIC[] =             "\xCC\xF3\xE7\xFB\xEA\xE0" ": ";
const char LC_SOUND[] =             "\xC7\xE2\xF3\xEA" ": ";

const char LC_FULLSCREEN_DIS[] =    "\xCF\xEE\xEB\xED\xEE\xFD\xEA\xF0\xE0\xED\xED\xFB\xE9" " "
                                    "\xF0\xE5\xE6\xE8\xEC" " " "\xEE\xF2\xEA\xEB\xFE\xF7\xE5\xED";
const char LC_FULLSCREEN_CON[] =    "\xF7\xE5\xF0\xE5\xE7" " " "\xEA\xEE\xED\xF1\xEE\xEB\xFC";
const char LC_RES_FAILED_320[] =    "\xCE\xF8\xE8\xE1\xEA\xE0" " "
                                    "\xE8\xE7\xEC\xE5\xED\xE5\xED\xE8\xFF" " "
                                    "\xF0\xE0\xE7\xF0\xE5\xF8\xE5\xED\xE8\xFF" " "
                                    "\xED\xE0" " " "320x240";
const char LC_RES_FAILED[] =        "\xCE\xF8\xE8\xE1\xEA\xE0" " "
                                    "\xE8\xE7\xEC\xE5\xED\xE5\xED\xE8\xFF" " "
                                    "\xF0\xE0\xE7\xF0\xE5\xF8\xE5\xED\xE8\xFF" " ";

const char LC_FPS[] =               "\xCF\xEE\xEA\xE0\xE7\xE0\xF2\xFC" " " "FPS";
const char LC_GODMODE[] =           "\xC2\xEA\xEB\xFE\xF7\xE8\xF2\xFC" " " "\xED\xE5\xF3\xFF\xE7\xE2\xE8\xEC\xEE\xF1\xF2\xFC";
const char LC_BOXES[] =             "\xD0\xE8\xF1\xEE\xE2\xE0\xF2\xFC" " " "\xE1\xEE\xEA\xF1\xFB";
const char LC_ADDEXP[] =            "\xC4\xEE\xE1\xE0\xE2\xE8\xF2\xFC" " +1 XP";
const char LC_SAVE_NOW[] =          "\xD1\xEE\xF5\xF0\xE0\xED\xE8\xF2\xFC" " " "\xE8\xE3\xF0\xF3" " " "\xF1\xE5\xE9\xF7\xE0\xF1";

const char LC_ON[] =                "\xC2\xEA\xEB";
const char LC_OFF[] =               "\xC2\xFB\xEA\xEB";
const char LC_BOSS_ONLY[] =         "\xD2\xEE\xEB\xFC\xEA\xEE" " " "\xED\xE0" " " "\xE1\xEE\xF1\xF1\xE0\xF5";

const char LC_NO_RPL[] =            "[" "\xED\xE5\xF2" " " "\xEF\xEE\xE2\xF2\xEE\xF0\xEE\xE2" "]";
const char LC_PLAY[] =              "\xC8\xE3\xF0\xE0\xF2\xFC";
const char LC_KEEP[] =              "\xD1\xEE\xF5\xF0\xE0\xED\xE8\xF2\xFC";
const char LC_FAIL_LOAD_HEADER[] =  "\xCE\xF8\xE8\xE1\xEA\xE0" " "
                                    "\xE7\xE0\xE3\xF0\xF3\xE7\xEA\xE8" " "
                                    "\xE7\xE0\xE3\xEE\xEB\xEE\xE2\xEA\xE0" " ";

const char LC_FAIL_WRITE_HEADER[] = "\xCE\xF8\xE8\xE1\xEA\xE0" " "
                                    "\xE7\xE0\xEF\xE8\xF1\xE8" " "
                                    "\xE7\xE0\xE3\xEE\xEB\xEE\xE2\xEA\xE0" ".";

const char LC_KB_LEFT[] =           "\xC2\xEB\xE5\xE2\xEE";
const char LC_KB_RIGHT[] =          "\xC2\xEF\xF0\xE0\xE2\xEE";
const char LC_KB_UP[] =             "\xC2\xE2\xE5\xF0\xF5";
const char LC_KB_DOWN[] =           "\xC2\xED\xE8\xE7";
const char LC_KB_JUMP[] =           "\xCF\xF0\xFB\xE6\xEE\xEA";
const char LC_KB_FIRE[] =           "\xCE\xE3\xEE\xED\xFC";
const char LC_KB_WPN_PREV[] =       "\xCF\xF0\xE5\xE4" ". "
                                    "\xEE\xF0\xF3\xE6\xE8\xE5";
const char LC_KB_WPN_NEXT[] =       "\xD1\xEB\xE5\xE4" ". "
                                    "\xEE\xF0\xF3\xE6\xE8\xE5";
const char LC_KB_INV[] =            "\xC8\xED\xE2\xE5\xED\xF2\xE0\xF0\xFC";
const char LC_KB_MAP[] =            "\xCA\xE0\xF0\xF2\xE0";
const char LC_KB_NEW_KEY[] =        "\xCD\xE0\xE6\xEC\xE8\xF2\xE5" " "
                                    "\xED\xEE\xE2\xF3\xFE" " "
                                    "\xEA\xEB\xE0\xE2\xE8\xF8\xF3" " "
                                    "\xE4\xEB\xFF" ":";
const char LC_KB_BUSY[] =           "\xCA\xEB\xE0\xE2\xE8\xF8\xE0" " "
                                    "\xF3\xE6\xE5" " "
                                    "\xE8\xF1\xEF\xEE\xEB\xFC\xE7\xF3\xE5\xF2\xF1\xFF" " "
                                    "\xED\xE0" ":";

const char LC_RETURN[] =            "\xC2\xEE\xE7\xE2\xF0\xE0\xF2";
const char LC_FULLSCREEN[] =        "\xCF\xEE\xEB\xED\xFB\xE9" " " "\xFD\xEA\xF0\xE0\xED";

const char LC_PS_QUIT[] =           ":" "\xC2\xFB\xF5\xEE\xE4";
const char LC_PS_RESUME[] =         ":" "\xC2\xEE\xE7\xE2\xF0\xE0\xF2";
const char LC_PS_RESET[] =          ":" "\xD1\xE1\xF0\xEE\xF1";
const char LC_PS_OPTIONS[] =        ":" "\xCE\xEF\xF6\xE8\xE8";
#else
const char LC_RES[] = "Resolution: ";
const char LC_KEYS[] = "Controls";
const char LC_REPLAY[] = "Replay";
const char LC_DEBUG[] = "Debug Menu";
const char LC_DEBUG_KEYS[] = "Enable Debug Keys";
const char LC_SAVE_SLOTS[] = "Save Slots: ";
const char LC_MUSIC[] = "Music: ";
const char LC_SOUND[] = "Sound: ";

const char LC_FULLSCREEN_DIS[] = "Fullscreen disabled via";
const char LC_FULLSCREEN_CON[] = "inhibit-fullscreen console setting";
const char LC_RES_FAILED_320[] = "Failed change resolution to 320x240";
const char LC_RES_FAILED[] = "Resolution change failed";

const char LC_FPS[] = "Show FPS";
const char LC_GODMODE[] = "Enable GodMode";
const char LC_BOXES[] = "Draw Boxes";
const char LC_ADDEXP[] = "Add +1 XP";
const char LC_SAVE_NOW[] = "Save now";

const char LC_ON[] = "On";
const char LC_OFF[] = "Off";
const char LC_BOSS_ONLY[] = "Boss Only";

const char LC_NO_RPL[] = "[no replays yet]";
const char LC_PLAY[] = "Play";
const char LC_KEEP[] = "Keep";
const char LC_FAIL_LOAD_HEADER[] = "Failed to load header.";
const char LC_FAIL_WRITE_HEADER[] = "Failed to write header.";

const char LC_KB_LEFT[] = "Left";
const char LC_KB_RIGHT[] = "Right";
const char LC_KB_UP[] = "Up";
const char LC_KB_DOWN[] = "Down";
const char LC_KB_JUMP[] = "Jump";
const char LC_KB_FIRE[] = "Fire";
const char LC_KB_WPN_PREV[] = "Wpn Prev";
const char LC_KB_WPN_NEXT[] = "Wpn Next";
const char LC_KB_INV[] = "Inventory";
const char LC_KB_MAP[] = "Map";
const char LC_KB_NEW_KEY[] = "Press new key for:";
const char LC_KB_BUSY[] = "Key already in use by:";

const char LC_RETURN[] = "Return";
const char LC_FULLSCREEN[] = "Fullscreen";

const char LC_PS_QUIT[] = ":Quit";
const char LC_PS_RESUME[] = ":Resume";
const char LC_PS_RESET[] = ":Reset";
const char LC_PS_OPTIONS[] = ":Options";
#endif

#endif // L10N_STRINGS_H
