#ifndef COLLIDE_GAME
#define COLLIDE_GAME

#include "pap.h"

//
// Defines
//
#ifdef PSX
#define MAX_COL_VECT_LINK 4000
#define MAX_COL_VECT 1000
#define MAX_WALK_POOL (10000)
#else
#define MAX_COL_VECT_LINK 10000
#define MAX_COL_VECT 10000
#define MAX_WALK_POOL (30000)
#endif

#define DONT_INTERSECT 0
#define DO_INTERSECT 1
#define COLLINEAR 2

//
// Structures
//

struct CollisionVectLink {
    std::uint16_t Next; // linked list of collision vectors
    std::uint16_t VectIndex;
};

struct CollisionVect {
    std::int32_t X[2];
    std::int16_t Y[2];
    std::int32_t Z[2];
    std::uint8_t PrimType; // A storey type...
    std::uint8_t PrimExtra;
    std::int16_t Face;
};

struct WalkLink {
    std::uint16_t Next;
    std::int16_t Face;
};
//
// Vars
//

extern struct CollisionVectLink col_vects_links[MAX_COL_VECT_LINK];
extern struct CollisionVect col_vects[MAX_COL_VECT];

extern std::uint16_t next_col_vect;
extern std::uint16_t next_col_vect_link;

//
// Functions
//

extern struct WalkLink walk_links[MAX_WALK_POOL];
extern std::uint16_t next_walk_link;

extern std::int32_t get_fence_height(std::int32_t h);
extern std::uint8_t two4_line_intersection(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::int32_t x3, std::int32_t y3, std::int32_t x4, std::int32_t y4);
extern std::int32_t lines_intersect(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::int32_t x3, std::int32_t y3, std::int32_t x4, std::int32_t y4, std::int32_t *x, std::int32_t *y);
extern std::int32_t insert_collision_vect(std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t x2, std::int32_t y2, std::int32_t z2, std::uint8_t prim, std::uint8_t prim_extra, std::int16_t face);
extern void remove_collision_vect(std::uint16_t vect);

extern std::int32_t get_point_dist_from_col_vect(std::int32_t vect, std::int32_t x, std::int32_t z, std::int32_t *ret_x, std::int32_t *ret_z, std::int32_t new_dist);
extern std::int32_t check_vect_circle(std::int32_t m_dx, std::int32_t m_dy, std::int32_t m_dz, Thing *p_thing, std::int32_t radius);
extern std::uint32_t move_thing(std::int32_t m_dx, std::int32_t m_dy, std::int32_t m_dz, Thing *p_thing);
extern std::uint32_t move_thing_quick(std::int32_t dx, std::int32_t dy, std::int32_t dz, Thing *p_thing);
extern std::int32_t check_vect_vect(std::int32_t m_dx, std::int32_t m_dy, std::int32_t m_dz, Thing *p_thing, std::int32_t scale);
extern std::int32_t find_face_for_this_pos(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t *ret_y, std::int32_t ignore_faces_of_this_building, std::uint8_t flag);
extern std::int32_t dist_to_line(std::int32_t x1, std::int32_t z1, std::int32_t x2, std::int32_t z2, std::int32_t a, std::int32_t b);
extern void correct_pos_for_ladder(struct DFacet *p_facet, std::int32_t *px, std::int32_t *pz, std::int32_t *angle, std::int32_t scale);
extern std::int32_t height_above_anything(Thing *p_person, std::int32_t body_part, std::int16_t *onface);
extern std::int32_t plant_feet(Thing *p_person);

//
// Early-out tests for box-box circle-box collision. These functions
// return true if the primitives don't collide for certain.
//

void box_box_early_out(
    std::int32_t box1_mid_x,
    std::int32_t box1_mid_z,
    std::int32_t box1_min_x,
    std::int32_t box1_min_z,
    std::int32_t box1_max_x,
    std::int32_t box1_max_z,
    std::int32_t box1_yaw,
    std::int32_t box2_mid_x,
    std::int32_t box2_mid_z,
    std::int32_t box2_min_x,
    std::int32_t box2_min_z,
    std::int32_t box2_max_x,
    std::int32_t box2_max_z,
    std::int32_t box2_yaw);

void box_circle_early_out(
    std::int32_t box1_mid_x,
    std::int32_t box1_mid_z,
    std::int32_t box1_min_x,
    std::int32_t box1_min_z,
    std::int32_t box1_max_x,
    std::int32_t box1_max_z,
    std::int32_t box1_yaw,
    std::int32_t cx,
    std::int32_t cz,
    std::int32_t cradius);

//
// Slides the given vector against a circle.
// Returns true if a collision occurred.
//

std::int32_t slide_around_circle(
    std::int32_t cx,
    std::int32_t cz,
    std::int32_t cradius,

    std::int32_t x1,
    std::int32_t z1,
    std::int32_t *x2,
    std::int32_t *z2);

