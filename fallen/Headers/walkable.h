#ifndef	WALKABLE_H
#define	WALKABLE_H	1


//
// defines
//
#define	calc_height_on_face(x,z,face,new_y) get_height_on_face_quad64_at(x,z,face,new_y)	

#define	FIND_ANYFACE			1
#define	FIND_FACE_BELOW			2
#define	FIND_FACE_NEAR_BELOW	3
//
//structures
//






//
//Data
//



//extern struct 	DInsideRect	inside_rect[MAX_INSIDE_RECT];


//
// Functions
//




//extern std::int32_t	calc_height_on_face(std::int32_t x,std::int32_t z,std::int32_t face,std::int32_t *new_y);
extern std::int32_t	get_height_on_face_quad64_at(std::int32_t rx, std::int32_t rz,std::int16_t face,std::int32_t *height);
extern std::int32_t	find_height_for_this_pos(	std::int32_t  x,std::int32_t  z,	std::int32_t *ret_face);
extern std::int32_t	calc_height_on_rface(std::int32_t x, std::int32_t z,std::int16_t	face,std::int32_t *new_y);



//
// Looks for a roof face over the given mapsquare.
// If it finds one it deletes it.
//

void WALKABLE_remove_rface(std::uint8_t map_x, std::uint8_t map_z);


#endif