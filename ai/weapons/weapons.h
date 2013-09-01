
#ifndef _WEAPONS_H
#define _WEAPONS_H

#include "../stdai.h"

uint8_t run_shot(Object *o, bool destroys_blocks);
enum run_shot_result
{
	RS_NONE = 0,
	RS_HIT_ENEMY,
	RS_HIT_WALL,
	RS_TTL_EXPIRED
};

Object *check_hit_enemy(Object *o, uint32_t flags_to_exclude=0);
Object *damage_enemies(Object *o, uint32_t flags_to_exclude=0);
int damage_multiple_enemies(Object *o, uint32_t flags_to_exclude=0);

void shot_spawn_effect(Object *o, int effectno);
void shot_dissipate(Object *o, int effectno = EFFECT_STARPOOF);
bool shot_destroy_blocks(Object *o);

bool IsBlockedInShotDir(Object *o);

// ---------------------------------------

// from Fireball code, shared with Snake
Object *create_fire_trail(Object *o, int objtype, int level);


#endif
