
#ifndef _PROFILE_H
#define _PROFILE_H

// how many bytes of data long a profile.dat is.
// used by the replays which use the regular profile functions
// to write a savefile then tack their own data onto the end.
#define PROFILE_LENGTH		0x604

struct Profile
{
	int stage;
	int songno;
	int px, py, pdir;
	int hp, maxhp, num_whimstars;
	uint32_t equipmask;
	
	int curWeapon;
	struct
	{
		bool hasWeapon;
		int level;
		int xp;
		int ammo, maxammo;
	} weapons[WPN_COUNT];
	
	int inventory[MAX_INVENTORY];
	int ninventory;
	
	bool flags[NUM_GAMEFLAGS];
	
	struct
	{
		int slotno;
		int scriptno;
	} teleslots[NUM_TELEPORTER_SLOTS];
	int num_teleslots;
};

#endif
