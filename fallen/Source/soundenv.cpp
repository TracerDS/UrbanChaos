//
//  soundenv.cpp
//
//  Matthew Rosenfeld    16th Dec 98
//
//  Sound Environment -- basically, creating 3d polys for the 3d sound
//

#include "MFStdLib.h"
#include "game.h"
#include "memory.h"
#include "soundenv.h"
#include "pap.h"
#include "sound.h"

typedef void (*SOUNDENV_CB_fn)(std::int32_t, std::int32_t, std::int32_t, std::int32_t);

struct AudioGroundQuad {
    std::int32_t x, y, ox, oy;
};

AudioGroundQuad SOUNDENV_gndquads[64];
int SOUNDENV_gndctr;

// std::int32_t SOUNDENV_gndlist;

//------------------------------------------------------------------------
//   Workingsing stuff
//------------------------------------------------------------------------

bool SOUNDENV_ClearRange(char map[128][128], std::int32_t min, std::int32_t max, std::int32_t y) {
    std::int32_t x;

    for (x = min; x <= max; x++) {
        if (map[x][y]) return 0;
    }
    for (x = min; x <= max; x++) {
        map[x][y] |= 2;
    }
    return 1;
}

void SOUNDENV_Quadify(char map[128][128], std::int32_t mx, std::int32_t my, std::int32_t mox, std::int32_t moy, SOUNDENV_CB_fn CBfn) {
    std::int32_t x, y, blx, bly, blox, bloy;

    for (y = my; y < moy; y++) {
        for (x = mx; x < mox; x++) {
            if (!map[x][y]) {
                blx = blox = x;
                bly = bloy = y;
                while ((blox < mox) && !map[blox][y]) {
                    map[blox][y] |= 2;
                    blox++;
                }
                blox--;
                while ((bloy < moy - 1) && SOUNDENV_ClearRange(map, blx, blox, bloy + 1)) bloy++;
                CBfn(blx, bly, blox, bloy);
            }
        }
    }

    for (y = my; y < moy; y++)
        for (x = mx; x < mox; x++)
            map[x][y] &= 1;
}

//------------------------------------------------------------------------
//   Interfacing stuff
//------------------------------------------------------------------------

void cback(std::int32_t x, std::int32_t y, std::int32_t ox, std::int32_t oy) {
    SOUNDENV_gndquads[SOUNDENV_gndctr].x = x << 16;
    SOUNDENV_gndquads[SOUNDENV_gndctr].y = y << 16;
    SOUNDENV_gndquads[SOUNDENV_gndctr].ox = (ox + 1) << 16;
    SOUNDENV_gndquads[SOUNDENV_gndctr].oy = (oy + 1) << 16;
    SOUNDENV_gndctr++;
}

void SOUNDENV_precalc() {
    char tempmap[128][128];
    std::int32_t x, y;
    /*
      SOUNDENV_gndctr=0;
      for (x=0;x<128;x++)
              for (y=0;y<128;y++) {
                      tempmap[x][y]=(PAP_2HI(x,y).Flags & PAP_FLAG_SEWER_SQUARE) ? 1 : 0;
              }
      SOUNDENV_Quadify(tempmap,0,0,128,128,cback);
      */
}

extern std::int32_t CAM_pos_x,
    CAM_pos_y,
    CAM_pos_z;

void SOUNDENV_upload() {
#ifdef USE_A3D
    /*
            std::int32_t i,cx,cz;

            A3DGTag(1); // tag 1 is always going to be the ground.
    //	A3DGMaterial(A3D_MAT_CARPET);
            A3DGMaterial(A3D_MAT_SNDPROOF);

            for (i=0;i<SOUNDENV_gndctr;i++) {
                    A3DGQuad2D(SOUNDENV_gndquads[i].x,SOUNDENV_gndquads[i].y,SOUNDENV_gndquads[i].ox,SOUNDENV_gndquads[i].oy,-256);
            }

    */

#endif
}
