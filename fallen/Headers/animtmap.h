#ifndef ANIM_TMAP_H
#define ANIM_TMAP_H

// defines

#define MAX_TMAP_FRAMES 16
#define MAX_ANIM_TMAPS 16

// structures
struct AnimTmap {
    std::uint8_t UV[MAX_TMAP_FRAMES][4][2];
    std::int8_t Delay[MAX_TMAP_FRAMES];
    std::uint8_t Page[MAX_TMAP_FRAMES];
    std::uint16_t Current;
    std::uint16_t Timer;
    std::uint16_t Flags;
};

// data
extern struct AnimTmap anim_tmaps[MAX_ANIM_TMAPS];

// functions
extern void save_animtmaps();
extern void sync_animtmaps();
extern void animate_texture_maps();
extern void load_animtmaps();

#endif