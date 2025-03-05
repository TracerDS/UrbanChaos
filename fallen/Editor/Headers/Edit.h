#ifndef EDIT_H
#define EDIT_H 1

#include "primativ.hpp"
#include "Anim.h"
// #include	"Engine.h"
// DEFINES

#define MAX_D_LIGHTS 10

#define CUBE_INDEX_FRONT (0)
#define CUBE_INDEX_TOP (1)
#define CUBE_INDEX_BOTTOM (2)
#define CUBE_INDEX_LEFT (3)
#define CUBE_INDEX_RIGHT (4)
#define CUBE_INDEX_BACK (5)

#define CUBE_FLAG_FRONT (1 << 0)
#define CUBE_FLAG_TOP (1 << 1)
#define CUBE_FLAG_BOTTOM (1 << 2)
#define CUBE_FLAG_LEFT (1 << 3)
#define CUBE_FLAG_RIGHT (1 << 4)
#define CUBE_FLAG_BACK (1 << 5)
#define CUBE_FLAG_ALL (0x3f)
#define CUBE_FLAG_MULTI (1 << 6)

#define CUBE_TYPE_FULL (1)
#define CUBE_TYPE_SLOPE_LR (2)
#define CUBE_TYPE_STEPS_LR (3)
#define CUBE_TYPE_LEDGE1 (4)
#define CUBE_TYPE_END (5)

#define TEXTURE_DEFAULT_ROCKS (3 + (6 << 3) + (1 << 6))
#define TEXTURE_DEFAULT_BEIGEWALL (1 + (6 << 3) + (1 << 6))
#define TEXTURE_DEFAULT_BEIGESLATS (1 + (6 << 3) + (1 << 6))

#define MAX_EDIT_FACE_LIST 3000

/*********************************************
// STRUCTS
**********************************************/
struct EditInfo {
    EdRect SelectRect;
    std::int16_t SelectFlag;
    char MapName[30];
    std::int32_t amb_dx, amb_dy, amb_dz, amb_bright, amb_offset, amb_flags;
    std::uint16_t GridOn;
    std::int32_t TileFlag, TileScale;
    std::int32_t DX, DY, DZ;
    std::int32_t OX, OY, OZ;
    std::uint16_t FlatShade;
    std::uint16_t NoHidden;
    std::int32_t MinX, MinZ, MaxX, MaxZ;
    std::uint16_t Clipped;
    std::uint16_t RoofTex;
    std::uint16_t Inside;
    std::uint16_t InsideHeight;
    std::uint16_t MapID;
    std::uint16_t HideMap;
};

struct Light {
    std::int16_t Intensity;
    std::int16_t R;

    std::int16_t G;
    std::int16_t B;

    std::int32_t X;
    std::int32_t Y;
    std::int32_t Z;
};

struct OldDepthStrip {
    std::uint16_t MapThingIndex;
    std::uint16_t Depth[4];
    std::uint16_t ColVectHead;
    std::uint16_t Dummy1;
    std::uint16_t Dummy2;
};

//
// These are mirrored in MAP.H in the FALLEN project.
//

// #define	FLOOR_FLIP_FLAG		(1<<0)
// #define	FLOOR_BRIGHT1		(1<<1)
// #define	FLOOR_BRIGHT2		(1<<2)
// #define	FLOOR_HIDDEN		(1<<3)
// #define FLOOR_SINK_SQUARE	(1<<4)	// Lowers the floorsquare to create a curb.
// #define FLOOR_SINK_POINT	(1<<5)	// Transform the point on the lower level.

#define FLOOR_HEIGHT_SHIFT (3)

struct CubeBits {
    std::uint8_t Prim : 3;
    std::uint8_t Rot : 3;
    std::uint8_t Spare : 2;
};