//
// Slides the given vector against a box. Returns true if
// a collision occurred.
//

std::int32_t slide_around_box(
    std::int32_t box_mid_x,
    std::int32_t box_mid_z,
    std::int32_t box_min_x,
    std::int32_t box_min_z,
    std::int32_t box_max_x,
    std::int32_t box_max_z,
    std::int32_t box_yaw,
    std::int32_t radius, // How far from the box the vector should be from.

    std::int32_t x1, // The start of the vector is ignored! So you can use this
    std::int32_t z1, // function to slide a circle around the box too.
    std::int32_t *x2,
    std::int32_t *z2);

//
// Returns true if a collision vector intersects a rotated box
//

std::int32_t collide_box(
    std::int32_t midx,
    std::int32_t midy,
    std::int32_t midz,
    std::int32_t minx, std::int32_t minz,
    std::int32_t maxx, std::int32_t maxz,
    std::int32_t yaw);

//
// Returns true if the given line intersects the rotated bounding box.
//

std::int32_t collide_box_with_line(
    std::int32_t midx,
    std::int32_t midz,
    std::int32_t minx, std::int32_t minz,
    std::int32_t maxx, std::int32_t maxz,
    std::int32_t yaw,
    std::int32_t lx1,
    std::int32_t lz1,
    std::int32_t lx2,
    std::int32_t lz2);

//
// Slides the given vector against the sausage.  Returns true
// if a slide-along occured.
//

std::int32_t slide_around_sausage(
    std::int32_t sx1,
    std::int32_t sz1,
    std::int32_t sx2,
    std::int32_t sz2,
    std::int32_t sradius,

    std::int32_t x1,
    std::int32_t z1,
    std::int32_t *x2,
    std::int32_t *z2);

//
// Returns the colvect of a nearby ladder that the thing can mount.
// Makes the thing mount the given ladder... Returns true if it did.
//

std::int32_t find_nearby_ladder_colvect(Thing *p_thing);
std::int32_t find_nearby_ladder_colvect_radius(std::int32_t mid_x, std::int32_t mid_z, std::int32_t radius);
std::int32_t mount_ladder(Thing *p_thing, std::int32_t col);

//
// Returns which side of the colvect you are on.
//

std::int32_t which_side(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    std::int32_t a, std::int32_t b);

//
// Returns the height of the floor at (x,z).
//

std::int32_t calc_height_at(std::int32_t x, std::int32_t z);

//
// Returns the height of the map at (x,z)... i.e. if (x,z) is inside a building
// then it returns the MAV_height at the top of the building, otherwise
// the call is equivalent to calc_height_at()
//

std::int32_t calc_map_height_at(std::int32_t x, std::int32_t z);

//
// Given a movement vector, this function changes it to slide
// along any col-vects it may encounter. If it find a colvect
// onto a walkable face, then it doesn't slide, it just returns
// that col-vect.
//

extern std::int32_t last_slide_colvect; // The last colvect you slid along, or nullptr if you didn't slide.

#define SLIDE_ALONG_DEFAULT_EXTRA_WALL_HEIGHT (-0x50)

#define SLIDE_ALONG_FLAG_CRAWL (1 << 0)
#define SLIDE_ALONG_FLAG_CARRYING (1 << 1)
#define SLIDE_ALONG_FLAG_JUMPING (1 << 2)

std::int32_t slide_along(
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t *x2, std::int32_t *y2, std::int32_t *z2,
    std::int32_t extra_wall_height, // Makes walls seem higher/lower so that a falling person won't fall through walls.
    std::int32_t radius, std::uint32_t flags = 0);

//
// Slides the movement vector around things- If the person hits a
// moving vehicle- he'll die. Returns true if a collision occurs.
//

std::int32_t collide_against_things(
    Thing *p_thing,
    std::int32_t radius,
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t *x2, std::int32_t *y2, std::int32_t *z2);

//
// Slides the given movement vector around OB_jects.
// Returns true if a collision occurs.
//

std::int32_t collide_against_objects(
    Thing *p_thing,
    std::int32_t radius,
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t *x2, std::int32_t *y2, std::int32_t *z2);

//
// Returns true if there is a los from 1 to 2.
// If there isn't a los, then it fills in los_failure_[x|y|z] with
// the last coordinate from point 1 to 2 that can be seen from point 1.
//

extern std::int32_t los_failure_x;
extern std::int32_t los_failure_y;
extern std::int32_t los_failure_z;
extern std::int32_t los_failure_dfacet; // The dfacet that caused the LOS to fail.

#define LOS_FLAG_IGNORE_SEETHROUGH_FENCE_FLAG (1 << 0)
#define LOS_FLAG_IGNORE_PRIMS (1 << 1)
#define LOS_FLAG_IGNORE_UNDERGROUND_CHECK (1 << 2)
#define LOS_FLAG_INCLUDE_CARS (1 << 3) // Only relevant if you don't ignore prims.

