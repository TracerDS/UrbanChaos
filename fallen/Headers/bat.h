//
// Bats/gargoles that circle an area and swoop to attack the player.
//

#pragma once



//
// The bat structures.
//

#define BAT_TYPE_UNUSED		0
#define BAT_TYPE_BAT		1
#define BAT_TYPE_GARGOYLE	2
#define BAT_TYPE_BALROG		3
#define BAT_TYPE_BANE		4

typedef struct
{
	std::uint8_t type;
	std::uint8_t health;
	std::uint8_t state;
	std::uint8_t substate;
	std::uint8_t home_x;
	std::uint8_t home_z;
	std::uint16_t target;
	std::uint16_t timer;
	std::uint16_t flag;
	std::int16_t want_x;
	std::int16_t want_y;
	std::int16_t want_z;
	std::uint16_t glow;	// How much bane glows...
	std::int32_t dx;
	std::int32_t dy;
	std::int32_t dz;

} Bat;

typedef Bat *BatPtr;

#define RBAT_MAX_BATS 40 //32
#define BAT_MAX_BATS  (save_table[SAVE_TABLE_BAT].Maximum)


//
// Initailises the bats.
//

void BAT_init();


//
// Creates a new bat thing. Returns the THING_INDEX or nullptr if
// it couldn't create a bat.
//

THING_INDEX BAT_create(
				std::int32_t type,		// BAT_TYPE_*
				std::int32_t x,
				std::int32_t z,
				std::uint16_t yaw);


//
// The bat recieves a wound from the aggressor.
//

void BAT_apply_hit(
		Thing *p_me,
		Thing *p_aggressor,
		std::int32_t  damage);


