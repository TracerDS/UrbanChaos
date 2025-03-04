//
// Explosions!
//

#pragma once


//
// Initialises all the bangs.
//

void BANG_init();

//
// Processes the bangs.
//

void BANG_process();

//
// Creates a new bang of the given type.
//

#define BANG_BIG	0

void BANG_create(
	std::int32_t type,
	std::int32_t x,
	std::int32_t y,
	std::int32_t z);

//
// Drawing the bangs requires a semi-sphere primitive in the engine 
// whose colour of the centre point is given and the colour on the edge
// should be black. The primitive should be additively blended.
//

typedef struct
{
	std::int32_t x;
	std::int32_t y;
	std::int32_t z;
	std::int32_t dx;	// Gives the direction of the semi-sphere normalised to 256
	std::int32_t dy;
	std::int32_t dz;
	std::int32_t radius;
	std::uint8_t red;
	std::uint8_t green;
	std::uint8_t blue;
	std::uint8_t frame;
	
} BANG_Info;

void       BANG_get_start(std::uint8_t xmin, std::uint8_t xmax, std::uint8_t z);
BANG_Info *BANG_get_next();	// nullptr => No more bangs.



