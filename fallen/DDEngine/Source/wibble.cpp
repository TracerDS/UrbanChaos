#include "game.h"
#include <ddlib.h>

std::int32_t mul_y1;
std::int32_t mul_y2;
std::int32_t mul_g1;
std::int32_t mul_g2;

std::int32_t shift1;
std::int32_t shift2;

void WIBBLE_simple(
    std::int32_t x1, std::int32_t y1,
    std::int32_t x2, std::int32_t y2,
    std::uint8_t wibble_y1,
    std::uint8_t wibble_y2,
    std::uint8_t wibble_g1,
    std::uint8_t wibble_g2,
    std::uint8_t wibble_s1,
    std::uint8_t wibble_s2) {
    std::int32_t y;
    std::int32_t offset;
    std::int32_t count;
    std::int32_t angle1;
    std::int32_t angle2;

    x1 = x1 * RealDisplayWidth / DisplayWidth;
    x2 = x2 * RealDisplayWidth / DisplayWidth;
    y1 = y1 * RealDisplayHeight / DisplayHeight;
    y2 = y2 * RealDisplayHeight / DisplayHeight;

    if (the_display.CurrMode->GetBPP() == 16) {
        std::uint16_t* dest;
        std::uint16_t* src;
        std::uint8_t* base = &the_display.screen[x1 + x1 + y1 * the_display.screen_pitch];

        for (y = y1; y < y2; y++) {
            angle1 = y * wibble_y1;
            angle2 = y * wibble_y2;
            angle1 += GAME_TURN * wibble_g1;
            angle2 += GAME_TURN * wibble_g2;

            angle1 &= 2047;
            angle2 &= 2047;

            offset = SIN(angle1) * wibble_s1 >> 19;
            offset += COS(angle2) * wibble_s2 >> 19;

            if (offset == 0) {
                //
                // Easy!
                //
            } else if (offset > 0) {
                dest = (std::uint16_t*) base;
                src = (std::uint16_t*) base;
                src += offset;

                count = x2 - x1;

                while (count-- > 0) {
                    *dest++ = *src++;
                }
            } else {
                count = x2 - x1;

                dest = (std::uint16_t*) base;
                src = (std::uint16_t*) base;

                dest += count - 1;
                src += count - 1;

                src += offset;

                while (count-- > 0) {
                    *dest-- = *src--;
                }
            }

            base += the_display.screen_pitch;
        }
    } else {
        // cut-and-paste, but I don't care anymore
        std::uint32_t* dest;
        std::uint32_t* src;
        std::uint8_t* base = &the_display.screen[x1 * 4 + y1 * the_display.screen_pitch];

        for (y = y1; y < y2; y++) {
            angle1 = y * wibble_y1;
            angle2 = y * wibble_y2;
            angle1 += GAME_TURN * wibble_g1;
            angle2 += GAME_TURN * wibble_g2;

            angle1 &= 2047;
            angle2 &= 2047;

            offset = SIN(angle1) * wibble_s1 >> 19;
            offset += COS(angle2) * wibble_s2 >> 19;

            if (offset == 0) {
                //
                // Easy!
                //
            } else if (offset > 0) {
                dest = (std::uint32_t*) base;
                src = (std::uint32_t*) base;
                src += offset;

                count = x2 - x1;

                while (count-- > 0) {
                    *dest++ = *src++;
                }
            } else {
                count = x2 - x1;

                dest = (std::uint32_t*) base;
                src = (std::uint32_t*) base;

                dest += count - 1;
                src += count - 1;

                src += offset;

                while (count-- > 0) {
                    *dest-- = *src--;
                }
            }

            base += the_display.screen_pitch;
        }
    }
}
