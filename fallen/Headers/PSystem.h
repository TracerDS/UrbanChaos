//
// psystem.h
//
// Proper particle system for handling a wide variety of effects:
// smoke, dust, mud, sparks, blood...
//

#pragma once

#ifndef PSX
#define PSYSTEM_MAX_PARTICLES	2048
#else
#define PSYSTEM_MAX_PARTICLES	64
#endif

#include "MFStdLib.h"
#include "game.h"
#include "thing.h"

// standard grav physics:
#define PFLAG_GRAVITY	1
// depth-pressure-linked bouyancy
#define PFLAG_BOUYANT	2
// fade alpha out over time:
#define PFLAG_FADE		4
// fade colour through fire palette over time:
#define PFLAG_FIRE		8
// bounce on collision with walls (else destroyed)
#define PFLAG_COLLIDE	16
// bounce on collision with floors
#define PFLAG_BOUNCE	32
// shrink/expand over time
#define PFLAG_RESIZE	64
// (could use 32+64 as something else)
// wander randomly (smoke etc)
#define PFLAG_WANDER	128
// enhanced wander, bigger range, kinda... er... drifty. theoretically.
#define PFLAG_DRIFT 	256
// animate thru the sprites
#define PFLAG_SPRITEANI	512
// invert the alpha
#define PFLAG_INVALPHA	1024
// shrink/expand over time, "hi res"
#define PFLAG_RESIZE2	2048
// loop the the sprites animation
#define PFLAG_SPRITELOOP 4096
// another crap fadey thing
#define PFLAG_FADE2		 8192
// damp out x/z movement
#define PFLAG_DAMPING	 16384
// hurts people it touches
#define PFLAG_HURTPEOPLE 32768
// explodes on impact
#define PFLAG_EXPLODE_ON_IMPACT 65536
// leaves a smoke trail
#define PFLAG_LEAVE_TRAIL 131072

// macros for commonly used sets
#define PFLAGS_SMOKE	PFLAG_FADE|PFLAG_RESIZE|PFLAG_WANDER
#define PFLAGS_BUBBLE	PFLAG_BOUYANT|PFLAG_EXPAND

struct Particle {
	std::int32_t	x,y,z;
	std::int32_t	colour, flags, life;
	std::uint16_t	page, sprite;
	std::uint8_t	padding, priority;
	std::int8_t	fade, resize;
	std::int16_t	dx,dy,dz;
	std::uint16_t	prev, next;
	std::uint16_t	size;
};


void PARTICLE_Run();
void PARTICLE_Draw();
void PARTICLE_Reset();
std::uint16_t PARTICLE_AddParticle(Particle &p);
std::uint16_t PARTICLE_Add(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t dx, std::int32_t dy, std::int32_t dz, std::uint16_t page, std::uint16_t sprite, std::int32_t colour, std::int32_t flags, std::int32_t life, std::uint8_t size, std::uint8_t priority, std::int8_t fade, std::int8_t resize);
	
// Some of the more commonly-used effects:
std::uint16_t PARTICLE_Exhaust(std::int32_t x, std::int32_t y, std::int32_t z,std::uint8_t density, std::uint8_t disperse);
std::uint16_t PARTICLE_Exhaust2(Thing *object, std::uint8_t density, std::uint8_t disperse);
std::uint16_t PARTICLE_Steam(std::int32_t x, std::int32_t y, std::int32_t z, std::uint8_t axis, std::int32_t vel, std::int32_t range, std::uint8_t time);
std::uint16_t PARTICLE_SGrenade(Thing *object, std::uint8_t time);


