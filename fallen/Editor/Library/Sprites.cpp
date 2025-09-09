// Sprites.cpp
// Guy Simmons, 13th February 1997.

#include "Editor.hpp"
extern std::uint8_t CurrentPalette[256 * 3];
/*
#define	RGB_TO_RGB565(r,g,b)		(std::uint16_t)(((r>>4)<<11)|((g>>3)<<5)|(b>>4))
#define	RGB_TO_RGB888(r,g,b)		((r<<16)|(g<<8)|(b))
#define COL_TO_RGB565(col,PALETTE)	(std::uint16_t)(((PALETTE[(col)*3]>>3)<<11)|((PALETTE[(col)*3+1]>>2)<<5)|(PALETTE[(col)*3+2]>>3))
#define COL_TO_RGB888(col,PALETTE)	((PALETTE[(col)*3]<<16)|(PALETTE[(col)*3+1]<<8)|PALETTE[(col)*3+2])
*/
#define RGB_TO_RGB565(r, g, b) ((std::uint16_t) the_display.GetFormattedPixel(r, g, b))
#define RGB_TO_RGB888(r, g, b) ((r << 16) | (g << 8) | (b))
#define COL_TO_RGB565(col, PALETTE) ((std::uint16_t) the_display.GetFormattedPixel(PALETTE[(col) * 3], PALETTE[(col) * 3 + 1], PALETTE[(col) * 3 + 2]))
#define COL_TO_RGB888(col, PALETTE) ((PALETTE[(col) * 3] << 16) | (PALETTE[(col) * 3 + 1] << 8) | PALETTE[(col) * 3 + 2])

#define DRAW_SPRITE                       \
    while (1) {                           \
        packet = *src_ptr++;              \
        switch (packet) {                 \
        case END_LINE:                    \
            dst_ptr += WorkScreenWidth;   \
            line_ptr = dst_ptr;           \
            break;                        \
        case COPY_PIXELS:                 \
            c0 = (*src_ptr++) + 1;        \
            while (c0--)                  \
                *line_ptr++ = *src_ptr++; \
            break;                        \
        case SKIP_PIXELS:                 \
            line_ptr += (*src_ptr++) + 1; \
            break;                        \
        case DUPLICATE_PIXELS:            \
            c0 = (*src_ptr++) + 1;        \
            dup_pixel = *src_ptr++;       \
            while (c0--)                  \
                *line_ptr++ = dup_pixel;  \
            break;                        \
        case FINISHED:                    \
            return;                       \
        }                                 \
    }

#define DRAW_SPRITE16(pal)                                  \
    while (1) {                                             \
        packet = *src_ptr++;                                \
        switch (packet) {                                   \
        case END_LINE:                                      \
            dst_ptr += WorkScreenPixelWidth;                \
            line_ptr = dst_ptr;                             \
            break;                                          \
        case COPY_PIXELS:                                   \
            c0 = (*src_ptr++) + 1;                          \
            while (c0--) {                                  \
                *line_ptr++ = COL_TO_RGB565(*src_ptr, pal); \
                src_ptr++;                                  \
            }                                               \
            break;                                          \
        case SKIP_PIXELS:                                   \
            line_ptr += *(src_ptr++) + 1;                   \
            break;                                          \
        case DUPLICATE_PIXELS:                              \
            c0 = (*src_ptr++) + 1;                          \
            dup_pixel = COL_TO_RGB565(*src_ptr, pal);       \
            src_ptr++;                                      \
            while (c0--)                                    \
                *line_ptr++ = dup_pixel;                    \
            break;                                          \
        case FINISHED:                                      \
            return;                                         \
        }                                                   \
    }

