//
// The new bikes are called dikes.
//

#ifndef TARGET_DC

#ifndef _DIKE_
#define _DIKE_

typedef struct
{
    //
    // The front wheel
    //

    std::int32_t fx;
    std::int32_t fy;
    std::int32_t fz;
    std::int32_t fdx;
    std::int32_t fdy;
    std::int32_t fdz;

    //
    // The back wheel
    //

    std::int32_t bx;
    std::int32_t by;
    std::int32_t bz;
    std::int32_t bdx;
    std::int32_t bdy;
    std::int32_t bdz;

    //
    // Front suspension.
    //

    std::int32_t fsy;
    std::int32_t fsdy;

    //
    // Back suspension.
    //

    std::int32_t bsy;
    std::int32_t bsdy;

    //
    // Bike control
    //

#define DIKE_CONTROL_ACCEL (1 << 0)
#define DIKE_CONTROL_LEFT (1 << 1)
#define DIKE_CONTROL_RIGHT (1 << 2)
#define DIKE_CONTROL_BRAKE (1 << 3)
#define DIKE_CONTORL_WHEELIE (1 << 4)

    std::uint8_t control;
    std::int8_t steer;
    std::int8_t power;

    //
    // Flags
    //

#define DIKE_FLAG_ON_GROUND_FRONT (1 << 0)
#define DIKE_FLAG_ON_GROUND_BACK (1 << 1)

    std::uint8_t flag;

    //
    // The angle of the bike.
    //

    std::uint16_t yaw;
    std::uint16_t pitch;

} DIKE_Dike;

#define DIKE_MAX_DIKES 8

extern DIKE_Dike DIKE_dike[DIKE_MAX_DIKES];
extern std::int32_t DIKE_dike_upto;

//
// Initialises the dikes.
//

void DIKE_init();

//
// Creates a new dike.
//

DIKE_Dike *DIKE_create(
    std::int32_t x,
    std::int32_t z,
    std::int32_t yaw);

//
// Processes a dike. Write directly into the 'control' field to
// move the dike.
//

void DIKE_process(DIKE_Dike *dd);

//
// A line-drawn representation of the dike.
//

void DIKE_draw(DIKE_Dike *dd);

#endif

#endif // #ifndef TARGET_DC
