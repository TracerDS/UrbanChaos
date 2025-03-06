#pragma once

#include <cstdint>

// DEFINES

// Set this to 1 to enable TomF's new D3D-friendly engine.
// 0 enables the old engine again.
// NOTE! There are other versions of this define dotted around in other header
// files! Make sure they all agree or you'll have grief.
#ifdef TARGET_DC
#define USE_TOMS_ENGINE_PLEASE_BOB 1
#else
#define USE_TOMS_ENGINE_PLEASE_BOB 1
#endif

//
// face savetype for primpoint size change
//
#define PRIM_START_SAVE_TYPE 5793
#define PRIM_END_SAVE_TYPE 5800

//
// roof face flags
//

#define RFACE_FLAG_SHADOW_1 (1 << 0) // Form a number from 0-7
#define RFACE_FLAG_SHADOW_2 (1 << 1)
#define RFACE_FLAG_SHADOW_3 (1 << 2)
#define RFACE_FLAG_SLIDE_EDGE (1 << 3)
#define RFACE_FLAG_SLIDE_EDGE_0 (1 << 3)
#define RFACE_FLAG_SLIDE_EDGE_1 (1 << 4)
#define RFACE_FLAG_SLIDE_EDGE_2 (1 << 5)
#define RFACE_FLAG_SLIDE_EDGE_3 (1 << 6)

#define RFACE_FLAG_SPLIT (1 << 7)  // Seeing as we aren't using this...
#define RFACE_FLAG_NODRAW (1 << 7) // ...I'll sneak in this flag! Its temporary.

#define GAME_SCALE (256.0f)     //(1660.0+200)   //3660
#define GAME_SCALE_DIV (100.0f) // 170.0 //100.0

#define FACE_FLAG_WMOVE (1 << 0) // A moving walkable face- ThingIndex in an index into the WMOVE_face array.
#define FACE_FLAG_SMOOTH (1 << 0)
#define FACE_FLAG_OUTLINE (1 << 1)
#define FACE_FLAG_SHADOW_1 (1 << 2) // Form a number from 0-7
#define FACE_FLAG_SHADOW_2 (1 << 3)
#define FACE_FLAG_SHADOW_3 (1 << 4)
#define FACE_FLAG_ROOF (1 << 5)
#define FACE_FLAG_WALKABLE (1 << 6)
#define FACE_FLAG_ENVMAP (1 << 7)
#define FACE_FLAG_VERTICAL (1 << 7)

#define FACE_FLAG_TINT (1 << 8)
#define FACE_FLAG_ANIMATE (1 << 9)
#define FACE_FLAG_OTHER_SPLIT (1 << 10)
#define FACE_FLAG_METAL (1 << 10)
#define FACE_FLAG_NON_PLANAR (1 << 11)
#define FACE_FLAG_TEX2 (1 << 12)

#define FACE_FLAG_FIRE_ESCAPE (1 << 13)
#define FACE_FLAG_PRIM (1 << 14)
#define FACE_FLAG_THUG_JACKET (1 << 15)

//
// The face has been fixed- the page changed by TEXTURE_fix_prim_textures()
//

#define FACE_FLAG_FIXED (1 << 5)

//
// An edge of a walkable quad that does not lead onto a
// connecting one.
//

#define FACE_FLAG_SLIDE_EDGE (1 << 1)
#define FACE_FLAG_SLIDE_EDGE_0 (1 << 1)
#define FACE_FLAG_SLIDE_EDGE_1 (1 << 2)
#define FACE_FLAG_SLIDE_EDGE_2 (1 << 3)
#define FACE_FLAG_SLIDE_EDGE_3 (1 << 4)

#define FACE_FLAG_SLIDE_EDGE_ALL (0xf << 1)
//
// Used only while loading in a SEX object...
//

#define FACE_FLAG_EDGE_VISIBLE (1 << 13)
#define FACE_FLAG_EDGE_VISIBLE_A (FACE_FLAG_EDGE_VISIBLE << 0)
#define FACE_FLAG_EDGE_VISIBLE_B (FACE_FLAG_EDGE_VISIBLE << 1)
#define FACE_FLAG_EDGE_VISIBLE_C (FACE_FLAG_EDGE_VISIBLE << 2)

