#ifndef PRIM_EDIT_H
#define PRIM_EDIT_H 1

extern void calc_prims_screen_box(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z, EdRect* rect);
extern void calc_prims_world_box(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z, EdRect* rect);

extern void save_asc(std::uint16_t building, std::uint16_t version);
extern void import_tex(char* fname);
extern void export_tex(char* fname);

extern std::int8_t read_asc(char* fname, std::int32_t scale, std::uint32_t offset);
extern std::int32_t read_multi_asc(char* asc_name, std::uint8_t flag, float scale = 1.0);
extern std::int8_t read_multi_dxf();
extern std::int8_t read_multi_vue(std::int32_t m_object);
extern void load_textures_for_prim(char* str, std::uint16_t prim);
extern void save_textures_for_prim(char* str, std::uint16_t prim);
extern std::int16_t SelectFlag;
extern std::int16_t SelectDrawn;
extern void calc_prims_screen_box(std::uint16_t prim, std::int32_t x, std::int32_t y, std::int32_t z, EdRect* rect);

//
// Saves out the given prim object. Returns false on failure.
//

std::int32_t save_prim_object(std::int32_t prim);

//
// Saves out all the loaded prims as individual objects.
//

void save_all_individual_prims();

#endif
