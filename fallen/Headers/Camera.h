// Camera.h
// Guy Simmons, 5th December 1997.

#ifndef CAMERA_H
#define CAMERA_H

//---------------------------------------------------------------

#define MAX_CAMERAS 8

#define CAMERA_NONE 0
#define CAMERA_TRACKER 1
#define CAMERA_FIXED 2

//---------------------------------------------------------------

// struct	Entity;		// Prototype 'Entity' structure definition.

struct CameraMan {
    COMMON(CameraType)

    std::int32_t CameraDX,
        CameraDY,
        CameraDZ;
    std::int32_t PrevDX;
    std::int32_t PrevDY;
    std::int32_t PrevDZ;
    void (*StateFn)(Entity *); // Things state function.
    THING_INDEX FocusThing;
};

//---------------------------------------------------------------

void init_cameras();
Entity *alloc_camera(std::uint8_t type);
void free_camera(Entity *camera_thing);
Entity *create_camera(std::uint8_t type, GameCoord *start_pos, Entity *track_thing);
void set_camera_type(Entity *c_thing, std::uint8_t type);
void lock_camera_position(Entity *c_thing, GameCoord *lock_pos, bool snap);
void free_camera_position(Entity *c_thing, GameCoord *rel_pos, bool snap);
void process_t_camera(Entity *camera_thing);
void process_f_camera(Entity *camera_thing);
void spin_camera_around_subject(Entity *c_thing);
void backup_camera(Entity *c_thing);
void restore_old_camera(Entity *c_thing);

//---------------------------------------------------------------

#endif
