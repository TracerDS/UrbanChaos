// Gamut.h
// Guy Simmons, 4th November 1997.

#ifndef GAMUT_H
#define GAMUT_H

//---------------------------------------------------------------

#define MAX_GAMUT_RADIUS (24)

//---------------------------------------------------------------

typedef struct
{
    std::int8_t DX,
        DZ;
    std::int16_t Angle;
} GamutElement;

//---------------------------------------------------------------

extern GamutElement gamut_ele_pool[],
    *gamut_ele_ptr[];

void build_gamut_table();
void draw_gamut(std::int32_t x, std::int32_t y);

//---------------------------------------------------------------

#endif
