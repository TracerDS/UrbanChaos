//
// An anti-aliased triangle draw.
//

#include <MFStdLib.h>

//
// The internal fixed point we used.
//

#define AA_FIX 10
#define AA_PIX (1 << AA_FIX)
#define AA_PMN (AA_PIX - 1)

//
// The spans.
//

// #define AA_MAX_SPAN_X 256
// #define AA_MAX_SPAN_Y 256
#define AA_MAX_SPAN_X 33
#define AA_MAX_SPAN_Y 33

struct AA_Span {
    std::int32_t rhs_min;
    std::int32_t rhs_max;

    std::int32_t lhs_min;
    std::int32_t lhs_max;

    std::int32_t pixel[AA_MAX_SPAN_X];
};

AA_Span AA_span[AA_MAX_SPAN_Y];

//
// A pixel on the rhs of a triangle filled in by 'frac' amount.
// frac is an AA_FIX-bit fraction.
//

void AA_pixel_rhs(
    std::int32_t x,
    std::int32_t y,
    std::int32_t frac) {
    ASSERT(WITHIN(x, 0, AA_MAX_SPAN_X - 1));
    ASSERT(WITHIN(y, 0, AA_MAX_SPAN_Y - 1));

    frac >>= AA_FIX - 8;

    AA_Span *as = &AA_span[y];

    if (WITHIN(x, as->rhs_min, as->rhs_max)) {
        as->pixel[x] += frac;
        return;
    }

    as->pixel[x] = frac;

    if (x < as->rhs_min) {
        as->rhs_min = x;
    }
    if (x > as->rhs_max) {
        as->rhs_max = x;
    }
}

//
// A pixel on the lhs of a triangle where there is frac_left
// free space on the left of the pixel and frac_right filled
// space on the right of the pixel.
//

void AA_pixel_lhs(
    std::int32_t x,
    std::int32_t y,
    std::int32_t frac_left,
    std::int32_t frac_right) {
    ASSERT(WITHIN(x, 0, AA_MAX_SPAN_X - 1));
    ASSERT(WITHIN(y, 0, AA_MAX_SPAN_Y - 1));

    frac_left >>= AA_FIX - 8;
    frac_right >>= AA_FIX - 8;

    // ASSERT(WITHIN(frac_left,  0, 256));
    // ASSERT(WITHIN(frac_right, 0, 256));

    AA_Span *as = &AA_span[y];

    if (WITHIN(x, as->rhs_min, as->rhs_max)) {
        //
        // Take a chunk out of this pixel.
        //

        as->pixel[x] -= frac_left;
    } else {
        if (WITHIN(x, as->lhs_min, as->lhs_max)) {
            as->pixel[x] += frac_right;
            return;
        }
        as->pixel[x] = frac_right;
    }

    if (x < as->lhs_min) {
        as->lhs_min = x;
    }
    if (x > as->lhs_max) {
        as->lhs_max = x;
    }
}

//
// An rhs span on one line. All values are in AA_FIX-bit fixed point.
//

