
#include "Editor.hpp"

#include "undo.hpp"
#include "edit.h"
#include "prim.h"
#include "c:\fallen\headers\memory.h"

Undo::Undo(void) {
    index = 1;
    index_undo = 1;
}

void Undo::advance_current_undo(std::uint8_t undo_mode) {
    if (!undo_mode) {
        index++;
        if (index >= MAX_UNDO)
            index = 1;
    } else {
        index_undo++;
        if (index_undo >= MAX_UNDO)
            index_undo = 1;
    }
}

void Undo::retreat_current_undo(std::uint8_t undo_mode) {
    if (undo_mode) {
        index_undo--;
        if (index_undo < 1)
            index_undo = MAX_UNDO - 1;
    } else {
        index--;
        if (index < 1)
            index = MAX_UNDO - 1;
    }
}

void Undo::MoveTexture(std::uint8_t undo_mode, std::uint16_t page, std::uint16_t face, std::uint8_t u1, std::uint8_t v1, std::uint8_t u2, std::uint8_t v2, std::uint8_t u3, std::uint8_t v3, std::uint8_t u4, std::uint8_t v4) {
    struct GenericUndo *p_u;

    if (undo_mode)
        p_u = &undo_undo_info[index_undo];
    else
        p_u = &undo_info[index];

    p_u->Type = UNDO_MOVE_TEXTURE;
    p_u->Texture.Page = page;
    p_u->Texture.U[0] = u1;
    p_u->Texture.U[1] = u2;
    p_u->Texture.U[2] = u3;
    p_u->Texture.U[3] = u4;
    p_u->Texture.V[0] = v1;
    p_u->Texture.V[1] = v2;
    p_u->Texture.V[2] = v3;
    p_u->Texture.V[3] = v4;
    advance_current_undo(undo_mode);
}

void Undo::ApplyPrim4(std::uint8_t undo_mode, std::uint16_t face, PrimFace4 *the_prim4) {
    std::int32_t c0;
    struct GenericUndo *p_u;

    if (undo_mode)
        p_u = &undo_undo_info[index_undo];
    else
        p_u = &undo_info[index];

    p_u->Type = UNDO_APPLY_PRIM4;
    p_u->Texture.DrawFlags = the_prim4->DrawFlags;
    p_u->Texture.Color = the_prim4->Col2;
    p_u->Texture.Face = face;
    p_u->Texture.Page = the_prim4->TexturePage;
    for (c0 = 0; c0 < 4; c0++) {
        p_u->Texture.U[c0] = the_prim4->UV[c0][0];
        p_u->Texture.V[c0] = the_prim4->UV[c0][1];
    }
    advance_current_undo(undo_mode);
}

void Undo::ApplyPrim3(std::uint8_t undo_mode, std::uint16_t face, PrimFace3 *the_prim3) {
    std::int32_t c0;
    struct GenericUndo *p_u;

    if (undo_mode)
        p_u = &undo_undo_info[index_undo];
    else
        p_u = &undo_info[index];

    p_u->Type = UNDO_APPLY_PRIM3;
    p_u->Texture.DrawFlags = the_prim3->DrawFlags;
    p_u->Texture.Color = the_prim3->Col2;
    p_u->Texture.Face = face;
    p_u->Texture.Page = the_prim3->TexturePage;
    for (c0 = 0; c0 < 3; c0++) {
        p_u->Texture.U[c0] = the_prim3->UV[c0][0];
        p_u->Texture.V[c0] = the_prim3->UV[c0][1];
    }
    advance_current_undo(undo_mode);
}

void Undo::ApplyTexturePrim4(std::uint8_t undo_mode, std::uint16_t page, std::uint16_t face, std::uint8_t u1, std::uint8_t v1, std::uint8_t u2, std::uint8_t v2, std::uint8_t u3, std::uint8_t v3, std::uint8_t u4, std::uint8_t v4) {
    struct GenericUndo *p_u;

    if (undo_mode)
        p_u = &undo_undo_info[index_undo];
    else
        p_u = &undo_info[index];

    p_u->Type = UNDO_APPLY_TEXTURE_PRIM4;
    p_u->Texture.Face = face;
    p_u->Texture.Page = page;
    p_u->Texture.U[0] = u1;
    p_u->Texture.U[1] = u2;
    p_u->Texture.U[2] = u3;
    p_u->Texture.U[3] = u4;
    p_u->Texture.V[0] = v1;
    p_u->Texture.V[1] = v2;
    p_u->Texture.V[2] = v3;
    p_u->Texture.V[3] = v4;
    advance_current_undo(undo_mode);
}

