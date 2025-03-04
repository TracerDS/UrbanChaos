//
// Precalculates shadows on a built city.
//

#ifndef _SHADOW_
#define _SHADOW_


//
// The SHADOW module creates 3bit values for a square-
// and this is what they mean.
//
//
//	0: No shadows
//
//	1:
//		+------+
//      |     /|
//      |    /#|
//      |   /m#|
//      |  /om#|
//      | /'om#|
//      |/ 'om#|
//      +------+
//
//	2:
//		+------+
//      |  'om/|
//      |  'o/#|
//      |  '/m#|
//      |  /om#|
//      | /'om#|
//      |/ 'om#|
//      +------+
//
//	3:
//		+------+
//      |  'om/|
//      |  'o/m|
//      |  '/oo|
//      |  /'''|
//      | /    |
//      |/     |
//      +------+
//
//	4:
//		+------+
//      |#####/|
//      |mmmm/m|
//      |ooo/oo|
//      |''/'''|
//      | /    |
//      |/     |
//      +------+
//
//	5:
//		+------+
//      |#####/|
//      |mmmm/#|
//      |ooo/m#|
//      |''/om#|
//      | /'om#|
//      |/ 'om#|
//      +------+
//
//	6:
//		+------+
//      |  'om/|
//      |  'o/#|
//      |  '/m#|
//      |  /om#|
//      | /'om#|
//      |/ 'om#|
//      +------+
//
//	7:
//		+------+
//      |#####/|
//      |mmmm/ |
//      |ooo/  |
//      |''/   |
//      | /    |
//      |/     |
//      +------+
//



//
// Shadows the outside world.
//

void SHADOW_do();



//
// Returns true if the given point is in shadow.
//

std::int32_t SHADOW_in(std::int32_t x, std::int32_t y, std::int32_t z);



#endif

