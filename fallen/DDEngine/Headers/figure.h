//
// Draws a person.
//

#pragma once


//
// This function uses the POLY module, and assumes
// that all the camera stuff has already been set up.
//

void FIGURE_draw(Thing *person);

extern std::int32_t	FIGURE_alpha;


//
// Draws a reflection of the person about the plane, y = height.  It fills in
// the bounding box of the object on screen.  The bounding box isn't necessarily
// on screen!
//

extern std::int32_t FIGURE_reflect_x1;
extern std::int32_t FIGURE_reflect_y1;
extern std::int32_t FIGURE_reflect_x2;
extern std::int32_t FIGURE_reflect_y2;

void FIGURE_draw_reflection(Thing *person, std::int32_t height);


//
// What's this doing in figure.h?
// Don't ask me. I was just told to dump it here...
//

void    init_flames();


//
// Draws an animating prim.
//

void ANIM_obj_draw(Thing *p_thing, DrawTween *dt);




