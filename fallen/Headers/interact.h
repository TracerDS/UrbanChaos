#ifndef INTERACT_H
#define INTERACT_H

#include "building.h"
#include "Thing.h"

//
// Defines
//

#ifdef TARGET_DC
#define MAX_ANIM_CHUNKS 16 // 256
#define MAX_GAME_CHUNKS 5  // 256
#else
#define MAX_ANIM_CHUNKS 16 // 256
#define MAX_GAME_CHUNKS 5  // 256
#endif

//
// Structs
//

//
// Data
//
/*
extern struct KeyFrame	*anim_array[300],
                                                *cop_array[300],
                                                *darci_array[300],
                                                *van_array[20],
                                                *roper_array[300],
                                                *thug_array[300];
*/

extern struct GameKeyFrame *global_anim_array[4][450];

extern struct KeyFrameChunk *test_chunk;
#if !defined(PSX) && !defined(TARGET_DC)
extern struct KeyFrameChunk test_chunk2,
    test_chunk3,
    thug_chunk;
#endif

extern struct KeyFrameElement *the_elements;
extern struct GameKeyFrameChunk game_chunk[MAX_GAME_CHUNKS];
extern struct GameKeyFrameChunk anim_chunk[MAX_ANIM_CHUNKS];

//
// The bounding boxes of the animating prims.
//

typedef struct
{
    std::int32_t minx;
    std::int32_t miny;
    std::int32_t minz;

    std::int32_t maxx;
    std::int32_t maxy;
    std::int32_t maxz;

} AnimPrimBbox;

extern AnimPrimBbox anim_prim_bbox[MAX_ANIM_CHUNKS];

//
// Functions
//

extern std::int32_t find_grab_face(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t radius,
    std::int32_t dy,
    std::int32_t angle,
    std::int32_t *grab_x,
    std::int32_t *grab_y,
    std::int32_t *grab_z,
    std::int32_t *grab_angle,
    std::int32_t ignore_faces_from_this_building,
    std::int32_t trench,
    std::int32_t *type,
    Thing *p_person);

#if !defined(PSX) && !defined(TARGET_DC)
extern std::int32_t find_grab_face_in_sewers(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t radius,
    std::int32_t dy,
    std::int32_t angle,
    std::int32_t *grab_x,
    std::int32_t *grab_y,
    std::int32_t *grab_z,
    std::int32_t *grab_angle);
#endif

extern void calc_sub_objects_position(Thing *p_mthing, std::int32_t tween, std::uint16_t object, std::int32_t *x, std::int32_t *y, std::int32_t *z);
extern void calc_sub_objects_position_keys(Thing *p_mthing, std::int32_t tween, std::uint16_t object, std::int32_t *x, std::int32_t *y, std::int32_t *z, struct GameKeyFrame *frame1, struct GameKeyFrame *frame2);
extern void calc_sub_objects_position_global(GameKeyFrame *cur_frame, GameKeyFrame *next_frame, std::int32_t tween, std::uint16_t object, std::int32_t *x, std::int32_t *y, std::int32_t *z);

//
// Returns the height of the cable at the given length along it.
// The length goes from 1 to CABLE_ALONG_MAX (defined in building.h)
//

std::int32_t find_cable_y_along(struct DFacet *p_facet, std::int32_t along); // 0 <= along <= CABLE_MAX_ALONG

#endif