void AA_span_rhs(
    std::int32_t dydx,
    std::int32_t x1,
    std::int32_t y1,
    std::int32_t x2,
    std::int32_t y2) {
    std::int32_t dx;
    std::int32_t dy;

    std::int32_t xleft;
    std::int32_t xright;
    std::int32_t ytop;
    std::int32_t ybot;

    std::int32_t frac;

    if (x2 == x1) {
        //
        // Ignore dydx!
        //

        frac = (x1 & AA_PMN) * (y2 - y1) >> AA_FIX; // Fraction of this pixel covered.

        AA_pixel_rhs(x1 >> AA_FIX, y1 >> AA_FIX, frac);
        return;
    }

    if (x2 > x1) {
        xleft = x1;
        ytop = y1;

        while (true) {
            xright = xleft + AA_PIX;
            xright &= ~AA_PMN;

            if (xright > x2) {
                xright = x2;
            }

            dx = xright - xleft;
            dy = dx * dydx >> AA_FIX;
            ybot = ytop + dy;

            frac = dx * dy >> (AA_FIX + 1);
            frac += dx * (y2 - ybot) >> AA_FIX;

            if (xleft & AA_PMN) {
                frac += dy * (xleft & AA_PMN) >> AA_FIX;
                frac += (y2 - ybot) * (xleft & AA_PMN) >> AA_FIX;
            }

            AA_pixel_rhs(xleft >> AA_FIX, y1 >> AA_FIX, frac);

            if (xright >= x2) {
                return;
            }

            xleft = xright;
            ytop = ybot;
        }
    } else {
        xright = x1;
        ytop = y1;

        while (true) {
            xleft = xright - 1;
            xleft &= ~AA_PMN;

            if (xleft < x2) {
                xleft = x2;
            }

            dx = xright - xleft;
            dy = -dx * dydx >> AA_FIX;
            ybot = ytop + dy;

            frac = dx * dy >> (AA_FIX + 1);
            frac += dx * (ytop - y1) >> AA_FIX;

            if (xleft & AA_PMN) {
                frac += dy * (xleft & AA_PMN) >> AA_FIX;
                frac += (ytop - y1) * (xleft & AA_PMN) >> AA_FIX;
            }

            AA_pixel_rhs(xleft >> AA_FIX, y1 >> AA_FIX, frac);

            if (xleft <= x2)
                return;

            xright = xleft;
            ytop = ybot;
        }
    }
}

//
// An lhs span on one line. All values are in AA_FIX-bit fixed point.
//

void AA_span_lhs(
    std::int32_t dydx,
    std::int32_t x1,
    std::int32_t y1,
    std::int32_t x2,
    std::int32_t y2) {
    std::int32_t dx;
    std::int32_t dy;

    std::int32_t xleft;
    std::int32_t xright;
    std::int32_t xpixel;
    std::int32_t ytop;
    std::int32_t ybot;

    std::int32_t frac_left;
    std::int32_t frac_right;

    if (x2 == x1) {
        //
        // Ignore dydx!
        //

        frac_left = (x1 & AA_PMN) * (y2 - y1) >> AA_FIX;
        frac_right = (AA_PMN - (x1 & AA_PMN)) * (y2 - y1) >> AA_FIX;

        AA_pixel_lhs(x1 >> AA_FIX, y1 >> AA_FIX, frac_left, frac_right);

        return;
    }

    if (x2 > x1) {
        xleft = x1;
        ytop = y1;

        while (true) {
            xright = xleft + AA_PIX;
            xright &= ~AA_PMN;
            xpixel = xright;

            if (xright > x2) {
                xright = x2;
            }

            dx = xright - xleft;
            dy = dx * dydx >> AA_FIX;
            ybot = ytop + dy;

            frac_right = dx * dy >> (AA_FIX + 1);
            frac_right += dx * (ytop - y1) >> AA_FIX;

            if (xpixel - xright) {
                frac_right += dy * (xpixel - xright) >> AA_FIX;
                frac_right += (ytop - y1) * (xpixel - xright) >> AA_FIX;
            }

            frac_left = dx * dy >> (AA_FIX + 1);
            frac_left += dx * (y2 - ybot) >> AA_FIX;

            if (xleft & AA_PMN) {
                frac_left += dy * (xleft & AA_PMN) >> AA_FIX;
                frac_left += (y2 - ybot) * (xleft & AA_PMN) >> AA_FIX;
            }

            AA_pixel_lhs(xleft >> AA_FIX, y1 >> AA_FIX, frac_left, frac_right);

            if (xright >= x2)
                return;

            xleft = xright;
            ytop = ybot;
        }
    } else {
        xright = x1;
        ytop = y1;

        while (1) {
            xleft = xright - 1;
            xleft &= ~AA_PMN;
            xpixel = xleft + AA_PIX;

            if (xleft < x2) {
                xleft = x2;
            }

            dx = xright - xleft;
            dy = dx * -dydx >> AA_FIX;
            ybot = ytop + dy;

            frac_right = dx * dy >> (AA_FIX + 1);
            frac_right += dx * (y2 - ybot) >> AA_FIX;

            if (xpixel - xright) {
                frac_right += dy * (xpixel - xright) >> AA_FIX;
                frac_right += (y2 - ybot) * (xpixel - xright) >> AA_FIX;
            }

            frac_left = dx * dy >> (AA_FIX + 1);
            frac_left += dx * (ytop - y1) >> AA_FIX;
            frac_left += dy * (xleft & AA_PMN) >> AA_FIX;
            frac_left += (ytop - y1) * (xleft & AA_PMN) >> AA_FIX;

            AA_pixel_lhs(
                xleft >> AA_FIX,
                y1 >> AA_FIX,
                frac_left,
                frac_right);

            if (xleft <= x2) {
                return;
            }

            xright = xleft;
            ytop = ybot;
        }
    }
}

