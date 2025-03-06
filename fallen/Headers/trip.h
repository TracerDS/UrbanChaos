//
// Trip wires.
//

#ifndef _TRIP_
#define _TRIP_

//
// When x1 == this magic number means the trip wire is deactivated.
//

#define TRIP_X1_DEACTIVATED 0xffff

//
// The trip-wires.
//

typedef struct
{
    std::uint8_t along;
    std::uint8_t wait;
    std::uint16_t counter;
    std::int16_t y;
    std::uint16_t x1; // If this equals TRIP_X1_DEACTIVATED then the tripwire isn't drawn or processed
    std::uint16_t z1;
    std::uint16_t x2;
    std::uint16_t z2;

} TRIP_Wire;

#define TRIP_MAX_WIRES 32

extern TRIP_Wire *TRIP_wire; //[TRIP_MAX_WIRES];
extern std::int32_t TRIP_wire_upto;

//
// Gets rid of the tripwire.
//

void TRIP_init();

//
// Creates a new length of tripwire. Returns the index of the
// trip-wire or nullptr if it cant create another tripwire.
//
// If an identical tripwire already exists, it just returns
// the index of that tripwire.
//

std::uint8_t TRIP_create(
    std::int32_t y,
    std::int32_t x1,
    std::int32_t z1,
    std::int32_t x2,
    std::int32_t z2);

//
// Processes the trip-wires.
//

void TRIP_process();

//
// Returns true if the given tripwire is activated.
//

std::int32_t TRIP_activated(std::uint8_t tripwire);

//
// Deactivates the given tripwire: stops it being drawn and processed.
//

void TRIP_deactivate(std::uint8_t tripwire);

//
// Drawing the trip-wires.
//

typedef struct
{
    std::int32_t y;
    std::int32_t x1;
    std::int32_t z1;
    std::int32_t x2;
    std::int32_t z2;
    std::uint16_t counter;
    std::uint8_t along; // How far along the beam is stopped due to someone being in the way.
    std::uint8_t padding;

} TRIP_Info;

void TRIP_get_start();
TRIP_Info *TRIP_get_next(); // Returns nullptr if there are no more trips left.

#endif