#define DRAW_SPRITE32(pal)                                  \
    while (1) {                                             \
        packet = *src_ptr++;                                \
        switch (packet) {                                   \
        case END_LINE:                                      \
            dst_ptr += WorkScreenPixelWidth;                \
            line_ptr = dst_ptr;                             \
            break;                                          \
        case COPY_PIXELS:                                   \
            c0 = (*src_ptr++) + 1;                          \
            while (c0--) {                                  \
                *line_ptr++ = COL_TO_RGB888(*src_ptr, pal); \
                src_ptr++;                                  \
            }                                               \
            break;                                          \
        case SKIP_PIXELS:                                   \
            line_ptr += *(src_ptr++) + 1;                   \
            break;                                          \
        case DUPLICATE_PIXELS:                              \
            c0 = (*src_ptr++) + 1;                          \
            dup_pixel = COL_TO_RGB888(*src_ptr, pal);       \
            src_ptr++;                                      \
            while (c0--)                                    \
                *line_ptr++ = dup_pixel;                    \
            break;                                          \
        case FINISHED:                                      \
            return;                                         \
        }                                                   \
    }

#define DRAW_M_SPRITE                        \
    while (1) {                              \
        packet = *src_ptr++;                 \
        switch (packet) {                    \
        case END_LINE:                       \
            dst_ptr += WorkScreenPixelWidth; \
            line_ptr = dst_ptr;              \
            break;                           \
        case COPY_PIXELS:                    \
            c0 = (*src_ptr++) + 1;           \
            src_ptr += c0;                   \
            while (c0--)                     \
                *line_ptr++ = dup_pixel;     \
            break;                           \
        case SKIP_PIXELS:                    \
            line_ptr += (*src_ptr++) + 1;    \
            break;                           \
        case DUPLICATE_PIXELS:               \
            c0 = (*src_ptr++) + 1;           \
            src_ptr++;                       \
            while (c0--)                     \
                *line_ptr++ = dup_pixel;     \
            break;                           \
        case FINISHED:                       \
            return;                          \
        }                                    \
    }

#define V_SCAN                           \
    while (v_scan) {                     \
        packet = *src_ptr++;             \
        switch (packet) {                \
        case END_LINE:                   \
            v_scan--;                    \
            break;                       \
        case COPY_PIXELS:                \
            src_ptr += 1 + *(src_ptr++); \
            break;                       \
        case SKIP_PIXELS:                \
            src_ptr++;                   \
            break;                       \
        case DUPLICATE_PIXELS:           \
            src_ptr += 2;                \
            break;                       \
        }                                \
    }

#define L_SCAN                                   \
    while (l_scan && sprite_height) {            \
        packet = *src_ptr++;                     \
        switch (packet) {                        \
        case END_LINE:                           \
            dst_ptr += WorkScreenPixelWidth;     \
            line_ptr = dst_ptr;                  \
            sprite_height--;                     \
            pixel_count = 0;                     \
            break;                               \
        case COPY_PIXELS:                        \
            c0 = (*src_ptr++) + 1;               \
            if ((pixel_count + c0) >= l_scan) {  \
                src_ptr += l_scan - pixel_count; \
                c0 -= l_scan - pixel_count;      \
                pixel_count = l_scan;            \
                goto copy_pixels;                \
            } else {                             \
                pixel_count += c0;               \
                src_ptr += c0;                   \
            }                                    \
            break;                               \
        case SKIP_PIXELS:                        \
            c0 = (*src_ptr++) + 1;               \
            if ((pixel_count + c0) >= l_scan) {  \
                c0 -= l_scan - pixel_count;      \
                pixel_count = l_scan;            \
                goto skip_pixels;                \
            } else {                             \
                pixel_count += c0;               \
            }                                    \
            break;                               \
        case DUPLICATE_PIXELS:                   \
            c0 = (*src_ptr++) + 1;               \
            if ((pixel_count + c0) >= l_scan) {  \
                c0 -= l_scan - pixel_count;      \
                pixel_count = l_scan;            \
                goto duplicate_pixels;           \
            } else {                             \
                pixel_count += c0;               \
                src_ptr++;                       \
            }                                    \
            break;                               \
        case FINISHED:                           \
            return;                              \
        }                                        \
    }

