// Move.cpp
// Guy Simmons, 27th March 1997.

#include "Editor.hpp"
#include "Structs.h"

#ifdef DOGPOO
//---------------------------------------------------------------

std::int32_t get_distance(Coord* position1, Coord* position2) {
    /*
            return	(
                                    Hypotenuse	(
                                                                    Hypotenuse	(
                                                                                                    position2->X-(std::int32_t)position1->X,
                                                                                                    position2->Z-(std::int32_t)position1->Z
                                                                                            ),
                                                                    position2->Y-(std::int32_t)position1->Y
                                                            )
                            );
    */
    return 0;
}

//---------------------------------------------------------------

std::int32_t get_approx_distance(Coord* position1, Coord* position2) {
    return 0;
}

//---------------------------------------------------------------

std::int32_t get_distance_xz(Coord* position1, Coord* position2) {
    /*
            return	(
                                    Hypotenuse	(
                                                                    position2->X-position1->X,
                                                                    position2->Z-position1->Z
                                                            )
                            );
    */
    return 0;
}

//---------------------------------------------------------------

std::int32_t get_angle_xz(Coord* position1, Coord* position2) {
    //	return(Arctan(position2->X-position1->X,position2->Z-position1->Z));
    return 0;
}

//---------------------------------------------------------------

std::int32_t get_angle_yz(Coord* position1, Coord* position2) {
    //	return(Arctan(position2->Y-position1->Y,-get_distance_xz(position1,position2)));
    return 0;
}

//---------------------------------------------------------------
#endif