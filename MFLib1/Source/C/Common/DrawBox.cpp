// DrawBox.cpp
// Guy Simmons, 11th February 1997.

#include <MFHeader.h>

#define COLOUR_TO_LONG(c) (0x01010101 * c)
#define COLOURW_TO_LONG(c) (0x00010001 * c)

//---------------------------------------------------------------

void DrawBox8(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour) {
    std::uint8_t* ptr;
    std::uint32_t big_col,
        *ptr_big,
        w2;
    std::int32_t temp_width;

    ptr = WorkWindow + x + (y * WorkScreenWidth); // have to use window
    temp_width = width;
    if (width <= 4) {
        // Thin rectangles.
        for (; height >= 0; height--) {
            for (width = temp_width; width > 0; width--) {
                *ptr++ = (std::uint8_t) colour;
            }
            ptr += WorkScreenWidth - temp_width;
        }
    } else {
        // Fat rectangles.
        big_col = COLOUR_TO_LONG(colour);

        for (; height >= 0; height--) {
            for (width = x & 3; width > 0; width--) {
                *ptr++ = (std::uint8_t) colour;
            }
            w2 = temp_width - (x & 3);
            ptr_big = (std::uint32_t*) ptr;
            for (width = w2 >> 2; width > 0; width--) {
                *ptr_big++ = big_col;
            }
            ptr = (std::uint8_t*) ptr_big;
            for (width = w2 & 3; width > 0; width--) {
                *ptr++ = (std::uint8_t) colour;
            }
            ptr += WorkScreenWidth - temp_width;
        }
    }
}

void DrawBox16(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour) {
    std::uint16_t* ptr;
    std::uint32_t big_col,
        *ptr_big,
        w2;
    std::int32_t temp_width;

    ptr = (std::uint16_t*) WorkWindow + x + (y * WorkScreenWidth >> 1); // have to use window
    temp_width = width;
    if (width <= 2) {
        // Thin rectangles.
        for (; height >= 0; height--) {
            for (width = temp_width; width > 0; width--) {
                *ptr++ = (std::uint16_t) colour;
            }
            ptr += WorkScreenWidth - temp_width;
        }
    } else {
        // Fat rectangles.
        big_col = COLOURW_TO_LONG(colour);

        for (; height >= 0; height--) {
            for (width = x & 1; width > 0; width--) {
                *ptr++ = (std::uint16_t) colour;
            }
            w2 = temp_width - (x & 1);
            ptr_big = (std::uint32_t*) ptr;
            for (width = w2 >> 1; width > 0; width--) {
                *ptr_big++ = big_col;
            }
            ptr = (std::uint16_t*) ptr_big;
            for (width = w2 & 1; width > 0; width--) {
                *ptr++ = (std::uint8_t) colour;
            }
            ptr += (WorkScreenWidth >> 1) - temp_width;
        }
    }
}

void DrawBox32(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour) {
    std::uint32_t* ptr;
    std::int32_t temp_width;

    ptr = (std::uint32_t*) WorkWindow + x + (y * WorkScreenWidth >> 2); // have to use window
    temp_width = width;
    {
        // Thin rectangles.
        for (; height >= 0; height--) {
            for (width = temp_width; width > 0; width--) {
                *ptr++ = colour;
            }
            ptr += ((WorkScreenWidth >> 2) - temp_width);
        }
    }
}

//---------------------------------------------------------------

void DrawBoxC8(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour) {
    std::uint8_t* ptr;
    std::uint32_t big_col,
        *ptr_big,
        w2;
    std::int32_t temp_width;

    if (x >= WorkWindowWidth || y >= WorkWindowHeight)
        return;

    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    if ((x + width) >= WorkWindowWidth) {
        width -= (x + width) - WorkWindowWidth;
    }
    if ((y + height) >= WorkWindowHeight) {
        height -= (y + height) - WorkWindowHeight;
    }

    if (width <= 0 || height <= 0)
        return;

    ptr = WorkWindow + x + (y * WorkScreenWidth); // have to use window
    temp_width = width;
    if (width <= 4) {
        // Thin rectangles.
        for (; height; height--) {
            for (width = temp_width; width > 0; width--) {
                *ptr++ = (std::uint8_t) colour;
            }
            ptr += WorkScreenWidth - temp_width;
        }
    } else {
        // Fat rectangles.
        big_col = COLOUR_TO_LONG(colour);

        for (; height; height--) {
            for (width = x & 3; width > 0; width--) {
                *ptr++ = (std::uint8_t) colour;
            }
            w2 = temp_width - (x & 3);
            ptr_big = (std::uint32_t*) ptr;
            for (width = w2 >> 2; width > 0; width--) {
                *ptr_big++ = big_col;
            }
            ptr = (std::uint8_t*) ptr_big;
            for (width = w2 & 3; width > 0; width--) {
                *ptr++ = (std::uint8_t) colour;
            }
            ptr += WorkScreenWidth - temp_width;
        }
    }
}

void DrawBoxC16(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour) {
    std::uint16_t* ptr;
    std::uint32_t big_col,
        *ptr_big,
        w2;
    std::int32_t temp_width;

    if (x >= WorkWindowWidth || y >= WorkWindowHeight)
        return;

    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    if ((x + width) >= WorkWindowWidth) {
        width -= (x + width) - WorkWindowWidth;
    }
    if ((y + height) >= WorkWindowHeight) {
        height -= (y + height) - WorkWindowHeight;
    }

    if (width <= 0 || height <= 0)
        return;

    ptr = (std::uint16_t*) WorkWindow + x + (y * WorkScreenWidth >> 1); // have to use window
    temp_width = width;
    if (width <= 2) {
        // Thin rectangles.
        for (; height; height--) {
            for (width = temp_width; width > 0; width--) {
                *ptr++ = (std::uint16_t) colour;
            }
            ptr += (WorkScreenWidth >> 1) - temp_width;
        }
    } else {
        // Fat rectangles.
        big_col = COLOURW_TO_LONG(colour);

        for (; height; height--) {
            for (width = x & 1; width > 0; width--) {
                *ptr++ = (std::uint16_t) colour;
            }
            w2 = temp_width - (x & 1);
            ptr_big = (std::uint32_t*) ptr;
            for (width = w2 >> 1; width > 0; width--) {
                *ptr_big++ = big_col;
            }
            ptr = (std::uint16_t*) ptr_big;
            for (width = w2 & 1; width > 0; width--) {
                *ptr++ = (std::uint16_t) colour;
            }
            ptr += (WorkScreenWidth >> 1) - temp_width;
        }
    }
}

void DrawBoxC32(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height, std::uint32_t colour) {
    std::uint32_t* ptr;
    std::int32_t temp_width;

    if (x >= WorkWindowWidth || y >= WorkWindowHeight)
        return;

    if (x < 0) {
        width += x;
        x = 0;
    }
    if (y < 0) {
        height += y;
        y = 0;
    }
    if ((x + width) >= WorkWindowWidth) {
        width -= (x + width) - WorkWindowWidth;
    }
    if ((y + height) >= WorkWindowHeight) {
        height -= (y + height) - WorkWindowHeight;
    }

    if (width <= 0 || height <= 0)
        return;

    ptr = (std::uint32_t*) WorkWindow + (x) + (y * WorkScreenWidth >> 2); // have to use window
    temp_width = width;

    for (; height; height--) {
        for (width = temp_width; width > 0; width--) {
            *ptr++ = colour;
        }
        ptr += ((WorkScreenWidth >> 2) - temp_width);
    }
}

//---------------------------------------------------------------