#define R_SCAN                           \
    while (1) {                          \
        packet = *src_ptr++;             \
        switch (packet) {                \
        case END_LINE:                   \
            goto end_line;               \
        case COPY_PIXELS:                \
            src_ptr += 1 + *(src_ptr++); \
            break;                       \
        case SKIP_PIXELS:                \
            src_ptr++;                   \
            break;                       \
        case DUPLICATE_PIXELS:           \
            src_ptr += 2;                \
            break;                       \
        case FINISHED:                   \
            return;                      \
        }                                \
    }

//---------------------------------------------------------------

void DrawBSpritePal16(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint8_t* pal) {
    std::uint8_t packet,
        *src_ptr;
    std::uint32_t c0;
    std::uint16_t dup_pixel,
        *dst_ptr,
        *line_ptr;

    dst_ptr = (std::uint16_t*) WorkWindow + x + (y * WorkScreenPixelWidth);
    line_ptr = dst_ptr;
    src_ptr = the_sprite->SpriteData;
    DRAW_SPRITE16(pal)
}

void DrawBSpritePal32(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint8_t* pal) {
    std::uint8_t packet,
        *src_ptr;
    std::uint32_t c0;
    std::uint32_t dup_pixel,
        *dst_ptr,
        *line_ptr;

    dst_ptr = (std::uint32_t*) WorkWindow + x + (y * WorkScreenPixelWidth);
    line_ptr = dst_ptr;
    src_ptr = the_sprite->SpriteData;
    DRAW_SPRITE32(pal)
}

void DrawBSprite8(std::int32_t x, std::int32_t y, BSprite* the_sprite) {
    std::uint8_t dup_pixel,
        packet,
        *dst_ptr,
        *line_ptr,
        *src_ptr;
    std::uint32_t c0;

    dst_ptr = WorkWindow + x + (y * WorkScreenWidth);
    line_ptr = dst_ptr;
    src_ptr = the_sprite->SpriteData;
    DRAW_SPRITE
}

void DrawBSprite16(std::int32_t x, std::int32_t y, BSprite* the_sprite) {
    DrawBSpritePal16(x, y, the_sprite, CurrentPalette);
}

void DrawBSprite32(std::int32_t x, std::int32_t y, BSprite* the_sprite) {
    DrawBSpritePal32(x, y, the_sprite, CurrentPalette);
}

//---------------------------------------------------------------

void DrawMonoBSprite8(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint32_t colour) {
    std::uint8_t dup_pixel,
        packet,
        *dst_ptr,
        *line_ptr,
        *src_ptr;
    std::uint32_t c0;

    dst_ptr = WorkWindow + x + (y * WorkScreenWidth);
    line_ptr = dst_ptr;
    src_ptr = the_sprite->SpriteData;
    dup_pixel = (std::uint8_t) colour;
    DRAW_M_SPRITE
}

void DrawMonoBSprite16(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint32_t colour) {
    std::uint8_t packet,
        *src_ptr;
    std::uint32_t c0;

    std::uint16_t dup_pixel,
        *dst_ptr,
        *line_ptr;

    dst_ptr = (std::uint16_t*) WorkWindow + x + (y * WorkScreenPixelWidth);
    line_ptr = dst_ptr;
    src_ptr = the_sprite->SpriteData;
    dup_pixel = (std::uint16_t) colour;
    DRAW_M_SPRITE
}

void DrawMonoBSprite32(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint32_t colour) {
    std::uint8_t packet,
        *src_ptr;
    std::uint32_t c0;

    std::uint32_t dup_pixel,
        *dst_ptr,
        *line_ptr;

    dst_ptr = (std::uint32_t*) WorkWindow + x + (y * WorkScreenPixelWidth);
    line_ptr = dst_ptr;
    src_ptr = the_sprite->SpriteData;
    dup_pixel = (std::uint32_t) colour;
    DRAW_M_SPRITE
}

void DrawMonoBSpriteC8(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint32_t colour) {
    if (x < 0 || (x + the_sprite->SpriteWidth) >= WorkWindowWidth || y < 0 || (y + the_sprite->SpriteHeight) >= WorkWindowHeight)
        return;

    DrawMonoBSprite8(x, y, the_sprite, colour);
}