// #define	FACE_TYPE_FIRE_ESCAPE	(1<<0)
#define FACE_TYPE_FENCE (1 << 1)
#define FACE_TYPE_CABLE (1 << 2)
#define FACE_TYPE_SKYLIGHT (1 << 3)
#define FACE_TYPE_PRIM (1 << 4)

#define GET_SORT_LEVEL(f) (((f) >> 2) & 7)
#define OR_SORT_LEVEL(u, f) u |= ((f) << 2)
#define SET_SORT_LEVEL(u, f) \
    u &= ~(7 << 2);          \
    u |= ((f) << 2)

//
// What each prim object is.
//

#define PRIM_OBJ_LAMPPOST 1
#define PRIM_OBJ_TRAFFIC_LIGHT 2
#define PRIM_OBJ_WALK_DONT_WALK 3
#define PRIM_OBJ_PETROL_PUMP 4
#define PRIM_OBJ_DOUBLE_PETROL_PUMP 5
#define PRIM_OBJ_GAS_STATION_SIGN 6
#define PRIM_OBJ_BILLBOARD 7
#define PRIM_OBJ_GAS_STATION_BBOARD 8
#define PRIM_OBJ_HOTEL_SIGN 9
#define PRIM_OBJ_GAS_SIGN 10
#define PRIM_OBJ_GAS_STATION_LIGHT 11
#define PRIM_OBJ_FIRE_ESCAPE1 12
#define PRIM_OBJ_FIRE_ESCAPE2 13
#define PRIM_OBJ_FIRE_ESCAPE3 14
#define PRIM_OBJ_FIRE_ESCAPE4 15
#define PRIM_OBJ_FIRE_ESCAPE5 16
#define PRIM_OBJ_FIRE_ESCAPE6 17
#define PRIM_OBJ_TYRE_SIGN 18
#define PRIM_OBJ_CANOPY1 19
#define PRIM_OBJ_CANOPY2 20
#define PRIM_OBJ_CANOPY3 21
#define PRIM_OBJ_CANOPY4 22
#define PRIM_OBJ_DINER_SIGN 23
#define PRIM_OBJ_MOTEL_SIGN1 25
#define PRIM_OBJ_MOTEL_SIGN2 26
#define PRIM_OBJ_WILDCATVAN_BODY 27
#define PRIM_OBJ_INTERIOR_STAIRS2 28
#define PRIM_OBJ_INTERIOR_STAIRS3 29

//
// Im giving up typing in the right names now....
//

