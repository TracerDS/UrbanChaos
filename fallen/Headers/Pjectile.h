// Pjectile.h
// Guy Simmons, 4th January 1998.

#pragma once

//---------------------------------------------------------------

#define	MAX_PROJECTILES			10

#define	PROJECTILE_NONE			0
#define	PROJECTILE_TEST			1

//---------------------------------------------------------------

typedef struct
{
	COMMON(ProjectileType)


}Projectile;

typedef Projectile* ProjectilePtr;

//---------------------------------------------------------------

void init_projectiles();
Thing *alloc_projectile(std::uint8_t type);
void free_projectile(Thing *proj_thing);

//---------------------------------------------------------------