void DrawMonoBSpriteC16(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint32_t colour) {
    if (x < 0 || (x + the_sprite->SpriteWidth) >= WorkWindowWidth || y < 0 || (y + the_sprite->SpriteHeight) >= WorkWindowHeight)
        return;

    DrawMonoBSprite16(x, y, the_sprite, colour);
}

void DrawMonoBSpriteC32(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint32_t colour) {
    if (x < 0 || (x + the_sprite->SpriteWidth) >= WorkWindowWidth || y < 0 || (y + the_sprite->SpriteHeight) >= WorkWindowHeight)
        return;

    DrawMonoBSprite32(x, y, the_sprite, colour);
}

//---------------------------------------------------------------

void DrawBSpriteC8(std::int32_t x, std::int32_t y, BSprite* the_sprite) {
    std::uint8_t dup_pixel,
        packet,
        *dst_ptr,
        *line_ptr,
        *src_ptr;
    std::uint32_t c0,
        clip,
        count_diff,
        pixel_count,
        sprite_height;
    std::uint32_t l_scan,
        v_scan;
    std::int32_t sprite_x;

    if (x >= WorkWindowWidth || (x + the_sprite->SpriteWidth) < 0 || y >= WorkWindowHeight || (y + the_sprite->SpriteHeight) < 0)
        return;

    sprite_height = the_sprite->SpriteHeight;
    sprite_x = x;
    src_ptr = the_sprite->SpriteData;
    clip = 0;
    l_scan = 0;

    if (x < 0) {
        l_scan = -x;
        sprite_x = 0;
        clip = 1;
    }
    if (y < 0) {
        v_scan = -y;
        sprite_height += y;
        V_SCAN
        y = 0;
        clip = 1;
    }
    if ((std::int32_t) (x + the_sprite->SpriteWidth) >= WorkWindowWidth) {
        clip = 1;
    }
    if ((std::int32_t) (y + sprite_height) >= WorkWindowHeight) {
        sprite_height -= (y + sprite_height) - WorkWindowHeight;
        clip = 1;
    }

    dst_ptr = WorkWindow + sprite_x + (y * WorkScreenWidth);
    line_ptr = dst_ptr;

    if (clip) {
        pixel_count = 0;
        L_SCAN
        while (sprite_height) {
            packet = *src_ptr++;
            switch (packet) {
            case END_LINE:
            end_line:
                dst_ptr += WorkScreenWidth;
                line_ptr = dst_ptr;
                sprite_height--;
                pixel_count = 0;
                L_SCAN
                break;
            case COPY_PIXELS:
                c0 = (*src_ptr++) + 1;
            copy_pixels:
                pixel_count += c0;
                if ((std::int32_t) (x + pixel_count) >= WorkWindowWidth) {
                    count_diff = (x + pixel_count) - WorkWindowWidth;
                    c0 -= count_diff;
                    while (c0--)
                        *line_ptr++ = *src_ptr++;
                    src_ptr += count_diff;
                    R_SCAN
                } else {
                    while (c0--)
                        *line_ptr++ = *src_ptr++;
                }
                break;
            case SKIP_PIXELS:
                c0 = (*src_ptr++) + 1;
            skip_pixels:
                pixel_count += c0;
                if ((std::int32_t) (x + pixel_count) >= WorkWindowWidth) {
                    R_SCAN
                } else
                    line_ptr += c0;
                break;
            case DUPLICATE_PIXELS:
                c0 = (*src_ptr++) + 1;
            duplicate_pixels:
                pixel_count += c0;
                dup_pixel = *src_ptr++;
                if ((std::int32_t) (x + pixel_count) >= WorkWindowWidth) {
                    c0 -= (x + pixel_count) - WorkWindowWidth;
                    while (c0--)
                        *line_ptr++ = dup_pixel;
                    R_SCAN
                } else {
                    while (c0--)
                        *line_ptr++ = dup_pixel;
                }
                break;
            case FINISHED:
                return;
            }
        }
    } else {
        DRAW_SPRITE
    }
}