#define PRIM_OBJ_RADIATOR 30
#define PRIM_OBJ_MOTORBIKE 31
#define PRIM_OBJ_BIN 33
#define PRIM_OBJ_ITEM_KEY 36
#define PRIM_OBJ_LION2 46
#define PRIM_OBJ_BIKE 47
#define PRIM_OBJ_VAN 48
#define PRIM_OBJ_BOAT 49
#define PRIM_OBJ_CAR 50
#define PRIM_OBJ_STROBE 51
#define PRIM_OBJ_DOOR 52
#define PRIM_OBJ_LAMP 54
#define PRIM_OBJ_SIGN 60
#define PRIM_OBJ_TRAFFIC_CONE 67
#define PRIM_OBJ_CHOPPER 74
#define PRIM_OBJ_CHOPPER_BLADES 75
#define PRIM_OBJ_CAN 76
#define PRIM_OBJ_CANOPY 81
#define PRIM_OBJ_ITEM_TREASURE 84
#define PRIM_OBJ_HOOK 86
#define PRIM_OBJ_VAN_WHEEL 87
#define PRIM_OBJ_VAN_BODY 88
#define PRIM_OBJ_PARK_BENCH 89
#define PRIM_OBJ_JEEP_BODY 90
#define PRIM_OBJ_MEATWAGON_BODY 91
#define PRIM_OBJ_ARMCHAIR 101
#define PRIM_OBJ_COFFEE_TABLE 102
#define PRIM_OBJ_SOFA 105
#define PRIM_OBJ_CAR_BODY 108
#define PRIM_OBJ_WOODEN_TABLE 110
#define PRIM_OBJ_ITEM_AK47 120
#define PRIM_OBJ_ITEM_MAGNUM 121
#define PRIM_OBJ_CHAIR 126
#define PRIM_OBJ_ITEM_SHOTGUN 127
#define PRIM_OBJ_SEDAN_BODY 131
#define PRIM_OBJ_BALLOON 132
#define PRIM_OBJ_BIKE_STEER 137 // The steering column of the bike.
#define PRIM_OBJ_BIKE_BWHEEL 138
#define PRIM_OBJ_BIKE_FWHEEL 139
#define PRIM_OBJ_BIKE_FRAME 140
#define PRIM_OBJ_BARREL 141
#define PRIM_OBJ_ITEM_HEALTH 142
#define PRIM_OBJ_ITEM_GUN 143
#define PRIM_OBJ_ITEM_KEYCARD 144
#define PRIM_OBJ_ITEM_BOMB 145
#define PRIM_OBJ_POLICE_BODY 150
#define PRIM_OBJ_TAXI_BODY 155
#define PRIM_OBJ_AMBULANCE_BODY 159
#define PRIM_OBJ_ITEM_KNIFE 166
#define PRIM_OBJ_ITEM_EXPLOSIVES 167
#define PRIM_OBJ_ITEM_GRENADE 168
#define PRIM_OBJ_TRIPWIRE 178
#define PRIM_OBJ_MINE 188
#define PRIM_OBJ_SPIKE 189
#define PRIM_OBJ_VALVE 196
#define PRIM_OBJ_THERMODROID 201
#define PRIM_OBJ_ITEM_BASEBALLBAT 205
#define PRIM_OBJ_CAR_WHEEL 220
#define PRIM_OBJ_POLICE_TARGET 147
#define PRIM_OBJ_SWITCH_OFF 216
#define PRIM_OBJ_SWITCH_ON 157
#define PRIM_OBJ_HYDRANT 228
#define PRIM_OBJ_ITEM_FILE 232
#define PRIM_OBJ_ITEM_FLOPPY_DISK 233
#define PRIM_OBJ_ITEM_CROWBAR 234
#define PRIM_OBJ_ITEM_GASMASK 235
#define PRIM_OBJ_ITEM_WRENCH 236
#define PRIM_OBJ_ITEM_VIDEO 237
#define PRIM_OBJ_ITEM_GLOVES 238
#define PRIM_OBJ_ITEM_WEEDKILLER 239
#define PRIM_OBJ_ITEM_AMMO_SHOTGUN 253
#define PRIM_OBJ_ITEM_AMMO_AK47 254
#define PRIM_OBJ_ITEM_AMMO_PISTOL 255
#define PRIM_OBJ_NUMBER 256
#define PRIM_OBJ_WEAPON_GUN 256
#define PRIM_OBJ_WEAPON_KNIFE 257
#define PRIM_OBJ_WEAPON_SHOTGUN 258
#define PRIM_OBJ_WEAPON_BAT 259
#define PRIM_OBJ_WEAPON_AK47 260
#define PRIM_OBJ_WEAPON_GUN_FLASH 261
#define PRIM_OBJ_WEAPON_SHOTGUN_FLASH 262
#define PRIM_OBJ_WEAPON_AK47_FLASH 263

//
// The "Fun Stuff" day (14th Jan 1999)
//

//
// Returns the collision model to use for each prim.
//

#define PRIM_COLLIDE_BOX 0      // As a rotated bounding box
#define PRIM_COLLIDE_NONE 1     // Just walk through the prim
#define PRIM_COLLIDE_CYLINDER 2 // As a cylinder
#define PRIM_COLLIDE_SMALLBOX 3 // A bounding box smaller than the prim

std::uint8_t prim_get_collision_model(std::int32_t prim);

//
// The type of shadow to draw under the prim.
//

#define PRIM_SHADOW_NONE 0
#define PRIM_SHADOW_BOXEDGE 1
#define PRIM_SHADOW_CYLINDER 2
#define PRIM_SHADOW_FOURLEGS 3
#define PRIM_SHADOW_FULLBOX 4

std::uint8_t prim_get_shadow_type(std::int32_t prim);

//
// Prim flags.
//

#define PRIM_FLAG_LAMPOST (1 << 0)
#define PRIM_FLAG_CONTAINS_WALKABLE_FACES (1 << 1)
#define PRIM_FLAG_GLARE (1 << 2)
#define PRIM_FLAG_ITEM (1 << 3)
#define PRIM_FLAG_TREE (1 << 4)
#define PRIM_FLAG_ENVMAPPED (1 << 5)
#define PRIM_FLAG_JUST_FLOOR (1 << 6)
#define PRIM_FLAG_ON_FLOOR (1 << 7)

