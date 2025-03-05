// DrawLine.c
// Guy Simmons, 7th October 1996

#include "Editor.hpp"

//---------------------------------------------------------------
// Standard Bresenham algorithm.

void DrawLine8(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour) {
    std::uint8_t *line_dest;
    std::int32_t ax, ay,
        d,
        dx, dy,
        modulo,
        sx, sy,
        x, y;

    dx = x2 - x1;
    dy = y2 - y1;

    if (abs(dx) + abs(dy) > 10000)
        return;

    ax = abs(dx) << 1;
    ay = abs(dy) << 1;
    sx = sgn(dx);
    sy = sgn(dy);
    modulo = WorkScreenWidth * sy;

    x = x1;
    y = y1;
    line_dest = WorkWindow + x + (y * WorkScreenWidth);
    if (ax > ay) {
        d = ay - (ax >> 1);
        while (1) {
            *line_dest = (std::uint8_t) colour;
            if (x == x2)
                return;
            if (d >= 0) {
                d -= ax;
                line_dest += modulo;
            }
            x += sx;
            d += ay;
            line_dest += sx;
        }
    } else {
        d = ax - (ay >> 1);
        while (1) {
            *line_dest = (std::uint8_t) colour;
            if (y == y2)
                return;
            if (d >= 0) {
                d -= ay;
                line_dest += sx;
            }
            y += sy;
            d += ax;
            line_dest += modulo;
        }
    }
}

void DrawLine16(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour) {
    std::uint16_t *line_dest;
    std::int32_t ax, ay,
        d,
        dx, dy,
        modulo,
        sx, sy,
        x, y;

    dx = x2 - x1;
    dy = y2 - y1;

    if (abs(dx) + abs(dy) > 10000)
        return;

    ax = abs(dx) << 1;
    ay = abs(dy) << 1;
    sx = sgn(dx);
    sy = sgn(dy);
    modulo = (WorkScreenWidth >> 1) * sy;

    x = x1;
    y = y1;
    line_dest = (std::uint16_t *) WorkWindow + x + (y * WorkScreenWidth >> 1);
    if (ax > ay) {
        d = ay - (ax >> 1);
        while (1) {
            *line_dest = (std::uint16_t) colour;
            if (x == x2)
                return;
            if (d >= 0) {
                d -= ax;
                line_dest += modulo;
            }
            x += sx;
            d += ay;
            line_dest += sx;
        }
    } else {
        d = ax - (ay >> 1);
        while (1) {
            *line_dest = (std::uint16_t) colour;
            if (y == y2)
                return;
            if (d >= 0) {
                d -= ay;
                line_dest += sx;
            }
            y += sy;
            d += ax;
            line_dest += modulo;
        }
    }
}

void DrawLine32(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour) {
    std::uint32_t *line_dest;
    std::int32_t ax, ay,
        d,
        dx, dy,
        modulo,
        sx, sy,
        x, y;

    dx = x2 - x1;
    dy = y2 - y1;

    if (abs(dx) + abs(dy) > 10000)
        return;

    ax = abs(dx) << 1;
    ay = abs(dy) << 1;
    sx = sgn(dx);
    sy = sgn(dy);
    modulo = (WorkScreenWidth >> 2) * sy;

    x = x1;
    y = y1;
    line_dest = (std::uint32_t *) WorkWindow + x + (y * WorkScreenWidth >> 2);
    if (ax > ay) {
        d = ay - (ax >> 1);
        while (1) {
            *line_dest = (std::uint32_t) colour;
            if (x == x2)
                return;
            if (d >= 0) {
                d -= ax;
                line_dest += modulo;
            }
            x += sx;
            d += ay;
            line_dest += sx;
        }
    } else {
        d = ax - (ay >> 1);
        while (1) {
            *line_dest = (std::uint32_t) colour;
            if (y == y2)
                return;
            if (d >= 0) {
                d -= ay;
                line_dest += sx;
            }
            y += sy;
            d += ax;
            line_dest += modulo;
        }
    }
}

//---------------------------------------------------------------
// Standard Bresenham algorithm.
// Bloody slow at clipping tho'

