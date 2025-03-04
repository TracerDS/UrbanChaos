/************************************************************
 *
 *   font2d.h
 *   2D text writer
 *
 */

#ifndef _FONT_2D_H_
#define _FONT_2D_H_

#include "MFStdLib.h"
#include "poly.h"



void FONT2D_init(std::int32_t font_id);




typedef struct
{
	float u;
	float v;
	std::int32_t width;	// Width is in pixels. Divide by 256.0F to get width in texture coords.

} FONT2D_Letter;



//
// Draws the letter and returns the width of the letter in pixels.
// 

std::int32_t FONT2D_DrawLetter(char chr, std::int32_t x, std::int32_t y, std::uint32_t rgb=0xffffff, std::int32_t scale=256, std::int32_t page=POLY_PAGE_FONT2D, std::int16_t fade=0);

//
// Returns the width of the given letter in pixels.
// 

std::int32_t FONT2D_GetLetterWidth(char chr);


//
// Draws the string.
// 

void FONT2D_DrawString(const char* chr, std::int32_t x, std::int32_t y, std::uint32_t rgb=0xffffff, std::int32_t scale=256, std::int32_t page=POLY_PAGE_FONT2D, std::int16_t fade=0);

//
// Draws text that won't go off the screen. Returns the y coordinate of the line it finished on.
// Sets FONT2D_rightmost_x to be the the coordinate after the rightmost character it drew.
//

extern std::int32_t FONT2D_rightmost_x;
extern std::int32_t FONT2D_leftmost_x;	// For right justify!

std::int32_t FONT2D_DrawStringWrapTo(char*str, std::int32_t x, std::int32_t y, std::uint32_t rgb, std::int32_t scale, std::int32_t page, std::int16_t fade, std::int16_t span);

#ifdef TARGET_DC
// Um.... guys.
inline std::int32_t FONT2D_DrawStringWrap(char*chr, std::int32_t x, std::int32_t y, std::uint32_t rgb=0xffffff, std::int32_t scale=256, std::int32_t page=POLY_PAGE_FONT2D, std::int16_t fade=0)
{
	return FONT2D_DrawStringWrapTo ( chr, x, y, rgb, scale, page, fade, 600 );
}
#else
std::int32_t FONT2D_DrawStringWrap(char*chr, std::int32_t x, std::int32_t y, std::uint32_t rgb=0xffffff, std::int32_t scale=256, std::int32_t page=POLY_PAGE_FONT2D, std::int16_t fade=0);
#endif

//
// The text is right-justified and it wraps. The function returns the y coordinate of the
// line it finished on.
//

std::int32_t FONT2D_DrawStringRightJustify(char* chr, std::int32_t x, std::int32_t y, std::uint32_t rgb=0xffffff, std::int32_t scale=256, std::int32_t page=POLY_PAGE_FONT2D, std::int16_t fade=0, bool bDontDraw=false);

std::int32_t FONT2D_DrawStringRightJustifyNoWrap(const char* chr, std::int32_t x, std::int32_t y, std::uint32_t rgb=0xffffff, std::int32_t scale=256, std::int32_t page=POLY_PAGE_FONT2D, std::int16_t fade=0);



//
// Draws a centre-justified string.
//

void FONT2D_DrawStringCentred(char*chr, std::int32_t x, std::int32_t y, std::uint32_t rgb=0xffffff, std::int32_t scale=256, std::int32_t page=POLY_PAGE_FONT2D, std::int16_t fade=0);



//
// Draws a strikethrough.
//

//void FONT2D_DrawStrikethrough(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t rgb=0xffffff, std::int32_t scale=256, std::int32_t page=POLY_PAGE_FONT2D, std::int32_t fade=0);
void FONT2D_DrawStrikethrough(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t rgb, std::int32_t scale, std::int32_t page, std::int32_t fade, bool bUseLastOffset=false);



#endif