#define PRIM_DAMAGE_DAMAGABLE (1 << 0)
#define PRIM_DAMAGE_EXPLODES (1 << 1)
#define PRIM_DAMAGE_CRUMPLE (1 << 2)
#define PRIM_DAMAGE_LEAN (1 << 3)
#define PRIM_DAMAGE_NOLOS (1 << 4) // You can't see through this prim (included in LOS calculation)

#define SMAT_SHIFT0 (2)
#define SMAT_SHIFT1 (12)
#define SMAT_SHIFT2 (22)
#define SMAT_SHIFTD (22)
#define CMAT0_MASK (0x3ff00000)
#define CMAT1_MASK (0x000ffc00)
#define CMAT2_MASK (0x000003ff)

// STRUCTS
struct CMatrix33 {
    std::int32_t M[3];
};

struct Matrix33 {
    std::int32_t M[3][3];
};

struct Matrix31 {
    std::int32_t M[3];
};

struct SMatrix31 {
    std::int16_t M[3];
};

struct OldPrimPoint {
    std::int32_t X, Y, Z;
};

struct PrimPoint {
    std::int16_t X, Y, Z;
};

#define ROOF_SHIFT 3
struct RoofFace4 {
    //	std::uint16_t	TexturePage; //could use the texture on the floor
    std::int16_t Y;
    std::int8_t DY[3];
    std::uint8_t DrawFlags;
    std::uint8_t RX;
    std::uint8_t RZ;
    std::int16_t Next; // link list of walkables off floor
};

#ifdef PSX
#define WALKABLE TexturePage

struct PrimFace4 {
    std::int16_t TexturePage;
    std::uint8_t AltPal;
    std::uint8_t DrawFlags;
    std::uint16_t Points[4];
    std::uint8_t UV[4][2];
    std::int16_t ThingIndex;
    std::uint16_t FaceFlags;
};

struct PrimFace3 {
    std::int16_t TexturePage;
    std::uint8_t AltPal;
    std::uint8_t DrawFlags;
    std::uint16_t Points[3];
    std::uint8_t UV[3][2];
    std::int16_t ThingIndex;
    std::uint16_t FaceFlags;
};

#else
#define WALKABLE Col2
struct PrimFace3 {
    std::uint8_t TexturePage;
    std::uint8_t DrawFlags;
    std::uint16_t Points[3];
    std::uint8_t UV[3][2];
    std::int16_t Bright[3]; // make into byte
    std::int16_t ThingIndex;
    std::uint16_t Col2;
    std::uint16_t FaceFlags;
    std::uint8_t Type; // move after bright
    std::int8_t ID;    // delete
};

struct PrimFace4 {
    std::uint8_t TexturePage;
    std::uint8_t DrawFlags;
    std::uint16_t Points[4];
    std::uint8_t UV[4][2];

    union {
        std::int16_t Bright[4]; // Used for people.

        struct // We cant use a LIGHT_Col because of circluar #include problems :-(
        {
            std::uint8_t red;
            std::uint8_t green;
            std::uint8_t blue;

        } col; // Used for building faces...
    };

    std::int16_t ThingIndex;
    std::int16_t Col2;
    std::uint16_t FaceFlags;
    std::uint8_t Type; // move after bright
    std::int8_t ID;
};
#endif

struct PrimFace4PSX {
    std::int16_t TexturePage;
    std::uint8_t AltPal;
    std::uint8_t DrawFlags;
    std::uint16_t Points[4];
    std::uint8_t UV[4][2];
    std::int16_t ThingIndex;
    std::uint16_t FaceFlags;
};

struct PrimFace3PSX {
    std::int16_t TexturePage;
    std::uint8_t AltPal;
    std::uint8_t DrawFlags;
    std::uint16_t Points[3];
    std::uint8_t UV[3][2];
    std::int16_t ThingIndex;
    std::uint16_t FaceFlags;
};

struct PrimObject {
    std::uint16_t StartPoint;
    std::uint16_t EndPoint;
    std::uint16_t StartFace4;
    std::uint16_t EndFace4;
    std::int16_t StartFace3;
    std::int16_t EndFace3;