void Undo::ApplyTexturePrim3(std::uint8_t undo_mode, std::uint16_t page, std::uint16_t face, std::uint8_t u1, std::uint8_t v1, std::uint8_t u2, std::uint8_t v2, std::uint8_t u3, std::uint8_t v3) {
    struct GenericUndo *p_u;

    if (undo_mode)
        p_u = &undo_undo_info[index_undo];
    else
        p_u = &undo_info[index];

    p_u->Type = UNDO_APPLY_TEXTURE_PRIM3;
    p_u->Texture.Face = face;
    p_u->Texture.Page = page;
    p_u->Texture.U[0] = u1;
    p_u->Texture.U[1] = u2;
    p_u->Texture.U[2] = u3;
    p_u->Texture.V[0] = v1;
    p_u->Texture.V[1] = v2;
    p_u->Texture.V[2] = v3;
    advance_current_undo(undo_mode);
}

void Undo::ApplyTextureCube(std::uint8_t undo_mode, std::uint16_t ele, std::uint16_t face, std::uint16_t text1, std::uint16_t text2) {
    struct GenericUndo *p_u;

    if (undo_mode)
        p_u = &undo_undo_info[index_undo];
    else
        p_u = &undo_info[index];

    p_u->Type = UNDO_APPLY_TEXTURE_CUBE;
    p_u->Ele.Ele = ele;
    p_u->Ele.Face = face;
    p_u->Ele.Text1 = text1;
    p_u->Ele.Text2 = text2;
    advance_current_undo(undo_mode);
}

void Undo::PlaceObject(std::uint8_t undo_mode, std::uint16_t prim, std::uint16_t thing, std::int32_t x, std::int32_t y, std::int32_t z) {
    struct GenericUndo *p_u;

    if (undo_mode)
        p_u = &undo_undo_info[index_undo];
    else
        p_u = &undo_info[index];

    p_u->Type = UNDO_PLACE_OBJECT;
    p_u->Object.Prim = prim;
    p_u->Object.Thing = thing;
    p_u->Object.X = x;
    p_u->Object.Y = y;
    p_u->Object.Z = z;
    advance_current_undo(undo_mode);
}

void Undo::PlaceCube(std::uint8_t undo_mode, std::uint16_t prev_cube, std::uint16_t cur_cube, std::int32_t x, std::int32_t y, std::int32_t z) {
    struct GenericUndo *p_u;

    if (undo_mode)
        p_u = &undo_undo_info[index_undo];
    else
        p_u = &undo_info[index];

    p_u->Type = UNDO_PLACE_OBJECT;
    p_u->Cube.PCube = prev_cube;
    p_u->Cube.CCube = cur_cube;
    p_u->Cube.X = x;
    p_u->Cube.Y = y;
    p_u->Cube.Z = z;
    advance_current_undo(undo_mode);
}

void Undo::DelObject(std::uint8_t undo_mode, std::uint16_t prim, std::uint16_t thing, std::int32_t x, std::int32_t y, std::int32_t z) {
    struct GenericUndo *p_u;

    if (undo_mode)
        p_u = &undo_undo_info[index_undo];
    else
        p_u = &undo_info[index];

    p_u->Type = UNDO_DEL_OBJECT;
    p_u->Object.Prim = prim;
    p_u->Object.Thing = thing;
    p_u->Object.X = x;
    p_u->Object.Y = y;
    p_u->Object.Z = z;
    advance_current_undo(undo_mode);
}

void Undo::MoveObject(std::uint8_t undo_mode, std::uint16_t thing, std::int32_t dx, std::int32_t dy, std::int32_t dz) {
    struct GenericUndo *p_u;

    if (undo_mode)
        p_u = &undo_undo_info[index_undo];
    else
        p_u = &undo_info[index];

    p_u->Type = UNDO_MOVE_OBJECT;
    p_u->Object.Thing = thing;
    p_u->Object.X = dx;
    p_u->Object.Y = dy;
    p_u->Object.Z = dz;
    advance_current_undo(undo_mode);
}