std::int32_t there_is_a_los_things(Thing *p_person_a, Thing *p_person_b, std::int32_t los_flags);

std::int32_t there_is_a_los(
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t x2, std::int32_t y2, std::int32_t z2,
    std::int32_t los_flag);

//
// An FOV calculation. Returns true if I can see him, given
// that I am looking in the given direction.
//

std::int32_t in_my_fov(
    std::int32_t me_x, std::int32_t me_z,
    std::int32_t him_x, std::int32_t him_z,
    std::int32_t im_looking_x,
    std::int32_t im_looking_z);

//
// Returns the nearest person of the given type that I can see.
// 'see' means in FOV and LOS.
//

THING_INDEX find_nearby_person(
    THING_INDEX me,
    std::uint16_t person_type_bits, // A bit for each person type (1 << person_type)
    std::int32_t max_range);        // 256 => One map block.

//
// It doesn't check that (x,z) is a point on the face or not!
//

// std::int32_t calc_height_on_face(std::int32_t x,std::int32_t z,std::int32_t face);

//
// Looks for any colvect that intersects the given line segment.
// If it find a colvect it returns it, otherwise it returns nullptr.
// If there is more than one colvect intersected, it prefers to
// return ladders and fences.
//

std::int32_t find_intersected_colvect(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    std::int32_t y);

//
// An collision routine for othogonal walls only.  It collides a thin vector
// against a thick orthogonal sausage shape.
//

std::int32_t collide_against_sausage(
    std::int32_t sx1, std::int32_t sz1,
    std::int32_t sx2, std::int32_t sz2,
    std::int32_t swidth,

    std::int32_t vx1, std::int32_t vz1,
    std::int32_t vx2, std::int32_t vz2,

    std::int32_t *slide_x,
    std::int32_t *slide_z);

//
// Useful mathsy stuff...
//
// A 'line' below actually means 'line _segment_'
//

void signed_dist_to_line_with_normal(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    std::int32_t a, std::int32_t b,
    std::int32_t *dist,
    std::int32_t *vec_x,
    std::int32_t *vec_z,
    std::int32_t *on);

void nearest_point_on_line(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    std::int32_t a, std::int32_t b,
    std::int32_t *ret_x,
    std::int32_t *ret_z);

std::int32_t nearest_point_on_line_and_dist(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    std::int32_t a, std::int32_t b,
    std::int32_t *ret_x,
    std::int32_t *ret_z);

std::int32_t nearest_point_on_line_and_dist_and_along(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    std::int32_t a, std::int32_t b,
    std::int32_t *ret_x,
    std::int32_t *ret_z,
    std::int32_t *along);

std::int32_t distance_to_line(
    std::int32_t x1, std::int32_t z1,
    std::int32_t x2, std::int32_t z2,
    std::int32_t a, std::int32_t b);

//
// Inserts additional STOREY_TYPE_JUST_COLLISION facets around the map
// to stop Darci going where she aughtn't.
//

void insert_collision_facets();

//
// Creates a shockwave that damages people and knocks them down.
//

void create_shockwave(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t radius,
    std::int32_t maxdamage,                             // Remember- a person's maximum health is 200
    Thing *p_aggressor, std::uint32_t just_people = 0); // Who caused the shockwave or nullptr if you don't know

//
// Flags fences with a see-through texture as being FACET_FLAG_SEETHROUGH
//

void COLLIDE_find_seethrough_fences();

// ========================================================
//
// FASTNAV CALCULATION
//
// ========================================================

//
// Fast slide-along collision detection. These functions tell you id a
// person movement vector does not need full collision detection if it
// is wholly contained within the given square.
//

//
// The fastnav bits array.
//

typedef std::uint8_t COLLIDE_Fastnavrow[PAP_SIZE_HI >> 3];

extern COLLIDE_Fastnavrow *COLLIDE_fastnav;

//
// Calculates the fastnav bits array.
//

void COLLIDE_calc_fastnav_bits();

//
// Returns true if you can fastnav in the given square.
//

#ifndef NDEBUG
std::int32_t COLLIDE_can_i_fastnav(std::int32_t x, std::int32_t z);
#else
#define COLLIDE_can_i_fastnav(x, z) (!(WITHIN((x), 0, PAP_SIZE_HI - 1) || !WITHIN((z), 0, PAP_SIZE_HI - 1) || (COLLIDE_fastnav[x][(z) >> 3] & (1 << ((z) & 0x7)))))
#endif // NDEBUG

//
// Draws a debug cross over each fastnav square near the given place
//

void COLLIDE_debug_fastnav(
    std::int32_t world_x, // 8-bits per mapsquare.
    std::int32_t world_z);

#endif
