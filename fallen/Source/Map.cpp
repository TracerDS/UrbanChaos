// Map.cpp
// Guy Simmons, 22nd October 1997.

#include "Game.h"

//---------------------------------------------------------------
#if !defined(PSX) && !defined(TARGET_DC)
void init_map() {
    memset((std::uint8_t*) MAP, 0, sizeof(MAP));
}
#endif
//---------------------------------------------------------------

std::int32_t MAP_light_get_height(std::int32_t x, std::int32_t z) {
    return 0;
}

LIGHT_Colour MAP_light_get_light(std::int32_t x, std::int32_t z) {
#ifdef TARGET_DC
    // Shouldn't be using this, apparently.
    ASSERT(false);
#endif

    MapElement* me;

    me = &MAP[MAP_INDEX(x, z)];

    return me->Color;
}

void MAP_light_set_light(std::int32_t x, std::int32_t z, LIGHT_Colour colour) {
#ifdef TARGET_DC
    // Shouldn't be using this, apparently.
    ASSERT(false);
#endif

    MapElement* me;

    me = &MAP[MAP_INDEX(x, z)];

    me->Color = colour;
}

LIGHT_Map MAP_light_map =
    {
        MAP_WIDTH,
        MAP_HEIGHT,
        MAP_light_get_height,
        MAP_light_get_light,
        MAP_light_set_light};
