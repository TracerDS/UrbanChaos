// TexTab.hpp
// Guy Simmons, 20th February 1997

#ifndef _UNDO_HPP_
#define _UNDO_HPP_

#include "Prim.h"
#define MAX_UNDO 100

#define UNDO_APPLY_TEXTURE_PRIM4 1
#define UNDO_APPLY_TEXTURE_PRIM3 2
#define UNDO_APPLY_TEXTURE_CUBE 3
#define UNDO_PLACE_OBJECT 4
#define UNDO_DEL_OBJECT 5
#define UNDO_MOVE_OBJECT 6
#define UNDO_PLACE_CUBE 7
#define UNDO_MOVE_TEXTURE 8
#define UNDO_APPLY_PRIM4 9
#define UNDO_APPLY_PRIM3 10

struct GenericUndo {
    std::uint8_t Type;
    union {
        struct
        {
            std::uint8_t DrawFlags;
            std::uint8_t Color;
            std::uint16_t Face;
            std::uint16_t Page;
            std::uint8_t U[4];
            std::uint8_t V[4];
        } Texture;
        struct
        {
            std::uint16_t Entity;
            std::uint16_t Prim;
            std::int32_t X;
            std::int32_t Y;
            std::int32_t Z;
        } Object;
        struct
        {
            std::uint16_t PCube;
            std::uint16_t CCube;
            std::int32_t X;
            std::int32_t Y;
            std::int32_t Z;
        } Cube;
        struct
        {
            std::uint16_t Ele;
            std::uint16_t Face;
            std::uint16_t Text1;
            std::uint16_t Text2;
        } Ele;
    };
};

class Undo {
   private:
    void advance_current_undo(std::uint8_t undo_mode);
    void retreat_current_undo(std::uint8_t undo_mode);
    std::int16_t index;
    std::int16_t index_undo;

   public:
    Undo();
    void ApplyPrim4(std::uint8_t undo_mode, std::uint16_t face, PrimFace4 *the_prim4);
    void ApplyPrim3(std::uint8_t undo_mode, std::uint16_t face, PrimFace3 *the_prim3);
    void ApplyTexturePrim4(std::uint8_t undo_mode, std::uint16_t page, std::uint16_t face, std::uint8_t u1, std::uint8_t v1, std::uint8_t u2, std::uint8_t v2, std::uint8_t u3, std::uint8_t v3, std::uint8_t u4, std::uint8_t v4);
    void ApplyTexturePrim3(std::uint8_t undo_mode, std::uint16_t page, std::uint16_t face, std::uint8_t u1, std::uint8_t v1, std::uint8_t u2, std::uint8_t v2, std::uint8_t u3, std::uint8_t v3);
    void ApplyTextureCube(std::uint8_t undo_mode, std::uint16_t ele, std::uint16_t face, std::uint16_t text1, std::uint16_t text2);
    void MoveTexture(std::uint8_t undo_mode, std::uint16_t page, std::uint16_t face, std::uint8_t u1, std::uint8_t v1, std::uint8_t u2, std::uint8_t v2, std::uint8_t u3, std::uint8_t v3, std::uint8_t u4, std::uint8_t v4);
    void PlaceObject(std::uint8_t undo_mode, std::uint16_t prim, std::uint16_t thing, std::int32_t x, std::int32_t y, std::int32_t z);
    void MoveObject(std::uint8_t undo_mode, std::uint16_t thing, std::int32_t dx, std::int32_t dy, std::int32_t dz);
    void DelObject(std::uint8_t undo_mode, std::uint16_t prim, std::uint16_t thing, std::int32_t x, std::int32_t y, std::int32_t z);
    void PlaceCube(std::uint8_t undo_mode, std::uint16_t prev_cube, std::uint16_t cur_cube, std::int32_t x, std::int32_t y, std::int32_t z);
    std::int32_t DoUndo(std::uint8_t undo_mode);

    GenericUndo undo_info[MAX_UNDO];
    GenericUndo undo_undo_info[MAX_UNDO];
};

#endif
