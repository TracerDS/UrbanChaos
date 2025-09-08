//
// Dog leads. One end is attached somewhere, the other end it attached to a Entity and
// restricts its movements.
//

#ifndef _LEAD_
#define _LEAD_

//
// The lead structure. The first point of the lead is (attach_x,attach_y,attach_z).
//

typedef struct
{
    std::uint8_t p_num;     // Number of points.
    std::uint8_t p_index;   // Index into the LEAD_point array.
    std::uint16_t attach_x; // Where the lead is tied to
    std::uint16_t attach_y;
    std::uint16_t attach_z;
    std::uint16_t attach_thing; // The thing on the other end of the leash.

} LEAD_Lead;

#define LEAD_MAX_LEADS 32

extern LEAD_Lead LEAD_lead[LEAD_MAX_LEADS];
extern std::int32_t LEAD_lead_upto;

//
// The points.
//

typedef struct
{
    std::int32_t x;
    std::int32_t y;
    std::int32_t z;
    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

} LEAD_Point;

#define LEAD_MAX_POINTS 256

extern LEAD_Point LEAD_point[LEAD_MAX_POINTS];
extern std::int32_t LEAD_point_upto;

//
// Removes all the leads.
//

void LEAD_init();

//
// Creates a lead of the given length.
//

#define LEAD_LEN_SHORT 1
#define LEAD_LEN_MEDIUM 2
#define LEAD_LEN_LONG 3

void LEAD_create(
    std::int32_t len,
    std::int32_t world_x,
    std::int32_t world_y,
    std::int32_t world_z);

//
// Attaches all the leads to nearby lamposts and dogs.
//

void LEAD_attach();

//
// Processes all the leads.
//

void LEAD_process();

#endif