void DrawLineC8(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::uint32_t colour) {
    std::int32_t ax, ay,
        d,
        dx, dy,
        sx, sy,
        x, y;

    dx = x2 - x1;
    dy = y2 - y1;

    if (abs(dx) + abs(dy) > 10000)
        return;

    ax = abs(dx) << 1;
    ay = abs(dy) << 1;
    sx = sgn(dx);
    sy = sgn(dy);

    x = x1;
    y = y1;
    if (ax > ay) {
        d = ay - (ax >> 1);
        while (1) {
            DrawPixelC(x, y, colour);
            if (x == x2)
                return;
            if (d >= 0) {
                y += sy;
                d -= ax;
            }
            x += sx;
            d += ay;
        }
    } else {
        d = ax - (ay >> 1);
        while (1) {
            DrawPixelC(x, y, colour);
            if (y == y2)
                return;
            if (d >= 0) {
                x += sx;
                d -= ay;
            }
            y += sy;
            d += ax;
        }
    }
}

//---------------------------------------------------------------

void DrawHLine8(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour) {
    std::uint8_t *line_dest;
    std::uint32_t count;

    if (x1 > x2) {
        swap(x1, x2);
    }
    line_dest = WorkWindow + x1 + (y * WorkScreenWidth);
    count = (x2 - x1) + 1;
    while (count--) {
        *(line_dest++) = (std::uint8_t) colour;
    }
}

void DrawHLine16(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour) {
    std::uint16_t *line_dest;
    std::uint32_t count;

    if (x1 > x2) {
        swap(x1, x2);
    }
    line_dest = (std::uint16_t *) WorkWindow + x1 + (y * WorkScreenWidth >> 1);
    count = (x2 - x1) + 1;
    while (count--) {
        *(line_dest++) = (std::uint16_t) colour;
    }
}

void DrawHLine32(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour) {
    std::uint32_t *line_dest;
    std::uint32_t count;

    if (x1 > x2) {
        swap(x1, x2);
    }
    line_dest = (std::uint32_t *) WorkWindow + x1 + (y * WorkScreenWidth >> 2);
    count = (x2 - x1) + 1;
    while (count--) {
        *(line_dest++) = (std::uint32_t) colour;
    }
}

//---------------------------------------------------------------

void DrawHLineC8(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour) {
    std::uint8_t *line_dest;
    std::uint32_t count;

    if (y >= 0 && y < WorkWindowHeight) {
        if (x1 > x2) {
            swap(x1, x2);
        }
        if (x1 < WorkWindowWidth && x2 >= 0) {
            if (x1 < 0)
                x1 = 0;
            if (x2 >= WorkWindowWidth)
                x2 = WorkWindowWidth - 1;
            line_dest = WorkWindow + x1 + (y * WorkScreenWidth);
            count = (x2 - x1) + 1;
            while (count--) {
                *(line_dest++) = (std::uint8_t) colour;
            }
        }
    }
}

void DrawHLineC16(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour) {
    std::uint16_t *line_dest;
    std::uint32_t count;

    if (y >= 0 && y < WorkWindowHeight) {
        if (x1 > x2) {
            swap(x1, x2);
        }
        if (x1 < WorkWindowWidth && x2 >= 0) {
            if (x1 < 0)
                x1 = 0;
            if (x2 >= WorkWindowWidth)
                x2 = WorkWindowWidth - 1;
            line_dest = (std::uint16_t *) WorkWindow + x1 + (y * WorkScreenWidth >> 1);
            count = (x2 - x1) + 1;
            while (count--) {
                *(line_dest++) = (std::uint16_t) colour;
            }
        }
    }
}

void DrawHLineC32(std::int32_t x1, std::int32_t x2, std::int32_t y, std::uint32_t colour) {
    std::uint32_t *line_dest;
    std::uint32_t count;

    if (y >= 0 && y < WorkWindowHeight) {
        if (x1 > x2) {
            swap(x1, x2);
        }
        if (x1 < WorkWindowWidth && x2 >= 0) {
            if (x1 < 0)
                x1 = 0;
            if (x2 >= WorkWindowWidth)
                x2 = WorkWindowWidth - 1;
            line_dest = (std::uint32_t *) WorkWindow + x1 + (y * WorkScreenWidth >> 2);
            count = (x2 - x1) + 1;
            while (count--) {
                *(line_dest++) = (std::uint32_t) colour;
            }
        }
    }
}

