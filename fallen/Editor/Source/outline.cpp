//
// Functions for helping with the outline of shapes on grids.
//

#include	"../Headers/Editor.hpp"
#include <MFStdLib.h>


//
// An outline.
//

#define OUTLINE_LINK_TYPE_START 0
#define OUTLINE_LINK_TYPE_END   1

struct OUTLINE_Link
{
	std::int32_t x;
	std::int32_t type;
	OUTLINE_Link* next;
};

struct OUTLINE_Outline
{
	std::int32_t max_z;
	OUTLINE_Link* link[];
};


OUTLINE_Outline* OUTLINE_create(std::int32_t num_z_squares)
{
	std::int32_t num_bytes;

	OUTLINE_Outline* oo;
	
	//
	// How much memory does this outline take?
	//
	
	num_bytes = sizeof(OUTLINE_Outline) + sizeof(OUTLINE_Link*) * num_z_squares;

	oo = (OUTLINE_Outline *) calloc(num_bytes, 1);
	if (!oo)
		return nullptr;

	//
	// Initialise it.
	//
	oo->max_z = num_z_squares;

	return oo;
}


//
// Inserts the link into the correct place in the outline.
//

void OUTLINE_insert_link(OUTLINE_Outline *oo, OUTLINE_Link *ol, std::int32_t link_z)
{
	std::int32_t here;

	OUTLINE_Link  *next;
	OUTLINE_Link **prev;

	ASSERT(WITHIN(link_z, 0, oo->max_z - 1));

	prev = &oo->link[link_z];
	next =  oo->link[link_z];

	while(1)
	{
		here = false;

		if (!next )
		{
			//
			// We must insert the link here.
			//

			here = true;
		}
		else
		if (next->x > ol->x)
		{
			here = true;
		}
		else
		if (next->x == ol->x)
		{
			//
			// For links at the same x, insert END links before START links.
			//

			if (ol->x == OUTLINE_LINK_TYPE_END)
			{
				here = true;
			}
		}

		if (here)
		{
			//
			// This is where we insert the link.
			//

		    ol->next = next;
		   *prev     = ol;

			return;
		}

		prev = &next->next;
		next =  next->next;
	}
}



void OUTLINE_add_line(
		OUTLINE_Outline *oo,
		std::int32_t x1, std::int32_t z1,
		std::int32_t x2, std::int32_t z2)
{
	std::int32_t z;

	std::int32_t type;

	ASSERT(x1 == x2 || z1 == z2);

	ASSERT(WITHIN(z1, 0, oo->max_z - 1));
	ASSERT(WITHIN(z2, 0, oo->max_z - 1));

	if (z1 == z2)
	{
		//
		// We can ignore this line.
		//

		return;
	}

	if (z1 > z2)
	{	
		SWAP(z1,z2);

		type = OUTLINE_LINK_TYPE_START;
	}
	else
	{
		type = OUTLINE_LINK_TYPE_END;
	}

	//
	// Insert it in all the zlines.
	//

	for (z = z1; z < z2; z++)
	{
		//
		// Allocate a new link.
		//

		OUTLINE_Link *ol = (OUTLINE_Link *) malloc(sizeof(OUTLINE_Link));

		ol->type = type;
		ol->x    = x1;
		ol->next = nullptr;

		//
		// Insert it.
		//

		OUTLINE_insert_link(oo, ol, z);
	}
}


void OUTLINE_free(OUTLINE_Outline *oo)
{
	std::int32_t z;

	OUTLINE_Link *ol;
	OUTLINE_Link *next;

	//
	// Free all the links first.
	//

	for (z = 0; z < oo->max_z; z++)
	{
		for (ol = oo->link[z]; ol; ol = next)
		{
			next = ol->next;

			free(ol);
		}
	}

	//
	// Now free the actual outline.
	//

	free(oo);
}


//
// Returns true if the two linked lists overlap.
//

