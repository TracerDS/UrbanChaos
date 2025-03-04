//
// Fire!
//

#include "game.h"
#include "fire.h"


//
// Each flame.
//

#define FIRE_MAX_FLAME_POINTS 4

typedef struct
{
	std::int8_t dx;
	std::int8_t dz;
	std::uint8_t die;
	std::uint8_t counter;
	std::uint8_t height;
	std::uint8_t next;
	std::uint8_t points;
	std::uint8_t shit;

	std::uint8_t angle [FIRE_MAX_FLAME_POINTS];
	std::uint8_t offset[FIRE_MAX_FLAME_POINTS];

} FIRE_Flame;

#define FIRE_MAX_FLAMES 256

FIRE_Flame FIRE_flame[FIRE_MAX_FLAMES];
std::int32_t FIRE_flame_free;

//
// Fire is a linked list of flames.
//

typedef struct
{
	std::uint8_t num;		// The number of flames this fire has, 0 => the fire is unused.
	std::uint8_t next;		// The flames.
	std::uint8_t size;
	std::uint8_t shrink;
	std::uint16_t x;
	std::int16_t y;
	std::uint16_t z;
	
} FIRE_Fire;

#define FIRE_MAX_FIRE 8

FIRE_Fire FIRE_fire[FIRE_MAX_FIRE];
std::int32_t FIRE_fire_last;


void FIRE_init()
{
	std::int32_t i;

	//
	// Initialise the linked list of flames.
	//

	FIRE_flame_free = 1;

	for (i = 1; i < FIRE_MAX_FLAMES; i++)
	{
		FIRE_flame[i].next = i + 1;
	}

	FIRE_flame[FIRE_MAX_FLAMES - 1].next = 0;

	//
	// Make all the fire as unused.
	//

	FIRE_fire_last = 0;

	for (i = 0; i < FIRE_MAX_FIRE; i++)
	{
		FIRE_fire[i].num = 0;
	}
}


//
// Returns how many flames a fire of the given size should have.
//

std::uint8_t FIRE_num_flames_for_size(std::uint8_t size)
{
	return size >> 2;
}


//
// Adds a flame to the given fire structure.
//

void FIRE_add_flame(FIRE_Fire *ff)
{
	std::int32_t i;

	std::int32_t dx;
	std::int32_t dz;
	std::int32_t dist;
	std::uint8_t flame;

	ASSERT(WITHIN(ff, &FIRE_fire[0], &FIRE_fire[FIRE_MAX_FIRE - 1]));

	FIRE_Flame *fl;

	if (!FIRE_flame_free )
	{
		//
		// No spare flame structure.
		//

		return;
	}

	ASSERT(WITHIN(FIRE_flame_free, 1, FIRE_MAX_FLAMES - 1));

	flame           =  FIRE_flame_free;
	fl              = &FIRE_flame[FIRE_flame_free];
	FIRE_flame_free =  fl->next;

	dx   = (Random() & 0xff) - 0x1f;
	dz   = (Random() & 0xff) - 0x1f;
	dist = abs(dx) + abs(dz);

	fl->dx      = dx;
	fl->dz      = dz;
	fl->height  = 255 - dist;
	fl->counter = 0;
	fl->die     = 32 + (Random() & 0x1f);
	fl->points  = (fl->height >> 6) + 1;

	SATURATE(fl->points, 2, FIRE_MAX_FLAME_POINTS);

	for (i = 0; i < fl->points; i++)
	{
		fl->angle [i] = Random();
		fl->offset[i] = Random();
	}

	//
	// Add this flame to the fire structure.
	//

	fl->next = ff->next;
	ff->next = flame;
	ff->num += 1;
}


void FIRE_create(
		std::uint16_t x,
		std::int16_t y,
		std::uint16_t z,
		std::uint8_t size,
		std::uint8_t life)
{
	std::int32_t i;

	FIRE_Fire *ff;

	//
	// Look for a spare fire structure.
	//

	for (i = 0; i < FIRE_MAX_FIRE; i++)
	{
		FIRE_fire_last += 1;

		if (FIRE_fire_last >= FIRE_MAX_FIRE)
		{
			FIRE_fire_last = 0;
		}

		if (!FIRE_fire[FIRE_fire_last].num)
		{
			goto found_spare_fire_structure;
		}
	}

	//
	// No spare fire structures!
	//

	return;

  found_spare_fire_structure:;

	ff = &FIRE_fire[FIRE_fire_last];

	ff->x      = x;
	ff->y      = y;
	ff->z      = z;
	ff->size   = size;
	ff->shrink = life;
	ff->num    = 0;

	//
	// Add a flame to this fire structure to mark it as used.
	//

	FIRE_add_flame(ff);
}



