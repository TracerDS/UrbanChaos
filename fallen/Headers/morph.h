//
// Morphing keyframes.
//

#ifndef _MORPH_
#define _MORPH_

//
// Loads all the morphs in.
//

void MORPH_load();

//
// Accessing the morphs.
//

#define MORPH_PIGEON_WALK1 0
#define MORPH_PIGEON_WALK2 1
#define MORPH_PIGEON_STAND 0
#define MORPH_PIGEON_PECK 0
#define MORPH_PIGEON_HEADCOCK 0
#define MORPH_NUMBER 2

typedef struct
{
    std::int16_t x;
    std::int16_t y;
    std::int16_t z;

} MORPH_Point;

MORPH_Point *MORPH_get_points(std::int32_t morph);
std::int32_t MORPH_get_num_points(std::int32_t morph);

#endif
