//
// Helicopters.
// wockawockawockawocka
//

#ifndef CHOPPER_H
#define CHOPPER_H

#define MAX_CHOPPERS	4

#define CHOPPER_NONE		0   // zilch, nada
#define CHOPPER_CIVILIAN	1   // basic no-weapons chopper
#define CHOPPER_NUMB		2

#define CHOPPER_substate_idle		0
#define CHOPPER_substate_takeoff	1
#define CHOPPER_substate_landing	2
#define CHOPPER_substate_landed		3
#define CHOPPER_substate_tracking   4
#define CHOPPER_substate_homing     5
#define CHOPPER_substate_patrolling 6


typedef struct
{
	Thing* thing;			// points at its thing. ooer.
	Thing* target;			// tracks this thing.
	GameCoord home;    		// copter's home -- keeps close to here if asked
	std::uint32_t dist;				// generically useful when pathfinding, chasing, etc
	std::int32_t radius;			// keeps inside this area, then returns to home
	std::int32_t patrol;			// rotates for patrolling routines
	std::int32_t spotx,spotz;		// beam target coords
	std::int32_t spotdx,spotdz;	// beam target deltas
	std::int32_t dx,dy,dz;			// motion vector
//	std::int32_t channel;			// audio channel for wockawocka...
	std::int32_t victim;			// who to track, as an EP
	std::int16_t rx,ry,rz;			// rotation vector
	std::uint16_t counter;          // various timings
	std::uint16_t rotors;			// rotor rotation
	std::uint16_t rotorspeed;		// and speed
	std::uint16_t speed;			// preferred cruising speed
	std::uint8_t ChopperType;      // civilian, gunship, etc
	std::uint8_t rotorprim;        // which prim
	std::uint8_t substate;			// takeoff, landing, etc
	std::uint8_t light;			// brightness of beam
	std::uint8_t since_takeoff;	// How long in the air since takeoff.

	std::uint8_t padding;			// padding

}Chopper;

typedef Chopper* ChopperPtr;

void   init_choppers();
struct Thing *alloc_chopper(std::uint8_t type);
void   free_chopper (struct Thing *chopper_thing);


//
// The chopper state functions.
//

extern GenusFunctions CHOPPER_functions[CHOPPER_NUMB];

extern StateFunction CIVILIAN_state_function[];

void CHOPPER_init_state(Thing *chopper_thing, std::uint8_t new_state);

//
// Creates a chopper thing of the given type.  It puts it at the
// given position on the mapwho and puts it into state STATE_INIT.
//

Thing *CHOPPER_create(GameCoord pos, std::uint8_t type);

//
// Returns the Chopper structure associated with the given chopper thing.
// Returns the DrawMesh structure associted with the given chopper thing.
//

Chopper  *CHOPPER_get_chopper (Thing *chopper_thing);
DrawMesh *CHOPPER_get_drawmesh(Thing *chopper_thing);


#endif
