//
// An anti-aliased triangle draw.
//

#ifndef _AA_
#define _AA_

//
// The value generated for each pixel is the percentage area of
// that pixel covered by the triangle. This value is added with
// saturate to the value already in the pixel.
//
// The points must be given in clockwise order.
//

void AA_draw(
    std::uint8_t *bitmap,
    std::uint8_t x_res,
    std::uint8_t y_res,
    std::int32_t pitch,
    std::int32_t p1x, std::int32_t p1y, // 16-bit fixed point.
    std::int32_t p2x, std::int32_t p2y,
    std::int32_t p3x, std::int32_t p3y);

#endif
