// Switch.h
// Guy Simmons, 9th March 1998.

#ifndef SWITCH_H
#define SWITCH_H

//---------------------------------------------------------------

#define MAX_SWITCHES 2 // 10

#define SWITCH_NONE 0
#define SWITCH_PLAYER 1
#define SWITCH_THING 2
#define SWITCH_GROUP 3
#define SWITCH_CLASS 4

//---------------------------------------------------------------

#define SWITCH_FLAGS_TRIGGERED (1 << 0)
#define SWITCH_FLAGS_RESET (1 << 1)

#define SCAN_MODE_SPHERE 0
#define SCAN_MODE_RECT 1

//---------------------------------------------------------------

typedef struct
{
    COMMON(SwitchType)

    std::uint8_t ScanMode;
    std::uint8_t padtoword;
    std::uint16_t Scanee;

    std::int32_t Depth,
        Height,
        Radius,
        Width;
} Switch;

typedef Switch *SwitchPtr;

//---------------------------------------------------------------

void init_switches();
Entity *alloc_switch(std::uint8_t type);
void free_switch(Entity *person_thing);
THING_INDEX create_switch();

void fn_switch_player(Entity *s_thing);
void fn_switch_thing(Entity *s_thing);
void fn_switch_group(Entity *s_thing);
void fn_switch_class(Entity *s_thing);

//---------------------------------------------------------------

#endif