//
// A line on the rhs of the triangle. py1 <= py2
//

void AA_line_rhs(std::int32_t px1, std::int32_t py1, std::int32_t px2, std::int32_t py2) {
    if (py1 == py2) {
        return;
    }

    std::int32_t dy;
    std::int32_t dx;

    std::int32_t dydx;
    std::int32_t dxdy;

    std::int32_t x1, x2;
    std::int32_t y1, y2;

    dx = px2 - px1;
    dy = py2 - py1;
    dxdy = (dx << AA_FIX) / dy;

    if (dx) {
        dydx = dy << AA_FIX;
        dydx /= dx;
    } else {
        //
        // dydx will never be referenced... honest!
        //

        dydx = 0;
    }

    x1 = px1;
    y1 = py1;

    while (1) {
        y2 = y1 + AA_PIX;
        y2 &= ~AA_PMN;

        if (y2 > py2) {
            y2 = py2;
        }

        dy = y2 - y1;

        if (dy == 0) {
            //
            // Hmm... do nowt.
            //
        } else {
            x2 = x1 + (dy * dxdy >> AA_FIX);

            AA_span_rhs(dydx, x1, y1, x2, y2);
        }

        if (y2 >= py2) {
            return;
        }

        x1 = x2;
        y1 = y2;
    }
}

//
// A line on the lhs of the triangle py1 <= py2
//

void AA_line_lhs(std::int32_t px1, std::int32_t py1, std::int32_t px2, std::int32_t py2) {
    if (py1 == py2) {
        return;
    }

    std::int32_t dy;
    std::int32_t dx;

    std::int32_t dydx;
    std::int32_t dxdy;

    std::int32_t x1, x2;
    std::int32_t y1, y2;

    dx = px2 - px1;
    dy = py2 - py1;
    dxdy = (dx << AA_FIX) / dy;

    if (dx) {
        dydx = dy << AA_FIX;
        dydx /= dx;
    } else {
        //
        // dydx will never be referenced... honest!
        //

        dydx = 0;
    }

    x1 = px1;
    y1 = py1;

    while (1) {
        y2 = y1 + AA_PIX;
        y2 &= ~AA_PMN;

        if (y2 > py2) {
            y2 = py2;
        }

        dy = y2 - y1;

        if (dy == 0) {
            //
            // Hmm... do nowt.
            //
        } else {
            x2 = x1 + (dy * dxdy >> AA_FIX);

            AA_span_lhs(dydx, x1, y1, x2, y2);
        }

        if (y2 >= py2) {
            return;
        }

        y1 = y2;
        x1 = x2;
    }
}

