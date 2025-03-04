#pragma once

//
// Extra stuff like puddles and mist.
//

#define EXTRA_TYPE_NONE		0
#define EXTRA_TYPE_PUDDLE	1
#define EXTRA_TYPE_MIST		2

typedef struct
{
	std::uint8_t type;
	std::uint8_t height;
	std::uint8_t detail;
	std::uint8_t shit1;
	std::uint16_t x;
	std::uint16_t z;
	std::uint16_t angle;
	std::uint16_t radius;
	std::int32_t shit2;
	std::int32_t shit3;

} EXTRA_Thing;

#define EXTRA_MAX_THINGS 256

extern EXTRA_Thing EXTRA_thing[EXTRA_MAX_THINGS];


//
// Creates something of the given type with the default
// settings at (x,z).  If something of that type already
// exists there, then it is deleted.
//

#define EXTRA_SELECT_DIST 256	// How close two things of the same type can be from eachother.

void EXTRA_create_or_delete(std::int32_t type, std::int32_t x, std::int32_t z);



