//
// ribbon.h
// special effects: smoke, fire, wheelspray... all sorts
// matthew rosenfeld 10 nov 1998
//

#ifndef _RIBBON_H_
#define _RIBBON_H_

#include "MFStdLib.h"
#include "Structs.h"

#define MAX_RIBBONS			64
#define MAX_RIBBON_SIZE		32

#define RIBBON_FLAG_USED	1
#define RIBBON_FLAG_FADE	2
#define RIBBON_FLAG_SLIDE	4
#define	RIBBON_FLAG_DOUBLED	8
#define	RIBBON_FLAG_CONVECT	16
#define	RIBBON_FLAG_IALPHA	32


struct Ribbon {
	std::int32_t		Flags;
	std::int32_t		Page;			// POLY_PAGE
	std::int32_t		Life;			// -1 is forever
	std::int32_t		RGB;			// colour
	std::uint8_t		Size;			// Max number of points allowed
	std::uint8_t		Head;			// Where the next point will be added
	std::uint8_t		Tail;			// Where points will be eroded away from
	std::uint8_t		Scroll;			// Offset for looped textures
	std::uint8_t		FadePoint;		// Number of steps across which fadeout occurs
	std::uint8_t		SlideSpeed;		// Offset applied to Scroll
	std::uint8_t		TextureU;		// How many loops for width of strip
	std::uint8_t		TextureV;		// How many steps each loop applies to
	GameCoord	Points[MAX_RIBBON_SIZE];
};




void RIBBON_init();
void RIBBON_draw();
void RIBBON_process();
std::int32_t RIBBON_alloc(std::int32_t flags, std::uint8_t max_segments, std::int32_t page, std::int32_t life=-1, std::uint8_t fade=0, std::uint8_t scroll=0, std::uint8_t u=1, std::uint8_t v=0, std::int32_t rgb=0xFFFFFF);
void RIBBON_free(std::int32_t ribbon);
void RIBBON_extend(std::int32_t ribbon, std::int32_t x, std::int32_t y, std::int32_t z);
std::int32_t RIBBON_length(std::int32_t ribbon);
void RIBBON_life(std::int32_t ribbon, std::int32_t life);


#endif
