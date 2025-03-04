//
// DrawXtra.h
// Matthew Rosenfeld	12 October 1998
//
// In order to make fallen more PSX-friendly, the various Really Gnasty Hacks scattered
// throughout the game code to draw "special" objects are being collected together in
// drawxtra.cpp -- stuff like the van, helicopter, footprints, etc.
//

#pragma once

#include "MFStdLib.h"
#include "game.h"
#include "thing.h"
#include "ribbon.h"

#define BLOOM_GLOW_ALWAYS	1
#define BLOOM_LENSFLARE		2
#define BLOOM_BEAM			4
#define	BLOOM_FAINT			8
#define BLOOM_NOGLOW		16
#define BLOOM_RAISE_LOS		32	// raise LOS point by 16 units

#define BLOOM_FLENSFLARE	(BLOOM_LENSFLARE|BLOOM_FAINT)

void CHOPPER_draw_chopper(Thing *p_chopper);
void TRACKS_DrawTrack(Thing *p_thing);
void PARTICLE_Draw();
void PYRO_draw_pyro(Thing *p_pyro);
void RIBBON_draw_ribbon(Ribbon *ribbon);
void BLOOM_draw(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t dx, std::int32_t dy, std::int32_t dz, std::int32_t col, std::uint8_t opts=BLOOM_LENSFLARE|BLOOM_BEAM);
void BLOOM_flare_draw(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t str);
void DRAWXTRA_Special(Thing *p_thing);
void ANIMAL_draw(Thing *p_thing);

void DRAW2D_Box(std::int32_t x, std::int32_t y, std::int32_t ox, std::int32_t oy, std::int32_t rgb, std::uint8_t flag, std::uint8_t depth=128);
#ifndef PSX
void DRAW2D_Box_Page(std::int32_t x, std::int32_t y, std::int32_t ox, std::int32_t oy, std::int32_t rgb, std::int32_t page, std::uint8_t depth=128);
#endif
void DRAW2D_Tri(std::int32_t x, std::int32_t y, std::int32_t ox, std::int32_t oy, std::int32_t tx, std::int32_t ty, std::int32_t rgb, std::uint8_t flag);
void DRAW2D_Sprite(std::int32_t x, std::int32_t y, std::int32_t ox, std::int32_t oy, float u, float v, float ou, float ov, std::int32_t page, std::int32_t rgb);

//
// Draws the final glowwy thing for the Guardian of Baalrog. A fade of 0 means transparent
// 255 is completely faded in.
//

void DRAWXTRA_final_glow(std::int32_t x, std::int32_t y, std::int32_t z, std::uint8_t fade);
void DRAWXTRA_MIB_destruct(Thing *p_thing);


// Do this to throttle effects down to sensible levels.

// Called with how many sprites are wanted.
// Returns the number it can have.
int IWouldLikeSomePyroSpritesHowManyCanIHave ( int iIWantThisMany );
// If the rout can't change how many it uses, at least call this to warn the pyro system that they will be used.
void IHaveToHaveSomePyroSprites( int iINeedThisMany );


// Just call this once a frame, would you? Ta.
void Pyros_EndOfFrameMarker ( void );


