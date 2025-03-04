//
// Anti-alaised triangle draw mark 2
//

#pragma once


//
// A clipped anti-aliased triangle draw, where each
// pixel is drawn additive.
//

void AB_draw(std::uint8_t* bitmap, std::int32_t xres, std::int32_t num_points, std::int32_t px[], std::int32_t py[]);