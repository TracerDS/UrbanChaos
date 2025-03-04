// wadpart.h
//
// Particle system for Wadmenu, this provides animating elements for
// the frontend.
//

#pragma once

#define WADPART_MAXPARTICLES	96

typedef struct {
	std::int32_t vx,vy;
} PVECTOR;

typedef struct {
	std::uint8_t	used;			// Is this particle used
	std::uint8_t	type;			// Type of particle
	std::uint16_t	life;			// Life time of particle (in frames)
	std::int32_t	value;			// String ID, Texture Page or Texture ID
	std::int32_t	scale;			// Scale (*256)
	std::int32_t	colour;			// Color of particle
	std::int32_t	flags;			// Flags (including render flags for type)
	PVECTOR velocity;		// Speed of particle movement (*256)
	PVECTOR location;		// Location of particle (*256)
} W_Particle;

#define WADPART_TYPE_TEXT	0
#define WADPART_TYPE_IMAGE	1
#define WADPART_TYPE_CHAR	2
#define WADPART_TYPE_LEAF	3
#define WADPART_TYPE_BLOOD	4
#define WADPART_TYPE_RAIN	5
#define WADPART_TYPE_ANIM	6
#define WADPART_TYPE_BOARD	7
#define WADPART_TYPE_RECT	8
#define WADPART_TYPE_STRING 9

#define WADPART_FLAG_NORMAL		0x00000000
#define WADPART_FLAG_ACCEL		0x00000100
#define WADPART_FLAG_DECEL		0x00000200
#define WADPART_FLAG_GRAVITY	0x00000400
#define WADPART_FLAG_FADE		0x00000800
#define WADPART_FLAG_AMBIENT	0x00001000
#define WADPART_FLAG_FADEIN		0x00002000
#define WADPART_FLAG_FLUTTER	0x00004000
#define WADPART_FLAG_PULSE		0x00008000
#define WADPART_FLAG_WIND		0x00010000
#define WADPART_FLAG_ANIMATED	0x00020000
#define WADPART_FLAG_EXPAND		0x00040000
#define WADPART_FLAG_SHRINK		0x00080000

#define WADPART_FLAG_LEAF		(WADPART_FLAG_GRAVITY|WADPART_FLAG_FLUTTER|WADPART_FLAG_AMBIENT|WADPART_FLAG_WIND)
#define WADPART_FLAG_SNOW		(WADPART_FLAG_GRAVITY|WADPART_FLAG_FLUTTER|WADPART_FLAG_AMBIENT|WADPART_FLAG_PULSE)
#define WADPART_FLAG_BLOOD		(WADPART_FLAG_ACCEL|WADPART_FLAG_AMBIENT)
#define WADPART_FLAG_RAIN		(WADPART_FLAG_GRAVITY|WADPART_FLAG_AMBIENT)
#define WADPART_FLAG_SPLASH		(WADPART_FLAG_ANIMATED|WADPART_FLAG_AMBIENT)
#define WADPART_FLAG_RIPPLE		(WADPART_FLAG_EXPAND|WADPART_FLAG_FADE|WADPART_FLAG_AMBIENT)

#define WADPART_PAGE_LEAF		454

extern W_Particle Wadpart_Particle[WADPART_MAXPARTICLES];

extern void Wadpart_Init();
extern std::int32_t Wadpart_Sync();
extern void Wadpart_AddTextParticle(std::int32_t text,std::int32_t colour,std::int32_t x,std::int32_t y,std::int32_t dx,std::int32_t dy,std::int32_t life,std::int32_t flags);
extern void Wadpart_AddStringParticle(char *text,std::int32_t colour,std::int32_t x,std::int32_t y,std::int32_t dx,std::int32_t dy,std::int32_t life,std::int32_t flags);
extern void Wadpart_AddImageParticle(std::int32_t image,std::int32_t colour,std::int32_t x,std::int32_t y,std::int32_t dx,std::int32_t dy,std::int32_t life,std::int32_t flags);
extern void Wadpart_AddCharExplode(std::int32_t text,std::int32_t colour,std::int32_t x,std::int32_t y,std::int32_t life,std::int32_t flags);
extern void Wadpart_AddRainParticle(std::int32_t image,std::int32_t x,std::int32_t y,std::int32_t vx,std::int32_t vy,std::int32_t scale,std::int32_t life,std::int32_t flags);
extern void Wadpart_Render();
extern void Wadpart_AddLeafParticle(std::int32_t image,std::int32_t colour,std::int32_t x,std::int32_t y,std::int32_t scale,std::int32_t flags);
extern void Wadpart_AddBloodParticle(std::int32_t x,std::int32_t y,std::int32_t vx,std::int32_t vy,std::int32_t scale,std::int32_t flags);
extern void Wadpart_AddBoardParticle(std::int32_t image,std::int32_t x,std::int32_t y,std::int32_t dx,std::int32_t dy,std::int32_t life,std::int32_t flags);
extern void Wadpart_AddRectParticle(std::int32_t x,std::int32_t y,std::int32_t w,std::int32_t h,std::int32_t colour,std::int32_t flags);


