//
// Wibbles the given bit of screen.
//

#pragma once



//
// ALL THESE FUNCTIONS MUST ONLY BE CALLED WHEN THE SCREEN IS LOCKED.
//
// Make sure the bounding boxes are all well on screen!
//

void WIBBLE_simple(
		std::int32_t x1, std::int32_t y1,
		std::int32_t x2, std::int32_t y2,
		std::uint8_t wibble_y1,
		std::uint8_t wibble_y2,
		std::uint8_t wibble_g1,
		std::uint8_t wibble_g2,
		std::uint8_t wibble_s1,
		std::uint8_t wibble_s2);



