//
// An A-Z of the current city.
//

#pragma once


//
// Initialises the AZ module.
//

void AZ_init();


//
// Creates a line-based representation of the current city. Make sure
// you have already sunken the roads before calling this function.
//

#define AZ_LINE_TYPE_ROAD		0
#define AZ_LINE_TYPE_BUILDING	1
#define AZ_LINE_TYPE_FENCE		2
#define AZ_LINE_TYPE_NUMBER		3

typedef struct
{
	std::uint8_t type;
	std::uint8_t x1;
	std::uint8_t z1;
	std::uint8_t x2;
	std::uint8_t z2;
	
} AZ_Line;

extern AZ_Line AZ_line[];
extern std::int32_t   AZ_line_upto;

void AZ_create_lines();





