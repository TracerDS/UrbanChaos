// State.h
// Guy Simmons, 4th January 1998.

#ifndef STATE_H
#define STATE_H

struct Entity;
//---------------------------------------------------------------

typedef struct
{
    std::uint8_t State;
    void (*StateFn)(Entity *);
} StateFunction;

//---------------------------------------------------------------

typedef struct
{
    std::uint8_t Genus;
    StateFunction *StateFunctions;
} GenusFunctions;

//---------------------------------------------------------------

extern void set_state_function(Entity *t_thing, std::uint8_t state);
extern void set_generic_person_state_function(Entity *t_thing, std::uint8_t state);
extern void set_generic_person_just_function(Entity *t_thing, std::uint8_t state);

//---------------------------------------------------------------

#endif