std::int32_t Undo::DoUndo(std::uint8_t undo_mode) {
    struct GenericUndo *p_u;
    std::int32_t c0,
        i;

    retreat_current_undo(undo_mode);
    if (undo_mode) {
        i = index_undo;
        p_u = &undo_undo_info[i];
    } else {
        i = index;
        p_u = &undo_info[i];
    }

    switch (p_u->Type) {
    case UNDO_APPLY_TEXTURE_PRIM4:
        prim_faces4[p_u->Texture.Face].TexturePage = p_u->Texture.Page;
        prim_faces4[p_u->Texture.Face].UV[0][0] = p_u->Texture.U[0];
        prim_faces4[p_u->Texture.Face].UV[1][0] = p_u->Texture.U[1];
        prim_faces4[p_u->Texture.Face].UV[2][0] = p_u->Texture.U[2];
        prim_faces4[p_u->Texture.Face].UV[3][0] = p_u->Texture.U[3];

        prim_faces4[p_u->Texture.Face].UV[0][1] = p_u->Texture.V[0];
        prim_faces4[p_u->Texture.Face].UV[1][1] = p_u->Texture.V[1];
        prim_faces4[p_u->Texture.Face].UV[2][1] = p_u->Texture.V[2];
        prim_faces4[p_u->Texture.Face].UV[3][1] = p_u->Texture.V[3];
        break;
    case UNDO_APPLY_TEXTURE_PRIM3:
        prim_faces3[p_u->Texture.Face].TexturePage = p_u->Texture.Page;
        prim_faces3[p_u->Texture.Face].UV[0][0] = p_u->Texture.U[0];
        prim_faces3[p_u->Texture.Face].UV[1][0] = p_u->Texture.U[1];
        prim_faces3[p_u->Texture.Face].UV[2][0] = p_u->Texture.U[2];

        prim_faces3[p_u->Texture.Face].UV[0][1] = p_u->Texture.V[0];
        prim_faces3[p_u->Texture.Face].UV[1][1] = p_u->Texture.V[1];
        prim_faces3[p_u->Texture.Face].UV[2][1] = p_u->Texture.V[2];
        break;
    case UNDO_APPLY_TEXTURE_CUBE:
        break;
    case UNDO_PLACE_OBJECT:
        map_things[p_u->Object.Thing].Type = 0;
        delete_thing_from_edit_map(p_u->Object.X, p_u->Object.Y, p_u->Object.Thing);
        break;
    case UNDO_DEL_OBJECT:
        break;
    case UNDO_MOVE_OBJECT:

        MoveObject(undo_mode ? 0 : 1, p_u->Object.Thing, map_things[p_u->Object.Thing].X, map_things[p_u->Object.Thing].Y, map_things[p_u->Object.Thing].Z);
        map_things[p_u->Object.Thing].X = p_u->Object.X;
        map_things[p_u->Object.Thing].Y = p_u->Object.Y;
        map_things[p_u->Object.Thing].Z = p_u->Object.Z;

        break;
    case UNDO_PLACE_CUBE:
        break;
    case UNDO_MOVE_TEXTURE:
        return (undo_mode ? -i : i);
        break;
    case UNDO_APPLY_PRIM4:
        prim_faces4[p_u->Texture.Face].DrawFlags = p_u->Texture.DrawFlags;
        prim_faces4[p_u->Texture.Face].Col2 = p_u->Texture.Color;
        prim_faces4[p_u->Texture.Face].TexturePage = p_u->Texture.Page;
        for (c0 = 0; c0 < 4; c0++) {
            prim_faces4[p_u->Texture.Face].UV[c0][0] = p_u->Texture.U[c0];
            prim_faces4[p_u->Texture.Face].UV[c0][1] = p_u->Texture.V[c0];
        }
        break;
    case UNDO_APPLY_PRIM3:
        prim_faces3[p_u->Texture.Face].DrawFlags = p_u->Texture.DrawFlags;
        prim_faces3[p_u->Texture.Face].Col2 = p_u->Texture.Color;
        prim_faces3[p_u->Texture.Face].TexturePage = p_u->Texture.Page;
        for (c0 = 0; c0 < 3; c0++) {
            prim_faces3[p_u->Texture.Face].UV[c0][0] = p_u->Texture.U[c0];
            prim_faces3[p_u->Texture.Face].UV[c0][1] = p_u->Texture.V[c0];
        }
        break;
    }
    p_u->Type = 0;
    return (0);
}
