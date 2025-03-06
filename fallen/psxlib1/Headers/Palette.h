// Palette.h
// Guy Simmons, 11th February 1997.

#ifndef _PALETTE_H_
#define _PALETTE_H_

#ifndef _MF_TYPES_H_
#include <MFTypes.h>
#endif

#define FADE_IN 1 << 0
#define FADE_OUT 1 << 1

extern std::uint8_t CurrentPalette[256 * 3];

void InitPalettes();
std::int32_t CreatePalettes();
void DestroyPalettes();
void RestorePalettes();
void SetPalette(std::uint8_t *the_palette);
std::int32_t FindColour(std::uint8_t *pal, std::int32_t r, std::int32_t g, std::int32_t b);

#endif
