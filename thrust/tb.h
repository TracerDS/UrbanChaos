//
// Tractor beams!
// 

#pragma once

#include "orb.h"
#include "ship.h"



#define TB_FLAG_USED   (1 << 0)
#define TB_FLAG_LOCKED (1 << 1)

typedef struct
{
	std::uint16_t flag;
	std::uint8_t ship;
	std::uint8_t orb;
	float length;
 
} TB_Tb;

#define TB_MAX_TBS 16

extern TB_Tb TB_tb[TB_MAX_TBS];


//
// Initialises the tractor beams.
//

void TB_init();



//
// Creates/destroys a tractor beam. If the ship is not close enough to
// an orb, the creation function returns nullptr.
//

TB_Tb *TB_create (SHIP_Ship *ss, float length);
void TB_destroy(TB_Tb     *tt);



//
// Processes all the tractor beams.
//

void TB_process_all();



//
// Draws the tractor beams.
//

void TB_draw_all(float mid_x, float mid_y, float zoom);



