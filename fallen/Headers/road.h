//
// Road stuff...
//

#pragma once



//
// The wander system for vehicles.
// 

typedef struct
{
	std::uint8_t	x;
	std::uint8_t	z;

	std::uint8_t	c[4];	// Other nodes connected to this one.

} ROAD_Node;

#define ROAD_MAX_NODES 256

extern ROAD_Node *ROAD_node;
extern std::int32_t      ROAD_node_upto;

//
// The indices of nodes that lead off (or onto) the map.
//

#define ROAD_MAX_EDGES 8

extern std::uint8_t *ROAD_edge;//[ROAD_MAX_EDGES];
extern std::uint16_t ROAD_edge_upto;

//
// The distance from the middle of the road that you want to drive along.
// 

#define ROAD_LANE (0x100)


//
// Creates curbs and cambers.
//

void ROAD_sink();




//
// Returns true if the given square is part of a road.
// Returns true if the given square is on a zebra crossing.
// 

std::int32_t ROAD_is_road (std::int32_t map_x, std::int32_t map_z);
std::int32_t ROAD_is_zebra(std::int32_t map_x, std::int32_t map_z);


//
// Calculates the type of each mqpsquare texture on the map. This
// is really slow!
//

void ROAD_calc_mapsquare_type();


//
// Returns the type of each mapsquare.
// 

#define ROAD_TYPE_TARMAC   0
#define ROAD_TYPE_GRASS    1
#define ROAD_TYPE_DIRT     2
#define ROAD_TYPE_SLIPPERY 3

std::int32_t ROAD_get_mapsquare_type(std::int32_t map_x, std::int32_t map_z);

//
// Calculates the nodes of the vehicle wandering system.
//

void ROAD_wander_calc();


//
// Returns the road nearest to the given point.
//

void ROAD_find(
		std::int32_t world_x,
		std::int32_t world_z,

		std::int32_t *n1,
		std::int32_t *n2);

//
// Returns the position of the given node.
//

void ROAD_node_pos(
		std::int32_t  node,
		std::int32_t *world_x,
		std::int32_t *world_z);

//
// Returns the degree (number of connections) to a given node
//

std::int32_t ROAD_node_degree(std::int32_t node);

//
// Returns the nearer of rn1 and rn2, and the distance squared
//

#define	JN_RADIUS_IN	0x300
#define JN_RADIUS_OUT	0x500
#define AT_JUNCTION		(JN_RADIUS_IN * JN_RADIUS_IN)		// square of distance from car to junction to count as being at / on the junction
#define NEAR_JUNCTION	(JN_RADIUS_OUT * JN_RADIUS_OUT)		// square of distance from car to junction to count as approaching the junction

std::int32_t ROAD_nearest_node(std::int32_t rn1, std::int32_t rn2, std::int32_t wx, std::int32_t wz, std::int32_t* nnd);

//
// Returns another road to drive down after reaching node n2.
//

void ROAD_whereto_now(
		std::int32_t  n1,
		std::int32_t  n2,
		std::int32_t *wtn1,
		std::int32_t *wtn2);

//
// Returns the destination for a length of road.
// 

void ROAD_get_dest(
		std::int32_t  n1,
		std::int32_t  n2,
		std::int32_t *world_x,
		std::int32_t *world_z);

//
// Returns true if the given node is the end of the line! i.e. it
// leads you off the edge of the map.
//

std::int32_t ROAD_is_end_of_the_line(std::int32_t n);

//
// Returns <0 for a left-hand bend, >0 for a right-hand bend and 0 for no bend (or a u-turn)
//

std::int32_t ROAD_bend(std::int32_t n1, std::int32_t n2, std::int32_t n3);

//
// If you've driven off the map and you want to reappear in a new place
// call this function!  Fill in (*world_x,*world_z) with your current
// position (so you won't get anywhere too nearby),
//

void ROAD_find_me_somewhere_to_appear(
		std::int32_t *world_x,		// Current position on calling, new position on return.
		std::int32_t *world_z,
		std::int32_t *nrn1,		// The new road you are on.
		std::int32_t *nrn2,
		std::int32_t *ryaw);		// The new yaw you should be at.

//
// Signed distance from the middle of road.  A negative distance
// means that you are on the wrong side of the road.
//

std::int32_t ROAD_signed_dist(
		std::int32_t n1,	// Going from
		std::int32_t n2,	// Going to
		std::int32_t world_x,
		std::int32_t world_z);

//
// Draws a debug display of the road system.
//

void ROAD_debug();


