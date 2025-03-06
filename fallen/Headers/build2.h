//
// The other building file!  A simpler map...
//

#ifndef _BUILD2_
#define _BUILD2_

//
// Puts all the facets and walkable faces on the mapwho.
//

void build_quick_city();

//
// Puts an single walkable face onto the Walkable mapwho.
// Removes a walkable face from the mapwho.
//

void attach_walkable_to_map(std::int32_t face);
void remove_walkable_from_map(std::int32_t face);

//
// Adds a dfacet to the mapwho.
//

void add_facet_to_map(std::int32_t dfacet);

#endif
