
#include "nx.h"
#include "profile.h"
#include "profile.fdh"

#define PF_WEAPONS_OFFS		0x38
#define PF_CURWEAPON_OFFS	0x24
#define PF_INVENTORY_OFFS	0xD8
#define PF_TELEPORTER_OFFS	0x158
#define PF_FLAGS_OFFS		0x218

#define MAX_WPN_SLOTS		8
#define MAX_TELE_SLOTS		8

// load savefile #num into the given Profile structure.
bool profile_load(const char *pfname, Profile *file)
{
int i, curweaponslot;
FILE *fp;

	stat("Loading profile from %s...", pfname);
	memset(file, 0, sizeof(Profile));
	
	fp = fileopen(pfname, "rb");
	if (!fp)
	{
		staterr("profile_load: unable to open '%s'", pfname);
		return 1;
	}
	
	if (!fverifystring(fp, "Do041220"))
	{
		staterr("profile_load: invalid savegame format: '%s'", pfname);
		fclose(fp);
		return 1;
	}
	
	file->stage = fgetl(fp);
	file->songno = fgetl(fp);
	
	file->px = fgetl(fp);
	file->py = fgetl(fp);
	file->pdir = CVTDir(fgetl(fp));
	
	file->maxhp = fgeti(fp);
	file->num_whimstars = fgeti(fp);
	file->hp = fgeti(fp);
	
	fgeti(fp);						// unknown value
	curweaponslot = fgetl(fp);		// current weapon (slot, not number, converted below)
	fgetl(fp);						// unknown value
	file->equipmask = fgetl(fp);	// equipped items
	
	// load weapons
	fseek(fp, PF_WEAPONS_OFFS, SEEK_SET);
	for(i=0;i<MAX_WPN_SLOTS;i++)
	{
		int type = fgetl(fp);
		if (!type) break;
        if (type < 0 || type >= MAX_WPN_SLOTS) {
            staterr("profile_load: invalid weapon type %d", type);
            break;
        }
		
		int level = fgetl(fp);
		int xp = fgetl(fp);
		int maxammo = fgetl(fp);
		int ammo = fgetl(fp);
		
		file->weapons[type].hasWeapon = true;
		file->weapons[type].level = (level - 1);
		file->weapons[type].xp = xp;
		file->weapons[type].ammo = ammo;
		file->weapons[type].maxammo = maxammo;
		
		if (i == curweaponslot)
		{
			file->curWeapon = type;
		}
	}
	
	// load inventory
	file->ninventory = 0;
	fseek(fp, PF_INVENTORY_OFFS, SEEK_SET);
	for(i=0;i<MAX_INVENTORY;i++)
	{
		int item = fgetl(fp);
		if (!item) break;
		
		file->inventory[file->ninventory++] = item;
	}
	
	// load teleporter slots
	file->num_teleslots = 0;
	fseek(fp, PF_TELEPORTER_OFFS, SEEK_SET);
	for(i=0;i<NUM_TELEPORTER_SLOTS;i++)
	{
		int slotno = fgetl(fp);
		int scriptno = fgetl(fp);
		if (slotno == 0) break;
		
		file->teleslots[file->num_teleslots].slotno = slotno;
		file->teleslots[file->num_teleslots].scriptno = scriptno;
		file->num_teleslots++;
	}
	
	// load flags
	fseek(fp, PF_FLAGS_OFFS, SEEK_SET);
	if (!fverifystring(fp, "FLAG"))
	{
		staterr("profile_load: missing 'FLAG' marker");
		fclose(fp);
		return 1;
	}
	
	fresetboolean();
	for(i=0;i<NUM_GAMEFLAGS;i++)
	{
		file->flags[i] = fbooleanread(fp);
	}
	
	fclose(fp);
	return 0;
}


bool profile_save(const char *pfname, Profile *file)
{
FILE *fp;
int i;

	//stat("Writing saved game to %s...", pfname);
	fp = fileopen(pfname, "wb");
	if (!fp)
	{
		staterr("profile_save: unable to open %s", pfname);
		return 1;
	}
	
	fputstringnonull("Do041220", fp);
	
	fputl(file->stage, fp);
	fputl(file->songno, fp);
	
	fputl(file->px, fp);
	fputl(file->py, fp);
	fputl((file->pdir == RIGHT) ? 2:0, fp);
	
	fputi(file->maxhp, fp);
	fputi(file->num_whimstars, fp);
	fputi(file->hp, fp);
	
	fseek(fp, 0x2C, SEEK_SET);
	fputi(file->equipmask, fp);
	
	// save weapons
	fseek(fp, PF_WEAPONS_OFFS, SEEK_SET);
	int slotno = 0, curweaponslot = 0;
	
	for(i=0;i<WPN_COUNT;i++)
	{
		if (file->weapons[i].hasWeapon)
		{
			fputl(i, fp);
			fputl(file->weapons[i].level + 1, fp);
			fputl(file->weapons[i].xp, fp);
			fputl(file->weapons[i].maxammo, fp);
			fputl(file->weapons[i].ammo, fp);
			
			if (i == file->curWeapon)
				curweaponslot = slotno;
			
			slotno++;
			if (slotno >= MAX_WPN_SLOTS) break;
		}
	}
	
	if (slotno < MAX_WPN_SLOTS)
		fputl(0, fp);	// 0-type weapon: terminator
	
	// go back and save slot no of current weapon
	fseek(fp, PF_CURWEAPON_OFFS, SEEK_SET);
	fputl(curweaponslot, fp);
	
	// save inventory
	fseek(fp, PF_INVENTORY_OFFS, SEEK_SET);
	for(i=0;i<file->ninventory;i++)
	{
		fputl(file->inventory[i], fp);
	}
	
	fputl(0, fp);
	
	// write teleporter slots
	fseek(fp, PF_TELEPORTER_OFFS, SEEK_SET);
	for(i=0;i<MAX_TELE_SLOTS;i++)
	{
		if (i < file->num_teleslots)
		{
			fputl(file->teleslots[i].slotno, fp);
			fputl(file->teleslots[i].scriptno, fp);
		}
		else
		{
			fputl(0, fp);
			fputl(0, fp);
		}
	}
	
	// write flags
	fseek(fp, PF_FLAGS_OFFS, SEEK_SET);
	fputstringnonull("FLAG", fp);
	
	fresetboolean();
	for(i=0;i<NUM_GAMEFLAGS;i++)
	{
		fbooleanwrite(file->flags[i], fp);
	}
	
	fbooleanflush(fp);
	
	fclose(fp);
	return 0;
}


/*
void c------------------------------() {}
*/

// returns the filename for a save file given it's number
const char *GetProfileName(int num)
{
#ifndef __HAIKU__
	if (num == 0)
		return "profile.dat";
	else
		return stprintf("profile%d.dat", num+1);
#else
	char path[PATH_MAX];
	char *haikuPath = getHaikuSettingsPath();
	strcpy(path, haikuPath);
	if (num == 0) {
		strcat(path, "profile.dat");
	} else {
		strcat(path, "profile%d.dat");
	}
	free(haikuPath);
	return stprintf(path, num+1);
#endif
}

// returns whether the given save file slot exists
bool ProfileExists(int num)
{
	return file_exists(GetProfileName(num));
}

bool AnyProfileExists()
{
	for(int i=0;i<MAX_SAVE_SLOTS;i++)
		if (ProfileExists(i)) return true;
	
	return false;
}



