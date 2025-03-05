// EdUtils.h
// Guy Simmons, 5th April 1997.

#include "..\headers\structs.h"
// #define	GAME_SCALE			2560.0

extern std::int32_t current_element;
extern struct KeyFrameElement *the_elements;

void read_object_name(FILE *file_handle, char *dest_string);
void load_multi_vue(struct KeyFrameChunk *the_chunk, float scale);
// void	load_key_frame_chunks(KeyFrameChunk *the_chunk,char* vue_name);
void sort_multi_object(struct KeyFrameChunk *the_chunk);
void setup_anim_stuff();
void reset_anim_stuff();
void load_chunk_texture_info(KeyFrameChunk *the_chunk);
void do_single_shot(std::uint8_t *screen, std::uint8_t *palette);
void do_record_frame(std::uint8_t *screen, std::uint8_t *palette);
std::int32_t write_pcx(char *fname, std::uint8_t *src, std::uint8_t *pal);
void editor_show_work_screen(std::uint32_t flags);
void editor_show_work_window(std::uint32_t flags);

//---------------------------------------------------------------

inline std::uint16_t calc_lights(std::int32_t x, std::int32_t y, std::int32_t z, struct SVector *p_vect) {
#ifdef EDITOR
    std::int32_t dx, dy, dz, dist;
    std::int32_t lx, ly, lz;
    std::uint32_t c0;
    std::int32_t total = 0;
    lx = p_vect->X + x;
    ly = p_vect->Y + y;
    lz = p_vect->Z + z;

    for (c0 = 1; c0 < next_d_light; c0++) {
        dx = abs(lx - d_lights[c0].X);
        dy = abs(ly - d_lights[c0].Y);
        dz = abs(lz - d_lights[c0].Z);

        //		dist=QDIST3(dx,dy,dz);
        dist = dx * dx + dy * dy + dz * dz;

        if (dist == 0)
            dist = 1;
        if (dist < (256 << 11))
            total += (d_lights[c0].Intensity << 11) / dist;
    }
    return ((std::uint16_t) total);
#else
    return (0);
#endif
}

//---------------------------------------------------------------
