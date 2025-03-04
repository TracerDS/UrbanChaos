//
// Splashes...
//

#ifndef _DRIP_
#define _DRIP_


#define DRIP_FLAG_PUDDLES_ONLY		(1)

//
// Gets rid of all the drips.
//

void DRIP_init();

//
// Creates a new drip at (x,y,z)
//

void DRIP_create(
		std::uint16_t x,
		std::int16_t y,
		std::uint16_t z,
		std::uint8_t flags);

//
// Creates a new drip at (x,y,z) only if (x,y,z)
// is in a puddle.
//

void DRIP_create_if_in_puddle(
		std::uint16_t x,
		std::int16_t y,
		std::uint16_t z);

//
// Makes all the drips get bigger and fade out.
//

void DRIP_process();


//
// Drawing the drips...
//

typedef struct
{
	std::uint16_t x;
	std::int16_t y;
	std::uint16_t z;
	std::uint8_t size;
	std::uint8_t fade; // 255 => opaque, 0 => transparent.
	std::uint8_t flags;
	
} DRIP_Info;

void DRIP_get_start();
DRIP_Info *DRIP_get_next (); // nullptr => no more drips.


#endif