extern void DrawBSpritePalC16(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint8_t* pal);
extern void DrawBSpritePalC32(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint8_t* pal);

void DrawBSpriteC16(std::int32_t x, std::int32_t y, BSprite* the_sprite) {
    DrawBSpritePalC16(x, y, the_sprite, CurrentPalette);
}

void DrawBSpriteC32(std::int32_t x, std::int32_t y, BSprite* the_sprite) {
    DrawBSpritePalC32(x, y, the_sprite, CurrentPalette);
}

void DrawBSpritePalC16(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint8_t* pal) {
    std::uint8_t packet,
        *src_ptr;
    std::uint32_t c0,
        clip,
        count_diff,
        pixel_count,
        sprite_height;
    std::uint32_t l_scan,
        v_scan;
    std::int32_t sprite_x;

    std::uint16_t dup_pixel,
        *dst_ptr,
        *line_ptr;

    if (x >= WorkWindowWidth || (x + the_sprite->SpriteWidth) < 0 || y >= WorkWindowHeight || (y + the_sprite->SpriteHeight) < 0)
        return;

    sprite_height = the_sprite->SpriteHeight;
    sprite_x = x;
    src_ptr = the_sprite->SpriteData;
    clip = 0;
    l_scan = 0;

    if (x < 0) {
        l_scan = -x;
        sprite_x = 0;
        clip = 1;
    }
    if (y < 0) {
        v_scan = -y;
        sprite_height += y;
        V_SCAN
        y = 0;
        clip = 1;
    }
    if ((std::int32_t) (x + the_sprite->SpriteWidth) >= WorkWindowWidth) {
        clip = 1;
    }
    if ((std::int32_t) (y + sprite_height) >= WorkWindowHeight) {
        sprite_height -= (y + sprite_height) - WorkWindowHeight;
        clip = 1;
    }

    dst_ptr = (std::uint16_t*) WorkWindow + sprite_x + (y * WorkScreenPixelWidth);
    line_ptr = dst_ptr;

    if (clip) {
        pixel_count = 0;
        L_SCAN
        while (sprite_height) {
            packet = *src_ptr++;
            switch (packet) {
            case END_LINE:
            end_line:
                dst_ptr += WorkScreenPixelWidth;
                line_ptr = dst_ptr;
                sprite_height--;
                pixel_count = 0;
                L_SCAN
                break;
            case COPY_PIXELS:
                c0 = (*src_ptr++) + 1;
            copy_pixels:
                pixel_count += c0;
                if ((std::int32_t) (x + pixel_count) >= WorkWindowWidth) {
                    count_diff = (x + pixel_count) - WorkWindowWidth;
                    c0 -= count_diff;
                    while (c0--) {
                        *line_ptr++ = COL_TO_RGB565(*src_ptr, pal);
                        src_ptr++;
                    }
                    src_ptr += count_diff;
                    R_SCAN
                } else {
                    while (c0--) {
                        *line_ptr++ = COL_TO_RGB565(*src_ptr, pal);
                        src_ptr++;
                    }
                }
                break;
            case SKIP_PIXELS:
                c0 = (*src_ptr++) + 1;
            skip_pixels:
                pixel_count += c0;
                if ((std::int32_t) (x + pixel_count) >= WorkWindowWidth) {
                    R_SCAN
                } else
                    line_ptr += c0;
                break;
            case DUPLICATE_PIXELS:
                c0 = (*src_ptr++) + 1;
            duplicate_pixels:
                pixel_count += c0;
                dup_pixel = COL_TO_RGB565(*src_ptr, pal);
                src_ptr++;

                if ((std::int32_t) (x + pixel_count) >= WorkWindowWidth) {
                    c0 -= (x + pixel_count) - WorkWindowWidth;
                    while (c0--)
                        *line_ptr++ = dup_pixel;
                    R_SCAN
                } else {
                    while (c0--)
                        *line_ptr++ = dup_pixel;
                }
                break;
            case FINISHED:
                return;
            }
        }
    } else {
        DRAW_SPRITE16(pal)
    }
}