void AA_draw_do(
    std::uint8_t *bitmap,
    std::uint8_t x_res,
    std::uint8_t y_res,
    std::int32_t pitch,

    //
    // In clockwise order.
    //

    std::int32_t px[3],
    std::int32_t py[3]) {
    std::int32_t i;

    std::int32_t miny;
    std::int32_t maxy;

    // std::int32_t minp;
    std::int32_t p1;
    std::int32_t p2;

    std::int32_t x;
    std::int32_t y;

    std::int32_t x1, y1;
    std::int32_t x2, y2;

    std::int32_t nextp[3] = {1, 2, 0};

    std::int32_t val;

    std::int32_t fill_top;
    std::int32_t fill_bot;

    std::int32_t fill;
    std::uint8_t *line;

    //
    // The bounds of the triangle.
    //

    miny = py[0];
    maxy = py[0];

    if (py[1] < miny) {
        miny = py[1];
    }
    if (py[1] > maxy) {
        maxy = py[1];
    }

    if (py[2] < miny) {
        miny = py[2];
    }
    if (py[2] > maxy) {
        maxy = py[2];
    }

    fill_top = AA_PIX - (miny & AA_PMN);
    fill_bot = ((maxy - 1) & AA_PMN);

    miny >>= AA_FIX;
    maxy -= 1;
    maxy >>= AA_FIX;

    //
    // Initialise these guys.
    //

    for (i = miny; i <= maxy; i++) {
        ASSERT(WITHIN(i, 0, AA_MAX_SPAN_Y - 1));

        AA_span[i].lhs_min = +INFINITY;
        AA_span[i].lhs_max = -INFINITY;

        AA_span[i].rhs_min = +INFINITY;
        AA_span[i].rhs_max = -INFINITY;
    }

    //
    // Go through each line.
    //

    p1 = 0;

    for (i = 0; i < 3; i++) {
        p2 = nextp[p1];

        x1 = px[p1];
        y1 = py[p1];

        x2 = px[p2];
        y2 = py[p2];

        if (y1 < y2) {
            //
            // Running along the RHS of the triangle.
            //

            AA_line_rhs(x1, y1, x2, y2);
        } else {
            //
            // Running along the LHS of the triangle.
            //

            AA_line_lhs(x2, y2, x1, y1);
        }

        p1 = p2;
    }

    //
    // Draw the bitmap from the info in the spans.
    //

    fill = fill_top;

    for (y = miny; y <= maxy; y++) {
        if (y == maxy) {
            fill = fill_bot;
        }

        ASSERT(WITHIN(y, 0, AA_MAX_SPAN_Y - 1));

        AA_Span *as = &AA_span[y];

        line = &bitmap[y * pitch];

        x = as->lhs_min;

        while (x <= as->lhs_max) {
            ASSERT(WITHIN(x, 0, AA_MAX_SPAN_X - 1));

            val = line[x];
            val += as->pixel[x];

            SATURATE(val, 0, 255);

            line[x] = (std::uint8_t) val;

            x += 1;
        }

        while (x < as->rhs_min) {
            ASSERT(WITHIN(x, 0, x_res - 1));

            val = line[x];
            val += fill;

            SATURATE(val, 0, 255);

            line[x] = (std::uint8_t) val;

            x += 1;
        }

        while (x <= as->rhs_max) {
            ASSERT(WITHIN(x, 0, AA_MAX_SPAN_X - 1));

            val = line[x];
            val += as->pixel[x];

            SATURATE(val, 0, 255);

            line[x] = (std::uint8_t) val;

            x += 1;
        }

        fill = 255;
    }
}

//
// The value generate for
//