    std::uint8_t coltype;
    std::uint8_t damage; // How this prim gets damaged
    std::uint8_t shadowtype;
    std::uint8_t flag;
};

#if USE_TOMS_ENGINE_PLEASE_BOB

// A flag that lives in the top bit of wTexturePage.
#define TEXTURE_PAGE_FLAG_JACKET (1 << 15)
#define TEXTURE_PAGE_FLAG_OFFSET (1 << 14)
#define TEXTURE_PAGE_FLAG_TINT (1 << 13)
#define TEXTURE_PAGE_FLAG_NOT_TEXTURED (1 << 12)
#define TEXTURE_PAGE_MASK (0x0fff)

// A prim's material.
// Note that the indices are listed by first the low-quality ones, then the high-quality ones.
// Also, the material uses the number of vertices specified by wNumHiVertices, of which the
// first wNumVertices are used. There are no vertices used by the low-quality mesh that are
// not also used by the high-quality one.
struct PrimObjectMaterial {
    std::uint16_t wTexturePage;     // The texture page, maybe with some flags in the top few bits.
    std::uint16_t wNumListIndices;  // How many list indices there are.
    std::uint16_t wNumStripIndices; // How many interrupted strip indices there are.
    std::uint16_t wNumVertices;     // Number of vertices used.
    // For the low-quality models.
    std::uint16_t wNumLoListIndices;  // How many list indices there are.
    std::uint16_t wNumLoStripIndices; // How many interrupted strip indices there are.
    std::uint16_t wNumLoVertices;     // Number of vertices used.
};

// My version of an object - an addition to the one above.
// (can't change the size of PrimObject or the loader gets kersplat.

// Object has no alpha polys in it - don't bother checking.
// #define D3DOBJECT_FLAG_NOALPHA				(1<<0)

struct TomsPrimObject {
    std::uint16_t wFlags;          // D3DOBJECT_FLAG_*** flags.
    std::uint16_t wNumMaterials;   // Number of materials.
    std::uint16_t wTotalSizeOfObj; // Number of vertices used by object.
    std::uint8_t bLRUQueueNumber;  // Position in the LRU queue.
    std::uint8_t bPadding;
    PrimObjectMaterial *pMaterials; // Pointer to the materials. Can MemFree this.
    void *pD3DVertices;             // Pointer to the D3DVERTEX list. DONT MEMFREE THIS
    std::uint16_t *pwListIndices;   // Pointer to the indices in list form. Can MemFree this.
    std::uint16_t *pwStripIndices;  // Pointer to the indices in interrupted strip form. DONT MEMFREE THIS
    float fBoundingSphereRadius;    // Guess!
};
#endif

struct PrimObjectOld {
    char ObjectName[32];
    std::uint16_t StartPoint;
    std::uint16_t EndPoint;
    std::uint16_t StartFace4;
    std::uint16_t EndFace4;
    std::int16_t StartFace3;
    std::int16_t EndFace3;

    std::uint8_t coltype;
    std::uint8_t damage; // How this prim gets damaged
    std::uint8_t shadowtype;
    std::uint8_t flag;

    std::uint16_t Dummy[4];
};

struct PrimMultiObject {
    std::uint16_t StartObject;
    std::uint16_t EndObject;
    std::int16_t Tween;
    std::int16_t Frame;
};

// data
extern char prim_names[2000][32];
extern struct SVector global_res[]; // max points per object?
extern std::int32_t global_flags[];
extern std::uint16_t global_bright[];

extern std::uint16_t background_prim;

// FUNCTIONS

// extern void	draw_a_rot_prim_at(std::uint16_t	prim,std::int32_t x,std::int32_t y,std::int32_t z,std::int32_t tween,struct	PrimMultiAnim *anim_info);
extern void test_poly();

extern std::int32_t load_all_prims(char *name);
extern std::int32_t save_all_prims(char *name);

extern std::int32_t copy_prim_to_end(std::uint16_t prim, std::uint16_t direct, std::int16_t thing);
extern void delete_a_prim(std::uint16_t prim);
extern void delete_last_prim();
extern void calc_normal(std::int16_t face, struct SVector *p_normal);
extern std::uint16_t apply_ambient_light_to_object(std::uint16_t object, std::int32_t lnx, std::int32_t lny, std::int32_t lnz, std::uint16_t intense);