void DrawBSpritePalC32(std::int32_t x, std::int32_t y, BSprite* the_sprite, std::uint8_t* pal) {
    std::uint8_t packet,
        *src_ptr;
    std::uint32_t c0,
        clip,
        count_diff,
        pixel_count,
        sprite_height;
    std::uint32_t l_scan,
        v_scan;
    std::int32_t sprite_x;

    std::uint32_t dup_pixel,
        *dst_ptr,
        *line_ptr;

    if (x >= WorkWindowWidth || (x + the_sprite->SpriteWidth) < 0 || y >= WorkWindowHeight || (y + the_sprite->SpriteHeight) < 0)
        return;

    sprite_height = the_sprite->SpriteHeight;
    sprite_x = x;
    src_ptr = the_sprite->SpriteData;
    clip = 0;
    l_scan = 0;

    if (x < 0) {
        l_scan = -x;
        sprite_x = 0;
        clip = 1;
    }
    if (y < 0) {
        v_scan = -y;
        sprite_height += y;
        V_SCAN
        y = 0;
        clip = 1;
    }
    if ((std::int32_t) (x + the_sprite->SpriteWidth) >= WorkWindowWidth) {
        clip = 1;
    }
    if ((std::int32_t) (y + sprite_height) >= WorkWindowHeight) {
        sprite_height -= (y + sprite_height) - WorkWindowHeight;
        clip = 1;
    }

    dst_ptr = (std::uint32_t*) WorkWindow + sprite_x + (y * WorkScreenPixelWidth);
    line_ptr = dst_ptr;

    if (clip) {
        pixel_count = 0;
        L_SCAN
        while (sprite_height) {
            packet = *src_ptr++;
            switch (packet) {
            case END_LINE:
            end_line:
                dst_ptr += WorkScreenPixelWidth;
                line_ptr = dst_ptr;
                sprite_height--;
                pixel_count = 0;
                L_SCAN
                break;
            case COPY_PIXELS:
                c0 = (*src_ptr++) + 1;
            copy_pixels:
                pixel_count += c0;
                if ((std::int32_t) (x + pixel_count) >= WorkWindowWidth) {
                    count_diff = (x + pixel_count) - WorkWindowWidth;
                    c0 -= count_diff;
                    while (c0--) {
                        *line_ptr++ = COL_TO_RGB888(*src_ptr, pal);
                        src_ptr++;
                    }
                    src_ptr += count_diff;
                    R_SCAN
                } else {
                    while (c0--) {
                        *line_ptr++ = COL_TO_RGB888(*src_ptr, pal);
                        src_ptr++;
                    }
                }
                break;
            case SKIP_PIXELS:
                c0 = (*src_ptr++) + 1;
            skip_pixels:
                pixel_count += c0;
                if ((std::int32_t) (x + pixel_count) >= WorkWindowWidth) {
                    R_SCAN
                } else
                    line_ptr += c0;
                break;
            case DUPLICATE_PIXELS:
                c0 = (*src_ptr++) + 1;
            duplicate_pixels:
                pixel_count += c0;
                dup_pixel = COL_TO_RGB888(*src_ptr, pal);
                src_ptr++;
                if ((std::int32_t) (x + pixel_count) >= WorkWindowWidth) {
                    c0 -= (x + pixel_count) - WorkWindowWidth;
                    while (c0--)
                        *line_ptr++ = dup_pixel;
                    R_SCAN
                } else {
                    while (c0--)
                        *line_ptr++ = dup_pixel;
                }
                break;
            case FINISHED:
                return;
            }
        }
    } else {
        DRAW_SPRITE32(pal)
    }
}

//---------------------------------------------------------------

void SetupBSprites(BSprite* sprite_ref, std::uint8_t* sprite_data) {
    std::uint32_t spr_count;

    spr_count = *(std::uint32_t*) (&sprite_ref->SpriteHeight);
    sprite_ref++;
    while (spr_count--) {
        sprite_ref->SpriteData += (std::uint32_t) sprite_data;
        sprite_ref++;
    }
}

//---------------------------------------------------------------