void AA_draw(
    std::uint8_t *bitmap,
    std::uint8_t x_res,
    std::uint8_t y_res,
    std::int32_t pitch,
    std::int32_t p1x, std::int32_t p1y, // 16-bit fixed point.
    std::int32_t p2x, std::int32_t p2y,
    std::int32_t p3x, std::int32_t p3y) {
#ifndef NDEBUG
    std::int32_t i;
#endif

    // std::int32_t miny;
    // std::int32_t maxy;

    std::int32_t minp;

    std::int32_t p1;
    std::int32_t p2;
    std::int32_t p3;

    // std::int32_t x;
    // std::int32_t y;

    std::int32_t px[3];
    std::int32_t py[3];

    // std::int32_t val;

    // std::int32_t  fill_top;
    // std::int32_t  fill_bot;

    // std::uint8_t  fill;
    // std::uint8_t *line;

    std::int32_t nextp[3] = {1, 2, 0};

    //
    // Work in AA_FIX-bit internally.
    //

    px[0] = p1x >> (16 - AA_FIX);
    py[0] = p1y >> (16 - AA_FIX);

    px[1] = p2x >> (16 - AA_FIX);
    py[1] = p2y >> (16 - AA_FIX);

    px[2] = p3x >> (16 - AA_FIX);
    py[2] = p3y >> (16 - AA_FIX);

    //
    // Lose the bottom few bits to preserve accuracy in
    // the triangle draws...
    //

    px[0] &= ~0xf;
    py[0] &= ~0xf;

    px[1] &= ~0xf;
    py[1] &= ~0xf;

    px[2] &= ~0xf;
    py[2] &= ~0xf;

#ifndef NDEBUG

    for (i = 0; i < 3; i++) {
        ASSERT(WITHIN(px[i], 0, AA_MAX_SPAN_X << AA_FIX));
        ASSERT(WITHIN(py[i], 0, AA_MAX_SPAN_Y << AA_FIX));
    }

#endif

    //
    // The maximum span
    //

    if (px[0] == AA_MAX_SPAN_X << AA_FIX) {
        px[0] -= 1;
    }
    if (px[1] == AA_MAX_SPAN_X << AA_FIX) {
        px[1] -= 1;
    }
    if (px[2] == AA_MAX_SPAN_X << AA_FIX) {
        px[2] -= 1;
    }

    if (py[0] == AA_MAX_SPAN_Y << AA_FIX) {
        py[0] -= 1;
    }
    if (py[1] == AA_MAX_SPAN_Y << AA_FIX) {
        py[1] -= 1;
    }
    if (py[2] == AA_MAX_SPAN_Y << AA_FIX) {
        py[2] -= 1;
    }

    //
    // What is the top point?
    //

    minp = 0;

    if (py[1] < py[0]) {
        minp = 1;
    }
    if (py[2] < py[minp]) {
        minp = 2;
    }

    p1 = minp;
    p2 = nextp[p1];
    p3 = nextp[p2];

    if (py[0] == py[1] ||
        py[1] == py[2] ||
        py[2] == py[0]) {
        //
        // This triangle is already flat along one edge!
        //

        std::int32_t tx[3];
        std::int32_t ty[3];

        tx[0] = px[p1];
        ty[0] = py[p1];

        tx[1] = px[p2];
        ty[1] = py[p2];

        tx[2] = px[p3];
        ty[2] = py[p3];

        AA_draw_do(
            bitmap,
            x_res,
            y_res,
            pitch,
            tx,
            ty);
    } else {
        if (py[p2] > py[p3]) {
            //
            // Pointy left triangle. Find the new point along the edge from p1 to p2.
            //

            std::int32_t dx = px[p2] - px[p1];
            std::int32_t dy = py[p2] - py[p1];

            std::int32_t nx;
            std::int32_t ny;

            nx = px[p1] + (dx * (py[p3] - py[p1])) / dy;
            ny = py[p3];

            //
            // Two new triangles.
            //

            std::int32_t tx[3];
            std::int32_t ty[3];

            tx[0] = px[p1];
            ty[0] = py[p1];

            tx[1] = nx;
            ty[1] = ny;

            tx[2] = px[p3];
            ty[2] = py[p3];

            AA_draw_do(
                bitmap,
                x_res,
                y_res,
                pitch,
                tx,
                ty);

            tx[0] = px[p3];
            ty[0] = py[p3];

            tx[1] = nx;
            ty[1] = ny;

            tx[2] = px[p2];
            ty[2] = py[p2];

            AA_draw_do(
                bitmap,
                x_res,
                y_res,
                pitch,
                tx,
                ty);
        } else {
            //
            // Pointy right triangle. Find the new point along the edge from p1 to p3.
            //

            std::int32_t dx = px[p3] - px[p1];
            std::int32_t dy = py[p3] - py[p1];

            std::int32_t nx;
            std::int32_t ny;

            nx = px[p1] + (dx * (py[p2] - py[p1])) / dy;
            ny = py[p2];

            //
            // Two new triangles.
            //

            std::int32_t tx[3];
            std::int32_t ty[3];

            tx[0] = px[p1];
            ty[0] = py[p1];

            tx[1] = px[p2];
            ty[1] = py[p2];

            tx[2] = nx;
            ty[2] = ny;

            AA_draw_do(
                bitmap,
                x_res,
                y_res,
                pitch,
                tx,
                ty);

            tx[0] = nx;
            ty[0] = ny;

            tx[1] = px[p2];
            ty[1] = py[p2];

            tx[2] = px[p3];
            ty[2] = py[p3];

            AA_draw_do(
                bitmap,
                x_res,
                y_res,
                pitch,
                tx,
                ty);
        }
    }
}
