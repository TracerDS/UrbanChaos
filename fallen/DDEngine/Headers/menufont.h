/************************************************************
 *
 *   menufont.h
 *   2D proportional-font text writer with poncey afterfx
 *
 */

#pragma once

#include "MFStdlib.h"

#define MENUFONT_FUTZING	(1)
#define MENUFONT_HALOED		(2)
#define MENUFONT_GLIMMER	(4)
#define MENUFONT_CENTRED	(8)
#define MENUFONT_FLANGED	(16)
#define MENUFONT_SINED		(32)
#define MENUFONT_ONLY 		(64)
#define MENUFONT_RIGHTALIGN	(128)
#define MENUFONT_HSCALEONLY	(256)
#define MENUFONT_SUPER_YCTR	(512)
#define MENUFONT_SHAKE		(1024)

void	MENUFONT_Load(char* fn, std::int32_t page, char* fontlist);
void	MENUFONT_Page(std::int32_t page);
void	MENUFONT_Draw(std::int16_t x, std::int16_t y, std::uint16_t scale, char* msg, std::int32_t rgb, std::uint16_t flags, std::int16_t max=-1);
void	MENUFONT_Draw_floats(float x, float y, std::uint16_t scale, char* msg, std::int32_t rgb, std::uint16_t flags);
void	MENUFONT_Free();
void	MENUFONT_Dimensions(char* fn, std::int32_t &x, std::int32_t &y, std::int16_t max=-1, std::int16_t scale=256);
std::int32_t	MENUFONT_CharFit(char* fn, std::int32_t x, std::uint16_t scale=256);
std::int32_t	MENUFONT_CharWidth(char fn, std::uint16_t scale=256);
#ifdef TARGET_DC
void MENUFONT_Draw_Selection_Box(std::int16_t x, std::int16_t y, std::uint16_t scale, char* msg, std::int32_t rgb, std::uint16_t flags, std::int16_t max=-1);
#endif



struct CharData {
	float x,y,ox,oy;	 // fractional texture coordinates. live with it.
	std::uint8_t width, height; 
	std::uint8_t xofs, yofs;    // offsets, hm.
};


extern CharData FontInfo[256];


#ifdef TARGET_DC
// The Yanks call them VMUs, Europeans call them VMs. Madness.
// Set this to true if you're a European.
extern bool bWriteVMInsteadOfVMU;
#endif


// ========================================================
//
// Cool line-fade-in text.
//
// ========================================================

//
// Clears any old fadein data.
//

void MENUFONT_fadein_init();

//
// Sets the x pixel fadein position.  Text will only appear to the left of this
// line.
//

void MENUFONT_fadein_line(std::int32_t x);	// x is in 8-bit fixed point

//
// Draws centred text where a fade 255 is opaque and 0 is transparent.
//

void MENUFONT_fadein_draw(std::int32_t x, std::int32_t y, std::uint8_t fade, char* msg);




