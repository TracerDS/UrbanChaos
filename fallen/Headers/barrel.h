//
// Crappy barrels with kludged physics
//

#pragma once

//
// needed by memory.h
//
#define BARREL_MAX_SPHERES 	80 // The maximum number of moving barrels * 2
#define BARREL_MAX_BARRELS (300)

#define TO_BARREL(t)				(&BARREL_barrel[t])
#define	BARREL_NUMBER(t)			(COMMON_INDEX)(t-TO_BARREL(0))


//
// Spheres that are moving use two sphere structures...
//

typedef struct
{
	std::int32_t x;	// -INFINITY => Unused
	std::int32_t y;
	std::int32_t z;
	std::int32_t dx;
	std::int32_t dy;
	std::int32_t dz;
	std::int16_t still;	// The number of gameturns that this sphere has been below a threshold speed.
	std::uint16_t radius;	// In the large coordinate system- even thought it is a std::uint16_t!

} BARREL_Sphere;

struct Barrel
{
	std::uint8_t type;
	std::uint8_t flag;
	std::uint16_t on;		// For stacked barrels it is the barrel you are supported by (nullptr => on the ground)
	std::uint16_t bs;		// For moving barrels its an index to 2 BARREL_Spheres in the BARREL_sphere[] array.
};

//
// Lets keep thing.h happy.
//

//struct  Barrel;
typedef Barrel *BarrelPtr;


//
// Initialises the barrels.
//

void BARREL_init();

extern BARREL_Sphere *BARREL_sphere; //[BARREL_MAX_SPHERES];
extern Barrel *BARREL_barrel;//[BARREL_MAX_BARRELS];

extern std::int32_t         BARREL_sphere_last;	  // MARK!!! WTF, you usuall call thing BLAH_blah_upto
extern std::int32_t  BARREL_barrel_upto;


//
// Creates a new barrel. If you create a barrel over a previous one- they
// stack up on top of eachother.
//

#define BARREL_TYPE_NORMAL	0
#define BARREL_TYPE_CONE	1
#define BARREL_TYPE_BURNING	2
#define BARREL_TYPE_BIN     3

std::uint16_t BARREL_alloc(
		std::int32_t type,
		std::int32_t prim,
		std::int32_t x,
		std::int32_t z,
		std::int32_t waypoint);	// The waypoint that creates this barrel or nullptr

//
// Returns the position on a burning barrel for where the flame should start.
//

GameCoord BARREL_fire_pos(Thing *p_barrel);

//
// For a moving vehicle- for instance.
// 

void BARREL_hit_with_prim(
		std::int32_t prim,
		std::int32_t x,
		std::int32_t y,
		std::int32_t z,
		std::int32_t yaw);

void BARREL_hit_with_sphere(
		std::int32_t x,
		std::int32_t y,
		std::int32_t z,
		std::int32_t radius);

//
// Call when the barrel is shot at by the given person.
//

void BARREL_shoot(
		Thing *p_barrel,
		Thing *p_shooter);

//
// Lets people pick up barrels.  This function positions the given barrel
// so that it looks like the person is holding the barrel.  It also stops
// the barrel processing itself. To start the barrel physics again, call
// BARREL_throw()
//

void BARREL_position_on_hands(Thing *p_barrel, Thing *p_person);
void BARREL_throw            (Thing *p_barrel);