struct TextureBits {
    std::uint32_t X : 5;
    std::uint32_t Y : 5;
    std::uint32_t Page : 4;
    std::uint32_t Rot : 2;
    std::uint32_t Flip : 2;
    std::uint32_t DrawFlags : 8;
    std::uint32_t Width : 3;
    std::uint32_t Height : 3;
};
/*
struct	MiniTextureBits
{
        std::uint16_t 	X:3;
        std::uint16_t 	Y:3;
        std::uint16_t	Page:4;
        std::uint16_t	Rot:2;
        std::uint16_t	Flip:2;
        std::uint16_t	Size:2;
};
*/
struct EditMapElement {
    struct CubeBits CubeType;
    std::uint8_t CubeFlags;
    struct TextureBits Textures[5]; // can never see 6th face
};

struct EditFace {
    std::int32_t MapX;
    std::int32_t MapY;
    std::int32_t MapZ;
    std::int32_t Z;
    std::int16_t Face;
    std::uint16_t Flag;
    struct EditMapElement *PEle;
    std::int32_t EditTurn;
    struct BucketHead *Bucket;
};

/*********************************************
// DATA
**********************************************/
extern struct SVector selected_prim_xyz;
extern std::uint8_t texture_sizes[];

extern struct Light d_lights[];
extern std::uint16_t next_d_light;

extern std::uint32_t editor_turn;
// extern EditFace		edit_face;
extern EditFace hilited_face,
    selected_face;

// extern struct	EditMapElement	edit_map_eles[65000];

extern std::int16_t face_selected_list[MAX_EDIT_FACE_LIST];
extern std::int16_t next_face_selected;

extern struct EditInfo edit_info;

/*********************************************
// FUNCTIONS
**********************************************/
extern std::uint16_t is_it_clockwise_xy(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, std::int32_t x3, std::int32_t y3);

extern std::int32_t add_a_light(std::int16_t i, std::int32_t x, std::int32_t y, std::int32_t z);
extern void clear_lights();

extern void calc_things_screen_box(std::int32_t map_thing, EdRect *rect);
extern std::int32_t hilight_map_backgrounds(std::uint16_t type);
extern std::int32_t hilight_map_things(std::uint16_t type);
extern std::int32_t select_map_things(MFPoint *mouse, std::uint16_t type);
extern std::int32_t select_map_backgrounds(MFPoint *mouse, std::uint16_t type);

extern void init_editor();
extern void draw_editor_map(std::uint32_t);
extern std::uint8_t check_big_point_triangle(std::int32_t x, std::int32_t y, std::int32_t ux, std::int32_t uy, std::int32_t vx, std::int32_t vy, std::int32_t wx, std::int32_t wy);
extern bool check_mouse_over_prim_quad(struct SVector *res, std::int32_t p1, std::int32_t p2, std::int32_t p3, std::int32_t p4, std::int32_t face);
extern bool check_mouse_over_prim_tri(struct SVector *res, std::int32_t p1, std::int32_t p2, std::int32_t p3, std::int32_t face);
extern void set_quad_buckets_texture(struct BucketQuad *p_bucket, struct TextureBits *texture);
extern std::uint32_t editor_user_interface(std::uint8_t type);
extern std::int32_t place_prim_at(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z);
extern std::int32_t load_map(char *name);
extern void save_map(char *name, std::int32_t quick);
extern std::uint32_t engine_keys_scroll();
extern std::uint32_t engine_keys_zoom();
extern std::uint32_t engine_keys_spin();
extern void set_things_faces(std::int16_t thing);
extern void add_face_to_list(std::int16_t face);
extern std::int32_t face_is_in_list(std::int16_t face);
extern void gamut_fiddle();
extern void clear_map();
extern void draw_3d_line(std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t col);
extern void set_screen_box(std::int32_t x, std::int32_t y, std::int32_t z, EdRect *rect, std::int32_t w, std::int32_t h);
extern void build_radius_info();
extern void draw_quick_map();
extern std::uint32_t engine_keys_scroll_game();
extern std::int32_t add_floor_face_to_bucket(std::int32_t x1, std::int32_t y1, std::int32_t z1, std::int32_t x2, std::int32_t y2, std::int32_t z2, std::int32_t x3, std::int32_t y3, std::int32_t z3, std::int32_t x4, std::int32_t y4, std::int32_t z4, struct DepthStrip *p_map, std::int32_t b1, std::int32_t b2, std::int32_t b3, std::int32_t b4, std::uint16_t tex);

#endif