//---------------------------------------------------------------

void DrawVLine8(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour) {
    std::uint8_t *line_dest;
    std::uint32_t count,
        modulo;

    if (y1 > y2) {
        swap(y1, y2);
    }
    line_dest = WorkWindow + x + (y1 * WorkScreenWidth);
    count = (y2 - y1) + 1;
    modulo = WorkScreenWidth;
    while (count--) {
        *line_dest = (std::uint8_t) colour;
        line_dest += modulo;
    }
}

void DrawVLine16(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour) {
    std::uint16_t *line_dest;
    std::uint32_t count,
        modulo;

    if (y1 > y2) {
        swap(y1, y2);
    }
    line_dest = (std::uint16_t *) WorkWindow + x + (y1 * WorkScreenWidth >> 1);
    count = (y2 - y1) + 1;
    modulo = WorkScreenWidth >> 1;
    while (count--) {
        *line_dest = (std::uint16_t) colour;
        line_dest += modulo;
    }
}

void DrawVLine32(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour) {
    std::uint32_t *line_dest;
    std::uint32_t count,
        modulo;

    if (y1 > y2) {
        swap(y1, y2);
    }
    line_dest = (std::uint32_t *) WorkWindow + x + (y1 * WorkScreenWidth >> 2);
    count = (y2 - y1) + 1;
    modulo = WorkScreenWidth >> 2;
    while (count--) {
        *line_dest = (std::uint32_t) colour;
        line_dest += modulo;
    }
}

//---------------------------------------------------------------

void DrawVLineC8(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour) {
    std::uint8_t *line_dest;
    std::uint32_t count;

    if (x >= 0 && x < WorkWindowWidth) {
        if (y1 > y2) {
            swap(y1, y2);
        }
        if (y1 < WorkWindowHeight && y2 >= 0) {
            if (y1 < 0)
                y1 = 0;
            if (y2 >= WorkWindowHeight)
                y2 = WorkWindowHeight - 1;
            line_dest = WorkWindow + x + (y1 * WorkScreenWidth);
            count = (y2 - y1) + 1;
            while (count--) {
                *line_dest = (std::uint8_t) colour;
                line_dest += WorkScreenWidth;
            }
        }
    }
}

void DrawVLineC16(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour) {
    std::uint16_t *line_dest;
    std::uint32_t count;

    if (x >= 0 && x < WorkWindowWidth) {
        if (y1 > y2) {
            swap(y1, y2);
        }
        if (y1 < WorkWindowHeight && y2 >= 0) {
            if (y1 < 0)
                y1 = 0;
            if (y2 >= WorkWindowHeight)
                y2 = WorkWindowHeight - 1;
            line_dest = (std::uint16_t *) WorkWindow + x + (y1 * WorkScreenWidth >> 1);
            count = (y2 - y1) + 1;
            while (count--) {
                *line_dest = (std::uint16_t) colour;
                line_dest += WorkScreenWidth >> 1;
            }
        }
    }
}

void DrawVLineC32(std::int32_t x, std::int32_t y1, std::int32_t y2, std::uint32_t colour) {
    std::uint32_t *line_dest;
    std::uint32_t count;

    if (x >= 0 && x < WorkWindowWidth) {
        if (y1 > y2) {
            swap(y1, y2);
        }
        if (y1 < WorkWindowHeight && y2 >= 0) {
            if (y1 < 0)
                y1 = 0;
            if (y2 >= WorkWindowHeight)
                y2 = WorkWindowHeight - 1;
            line_dest = (std::uint32_t *) WorkWindow + x + (y1 * WorkScreenWidth >> 2);
            count = (y2 - y1) + 1;
            while (count--) {
                *line_dest = (std::uint32_t) colour;
                line_dest += WorkScreenWidth >> 2;
            }
        }
    }
}

//---------------------------------------------------------------
