#ifndef OVERLAY_H
#define OVERLAY_H

extern void OVERLAY_handle();
extern void track_enemy(Entity *p_thing);
extern void track_gun_sight(Entity *p_thing, std::int32_t accuracy);

#endif
