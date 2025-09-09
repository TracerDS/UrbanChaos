// Pjectile.h
// Guy Simmons, 4th January 1998.

#ifndef PJECTILE_H
#define PJECTILE_H

//---------------------------------------------------------------

#define MAX_PROJECTILES 10

#define PROJECTILE_NONE 0
#define PROJECTILE_TEST 1

//---------------------------------------------------------------

typedef struct
{
    COMMON(ProjectileType)

} Projectile;

typedef Projectile *ProjectilePtr;

//---------------------------------------------------------------

void init_projectiles();
Entity *alloc_projectile(std::uint8_t type);
void free_projectile(Entity *proj_thing);

//---------------------------------------------------------------

#endif
