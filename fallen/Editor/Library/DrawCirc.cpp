// DrawCirc.cpp
// Guy Simmons, 9th February 1998.

#include "Editor.hpp"

#define DRAW_STEPS 32

//---------------------------------------------------------------

void DrawCircle(std::int32_t x, std::int32_t y, std::int32_t radius, std::uint32_t colour) {
    std::int32_t angle = 0,
                 angle_step = (2048 / DRAW_STEPS),
                 c0,
                 x1, y1,
                 x2, y2;

    for (c0 = 0; c0 < DRAW_STEPS; c0++) {
        x1 = x + ((SIN(angle) * radius) >> 16);
        y1 = y + ((COS(angle) * radius) >> 16);
        x2 = x + ((SIN((angle + angle_step) & 2047) * radius) >> 16);
        y2 = y + ((COS((angle + angle_step) & 2047) * radius) >> 16);

        DrawLine(x1, y1, x2, y2, colour);

        angle += angle_step;
    }
}

//---------------------------------------------------------------

void DrawCircleC(std::int32_t x, std::int32_t y, std::int32_t radius, std::uint32_t colour) {
    std::int32_t angle = 0,
                 angle_step = (2048 / DRAW_STEPS),
                 c0,
                 x1, y1,
                 x2, y2;

    for (c0 = 0; c0 < DRAW_STEPS; c0++) {
        x1 = x + ((SIN(angle) * radius) >> 16);
        y1 = y + ((COS(angle) * radius) >> 16);
        x2 = x + ((SIN((angle + angle_step) & 2047) * radius) >> 16);
        y2 = y + ((COS((angle + angle_step) & 2047) * radius) >> 16);

        DrawLineC(x1, y1, x2, y2, colour);

        angle += angle_step;
    }
}

//---------------------------------------------------------------
