//
// Collections of little sparkley projectiles.
//

#pragma once



//
// Remove all glitter.
//

void GLITTER_init();



//
// Creates a new collection of sparkles... a glitter!
//

#define GLITTER_FLAG_DXPOS	 (1 << 0)	// Sparkle 'dx' must be positive.
#define GLITTER_FLAG_DXNEG	 (1 << 1)	// Sparkle 'dx' must be negative.
#define GLITTER_FLAG_DYPOS	 (1 << 2)	// Sparkle 'dy' must be positive.
#define GLITTER_FLAG_DYNEG	 (1 << 3)	// Sparkle 'dy' must be negative.
#define GLITTER_FLAG_DZPOS	 (1 << 4)	// Sparkle 'dz' must be positive.
#define GLITTER_FLAG_DZNEG	 (1 << 5)	// Sparkle 'dz' must be negative.
#define GLITTER_FLAG_USED	 (1 << 6)	// Private...
#define GLITTER_FLAG_DESTROY (1 << 7)	// Private...

std::uint8_t GLITTER_create(
		std::uint8_t flag,
		std::uint8_t map_x,
		std::uint8_t map_z,
		std::uint32_t colour);

//
// Destroys the glitter when all its spark die off.
//

void  GLITTER_destroy(std::uint8_t glitter);

//
// Adds a sparkle to a glitter.
//

void GLITTER_add(
		std::uint8_t glitter,
		std::int32_t x,
		std::int32_t y,
		std::int32_t z);


void GLITTER_process();




//
// Each sparkle is a line segment...
//

typedef struct
{
	std::int32_t x1;
	std::int32_t y1;
	std::int32_t z1;
	std::int32_t x2;
	std::int32_t y2;
	std::int32_t z2;
	std::uint32_t colour;
	
} GLITTER_Info;

void          GLITTER_get_start(std::uint8_t xmin, std::uint8_t xmax, std::uint8_t z);
GLITTER_Info *GLITTER_get_next();