void FIRE_process()
{
	std::int32_t  i;
	std::int32_t  j;
	std::uint8_t  flame;
	std::uint8_t  next;
	std::uint8_t *prev;

	FIRE_Fire  *ff;
	FIRE_Flame *fl;

	for (i = 0; i < FIRE_MAX_FIRE; i++)
	{
		ff = &FIRE_fire[i];

		if (ff->num)
		{
			//
			// Process all the nexts of this fire.
			//

			for (next = ff->next; next; next = fl->next)
			{
				ASSERT(WITHIN(next, 1, FIRE_MAX_FLAMES - 1));

				fl = &FIRE_flame[next];

				fl->counter += 1;

				for (j = 1; j < fl->points; j++)
				{
					if (j & 1)
					{
						fl->angle [j] += 31;
						fl->offset[j] -= 17;
					}
					else
					{
						fl->angle [j] -= 33;
						fl->offset[j] += 21;
					}
				}
			}

			//
			// Delete all the dead flames of this fire.
			//

			prev = &ff->next;
			next =  ff->next;

			while(next)
			{
				ASSERT(WITHIN(next, 1, FIRE_MAX_FLAMES - 1));

				fl = &FIRE_flame[next];
				
				if (fl->counter >= fl->die)
				{
					//
					// Kill this flame.
					//

					flame = next;

				   *prev  = fl->next;
					next  = fl->next;

					//
					// Put it in the free list.
					//

					fl->next        = FIRE_flame_free;
					FIRE_flame_free = flame;

					ff->num -= 1;
				}
				else
				{
					//
					// Keep this flame.
					//

					prev = &fl->next;
					next =  fl->next;
				}
			}

			if ((GAME_TURN & 0x3) == 0)
			{
				std::int32_t size;
				
				size  = ff->size;
				size -= ff->shrink;

				if (size < 0) {size = 0;}

				ff->size = size;
			}

			//
			// If this fire does not have enough flames...
			//

			if (ff->num < FIRE_num_flames_for_size(ff->size))
			{
				FIRE_add_flame(ff);
			}
		}
	}
}

#define FIRE_MAX_POINTS 16

std::uint8_t FIRE_get_z;
std::uint8_t FIRE_get_xmin;
std::uint8_t FIRE_get_xmax;
std::uint8_t FIRE_get_fire_upto;
std::uint8_t FIRE_get_flame;
FIRE_Info FIRE_get_info;
FIRE_Point FIRE_get_point[FIRE_MAX_POINTS];

void FIRE_get_start(std::uint8_t z, std::uint8_t xmin, std::uint8_t xmax)
{
	FIRE_get_fire_upto = 0;
	FIRE_get_flame     = 0;
	FIRE_get_z         = z;
	FIRE_get_xmin      = xmin;
	FIRE_get_xmax      = xmax;
}

FIRE_Info *FIRE_get_next()
{
	FIRE_Flame *fl;

	if (!FIRE_get_flame )
	{
		while(1)
		{
			ASSERT(WITHIN(FIRE_get_fire_upto, 0, FIRE_MAX_FIRE - 1));

			if (FIRE_fire[FIRE_get_fire_upto].num)
			{
				FIRE_get_flame      = FIRE_fire[FIRE_get_fire_upto].next;
				FIRE_get_fire_upto += 1;

				break;
			}

			FIRE_get_fire_upto += 1;

			if (FIRE_get_fire_upto >= FIRE_MAX_FIRE)
			{
				return nullptr;
			}
		}
	}

	ASSERT(WITHIN(FIRE_get_flame, 1, FIRE_MAX_FLAMES - 1));

	fl = &FIRE_flame[FIRE_get_flame];

	//
	// Create the points of the flame.
	//
	
	return nullptr;
}