std::int32_t OUTLINE_overlap(
		OUTLINE_Link *ol1,
		OUTLINE_Link *ol2)
{
	std::int32_t on1 = false;
	std::int32_t on2 = false;

	while(1)
	{
		if (ol1 == nullptr ||
			ol2 == nullptr)
		{
			if (!ol1 ) {ASSERT(!on1);}
			if (!ol2 ) {ASSERT(!on2);}

			return false;
		}

		if ((ol1->x < ol2->x) || (ol1->x == ol2->x && ol1->type == OUTLINE_LINK_TYPE_END))
		{
			//
			// Move on the first line.
			//


			if((!on1 && ol1->type == OUTLINE_LINK_TYPE_START) ||( on1 && ol1->type == OUTLINE_LINK_TYPE_END))
			{

			}
			else
			return(0);
			ASSERT(
				(!on1 && ol1->type == OUTLINE_LINK_TYPE_START) ||
				( on1 && ol1->type == OUTLINE_LINK_TYPE_END));

			ol1  = ol1->next;
			on1 ^= true;
		}
		else
		if (ol2->x < ol1->x || (ol1->x == ol2->x && ol2->type == OUTLINE_LINK_TYPE_END))
		{
			//
			// Move on the second line.
			//

				if((!on2 && ol2->type == OUTLINE_LINK_TYPE_START) ||( on2 && ol2->type == OUTLINE_LINK_TYPE_END))
				{
				}
				else
				{
					return(0);
				}
			ASSERT(
				(!on2 && ol2->type == OUTLINE_LINK_TYPE_START) ||
				( on2 && ol2->type == OUTLINE_LINK_TYPE_END));

			ol2  = ol2->next;
			on2 ^= true;
		}
		else
		if (ol1->x == ol2->x)
		{
			ASSERT(ol1->type == OUTLINE_LINK_TYPE_START);
			ASSERT(ol2->type == OUTLINE_LINK_TYPE_START);

			//
			// Neither line is ending here (OUTLINE_LINK_TYPE_END) so they must be
			// both starting here- so they must overlap.
			//

			return true;
		}

		if (on1 && on2)
		{
			return true;
		}
	}
}


std::int32_t OUTLINE_overlap(
		OUTLINE_Outline *oo1,
		OUTLINE_Outline *oo2)
{
	std::int32_t z;
	std::int32_t minz = MIN(oo1->max_z, oo2->max_z);

	for (z = 0; z < minz; z++)
	{
		if (OUTLINE_overlap(
				oo1->link[z],
				oo2->link[z]))
		{
			return true;
		}
	}

	return false;
}

//
// Returns true if the given square is in the outline.
//

std::int32_t OUTLINE_inside(
		OUTLINE_Outline *oo,
		std::int32_t x,
		std::int32_t z)
{
	OUTLINE_Link *ol;

	if (!WITHIN(z, 0, oo->max_z - 1))
	{
		//
		// Not in z-range.
		//

		return false;
	}

	ol = oo->link[z];

	while(1)
	{
		if (!ol || !ol->next )
		{
			return false;
		}

		if (ol->type       == OUTLINE_LINK_TYPE_START &&
			ol->next->type == OUTLINE_LINK_TYPE_END)
		{
			if (WITHIN(x, ol->x, ol->next->x - 1))
			{
				return true;
			}
		}

		ol = ol->next;
	}
}


std::int32_t OUTLINE_intersects(
		OUTLINE_Outline *oo,
		std::int32_t x1, std::int32_t z1,
		std::int32_t x2, std::int32_t z2)
{
	std::int32_t i;
	std::int32_t x;
	std::int32_t z;
	std::int32_t dx;
	std::int32_t dz;
	std::int32_t mx1;
	std::int32_t mz1;
	std::int32_t mx2;
	std::int32_t mz2;
	std::int32_t len;

	ASSERT(x1 == x2 || z1 == z2);

	dx = x2 - x1;
	dz = z2 - z1;

	len = MAX(abs(dx),abs(dz));

	x = x1 << 8;
	z = z1 << 8;

	dx = SIGN(dx);
	dz = SIGN(dz);

	x += dx << 7;
	z += dz << 7;

	for (i = 0; i < len; i++)
	{
		mx1 = x - (dz << 7) >> 8;
		mz1 = z + (dx << 7) >> 8;

		mx2 = x + (dz << 7) >> 8;
		mz2 = z - (dx << 7) >> 8;

		if (OUTLINE_inside(oo, mx1, mz1) ||
			OUTLINE_inside(oo, mx2, mz1))
		{
			return true;
		}

		x += dx << 8;
		z += dz << 8;
	}

	return false;
}

