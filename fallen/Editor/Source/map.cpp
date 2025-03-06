#include "Editor.hpp"

#include "map.h"

struct DepthStrip edit_map[EDIT_MAP_WIDTH][EDIT_MAP_DEPTH]; // 2meg
std::uint16_t tex_map[EDIT_MAP_WIDTH][EDIT_MAP_DEPTH];

std::int8_t edit_map_roof_height[EDIT_MAP_WIDTH][EDIT_MAP_DEPTH];
