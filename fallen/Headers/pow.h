//
// Yet another explosion system.
//

#ifndef _POW_
#define _POW_

//
// A single element of an explosion.
//

typedef struct
{
    std::uint8_t next;
    std::uint8_t frame;       // Which frame of animation it is on...
    std::uint8_t frame_speed; // How fast it moves between frames.
    std::uint8_t damp;        // How damped the motion is.
    std::int32_t x;
    std::int32_t y;
    std::int32_t z;
    std::int16_t dx;
    std::int16_t dy;
    std::int16_t dz;
    std::uint16_t frame_counter; // Countdown to going onto next frame.

} POW_Sprite;

#ifdef TARGET_DC

#define POW_MAX_SPRITES 128

#else

#ifndef PSX
#define POW_MAX_SPRITES 256
#else
#define POW_MAX_SPRITES 192
#endif
#endif

extern POW_Sprite POW_sprite[POW_MAX_SPRITES];
extern std::uint8_t POW_sprite_free;

//
// An explosion.
//

typedef struct
{
    std::uint8_t type;
    std::uint8_t next;
    std::uint8_t sprite; // Index into the linked list of sprites for this POW
    std::uint8_t mapwho;
    std::int32_t x;
    std::int32_t y;
    std::int32_t z;
    std::int16_t dx;
    std::int16_t dy;
    std::int16_t dz;
    std::uint16_t timer;
    std::uint8_t flag;
    std::uint8_t time_warp;
    std::uint16_t padding;

} POW_Pow;

#ifdef TARGET_DC

#define POW_MAX_POWS 16

#else

#ifndef PSX
#define POW_MAX_POWS 32
#else
#define POW_MAX_POWS 24
#endif

#endif

extern POW_Pow POW_pow[POW_MAX_POWS];
extern std::uint8_t POW_pow_free;
extern std::uint8_t POW_pow_used;

//
// The POW 1D mapwho- thats only 32 bytes!
//

extern std::uint8_t POW_mapwho[PAP_SIZE_LO];

//
// Initialises the explosions.
//

void POW_init();

//
// Sets off an explosion.
//
#define POW_TYPE_UNUSED 0
#define POW_TYPE_BASIC_SPHERE_LARGE 1
#define POW_TYPE_BASIC_SPHERE_MEDIUM 2
#define POW_TYPE_BASIC_SPHERE_SMALL 3
#define POW_TYPE_SPAWN_SPHERE_LARGE 4
#define POW_TYPE_SPAWN_SPHERE_MEDIUM 5
#define POW_TYPE_MULTISPAWN 6
#define POW_TYPE_MULTISPAWN_SEMI 7
#define POW_TYPE_NUMBER 8

#define POW_CREATE_LARGE_SEMI 0
#define POW_CREATE_MEDIUM 1
#define POW_CREATE_LARGE 2

//
// Creates a high-level explosion.
//

void POW_new(std::int32_t type, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t dx, std::int32_t dy, std::int32_t dz);

inline void POW_create(
    std::int32_t which,
    std::int32_t x, // 16-bits per mapsquare
    std::int32_t y,
    std::int32_t z,
    std::int32_t dx, // 16-bits per mapsquare
    std::int32_t dy,
    std::int32_t dz) {
    switch (which) {
        case POW_CREATE_LARGE_SEMI: POW_new(POW_TYPE_MULTISPAWN_SEMI, x, y, z, dx, dy, dz); break;
        case POW_CREATE_MEDIUM: POW_new(POW_TYPE_SPAWN_SPHERE_MEDIUM, x, y, z, dx, dy, dz); break;
        case POW_CREATE_LARGE: POW_new(POW_TYPE_MULTISPAWN, x, y, z, dx, dy, dz); break;

        default:
            ASSERT(0);
            break;
    }
}
/*
void POW_create(
                std::int32_t which,
                std::int32_t x,		// 16-bits per mapsquare
                std::int32_t y,
                std::int32_t z,
                std::int32_t dx = 0,	// 16-bits per mapsquare
                std::int32_t dy = 0,
                std::int32_t dz = 0);
*/

//
// Processes the explosions.
//

void POW_process();

#endif