//
// Initialises all the prim data.
//

void clear_prims();

//
// Calculates the FACE_FLAG_SLIDE_EDGE flags in the walkable faces.
//

void calc_slide_edges();

//
// Calculates the normals for each prim. These aren't saved
// or loaded in. The length of each normal is 256.
//

void calc_prim_normals();

//
// Calculates the PRIM_Infos for all the prims and sets the
// PRIM_FLAG_ENVMAP_OR_TINTED flag for prims that contain environment
// mapped or tinted faces.
//

void calc_prim_info();

//
// Puts all the array indices back to what they were at
// some point.
//

void record_prim_status();
void revert_to_prim_status();

//
// Returns the info for each prim.
//

typedef struct
{
    std::int32_t minx; // The bounding rectangle of the prim.
    std::int32_t miny;
    std::int32_t minz;

    std::int32_t maxx;
    std::int32_t maxy;
    std::int32_t maxz;

    std::int32_t cogx; // The centre of gravity of the prim.
    std::int32_t cogy;
    std::int32_t cogz;

    std::int32_t radius; // The bounding sphere about the origin.

} PrimInfo;

PrimInfo *get_prim_info(std::int32_t prim);

//
// ...
//

void compress_prims();

//
// Returns the position of the given point of the prim.
// If the point is -1 then a random point is returned.
//

void get_rotated_point_world_pos(
    std::int32_t point, // -1 => A random point.
    std::int32_t prim,
    std::int32_t prim_x,
    std::int32_t prim_y,
    std::int32_t prim_z,
    std::int32_t prim_yaw,
    std::int32_t prim_pitch,
    std::int32_t prim_roll,
    std::int32_t *px,
    std::int32_t *py,
    std::int32_t *pz);

//
// Collides the a movement vector with the bounding-box of the given prim.
// Returns true if a collision occured.
//

std::int32_t slide_along_prim(
    std::int32_t prim,
    std::int32_t prim_x,
    std::int32_t prim_y,
    std::int32_t prim_z,
    std::int32_t prim_yaw,
    std::int32_t x1, std::int32_t y1, std::int32_t z1,
    std::int32_t *x2, std::int32_t *y2, std::int32_t *z2,
    std::int32_t radius,
    std::int32_t shrink,      // Makes the bounding box of the prim much shorter and smaller.
    std::int32_t dont_slide); // true => Don't move if the vector collides with the prim.

//
// Sets the animation used by the given anim_prim. Anims start
// from 1... anim 0 is not used.
//

void set_anim_prim_anim(std::int32_t anim_prim_thing_index, std::int32_t anim);

//
// Returns the type of the given anim prim.
//

#define ANIM_PRIM_TYPE_NORMAL 0
#define ANIM_PRIM_TYPE_DOOR 1
#define ANIM_PRIM_TYPE_SWITCH 2

std::int32_t get_anim_prim_type(std::int32_t anim_prim);

//
// Returns the THING_INDEX of the nearest anim prim to the given point of
// one of the given types within the maximum range.
//

std::int32_t find_anim_prim(
    std::int32_t x,
    std::int32_t y,
    std::int32_t z,
    std::int32_t range,
    std::uint32_t type_bit_field); // i.e (1 << ANIM_PRIM_TYPE_DOOR) | (1 << ANIM_PRIM_TYPE_NORMAL)

//
// Toggles the state of the given switch prim.
//

void toggle_anim_prim_switch_state(std::int32_t anim_prim_thing_index);

//
// Find the bounding box of each anim-prim. The positions and rotations
// are taken from the initial position of the first frame.
//

void find_anim_prim_bboxes();

//
// Does nothing to the prim_points and prim_faces, but marks
// each of the prim_objects as not loaded.  You won't be able
// to draw them, but next time you call load_prim_object()
// it will load in the prim.
//

void mark_prim_objects_as_unloaded();

//
// Goes through all the prim faces and makes sure that none of them have
// their WMOVE flag set.
//

void clear_all_wmove_flags();

//
// Returns true if a face lies along this line. Coordinates are in world coordinates (8-bits per mapsquare)
// The line must lie completely within the fence.
//

std::int32_t does_fence_lie_along_line(std::int32_t x1, std::int32_t z1, std::int32_t x2, std::int32_t z2);