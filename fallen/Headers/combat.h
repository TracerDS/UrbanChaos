#ifndef COMBAT_H

#define COMBAT_H 1

//
// Defines
//

#define HIT_TYPE_GUN_SHOT_H (1)
#define HIT_TYPE_GUN_SHOT_M (2)
#define HIT_TYPE_GUN_SHOT_L (3)
#define HIT_TYPE_PUNCH_H (4)
#define HIT_TYPE_PUNCH_M (5)
#define HIT_TYPE_PUNCH_L (6)
#define HIT_TYPE_KICK_H (7)
#define HIT_TYPE_KICK_M (8)
#define HIT_TYPE_KICK_L (9)

#define HIT_TYPE_GUN_SHOT_PISTOL (10)
#define HIT_TYPE_GUN_SHOT_SHOTGUN (11)
#define HIT_TYPE_GUN_SHOT_AK47 (12)

#define MAX_HISTORY 20
#define MAX_MOVES 16 // must be even

#define COMBAT_NONE 0
#define COMBAT_PUNCH 1
#define COMBAT_KICK 2
#define COMBAT_KNIFE 3
//
// Structs
//

struct ComboHistory {
    std::uint16_t Owner;
    std::int8_t Power[MAX_MOVES];
    std::int8_t Moves[MAX_MOVES];
    std::uint16_t Times[MAX_MOVES];
    std::uint8_t Result[MAX_MOVES];
    std::uint16_t LastUsed;
    std::uint8_t Index;
    std::uint8_t Count;
};

struct BlockingHistory {
    std::uint16_t Owner;            // who this blocking history is for
    std::uint8_t Attack[MAX_MOVES]; // attack move performed
    std::uint8_t Flags[MAX_MOVES];  // did it hit, did I block it
    std::uint16_t Times[MAX_MOVES]; // at what time did it occur
    std::uint16_t Perp[MAX_MOVES];  // who perpetrated the attack
    std::uint16_t LastUsed;         // when was this whoile structure used
    std::uint8_t Index;
    std::uint8_t Count;
};

//
// Owner is the person under attack by multiple foes
// This structure has slots for angles that enemies are attacking from
struct GangAttack {
    std::uint16_t Owner;   // who this gang attack is for
    std::uint16_t Perp[8]; // who is attacking in each of the eight compass points

    std::uint16_t LastUsed; // when was this whole structure used
    std::uint8_t Index;
    std::uint8_t Count;
};

//
// Data
//

extern struct ComboHistory combo_histories[MAX_HISTORY];
extern struct GangAttack gang_attacks[MAX_HISTORY];

//
// Functions
//

extern std::int32_t get_combat_type_for_node(std::uint8_t current_node);
extern std::int32_t get_anim_and_node_for_action(std::uint8_t current_node, std::uint8_t action, std::uint16_t *new_anim);
extern std::int32_t apply_violence(Entity *p_thing);
extern std::int32_t apply_hit_to_person(Entity *p_thing, std::int32_t angle, std::int32_t type, std::int32_t damage, Entity *p_aggressor, struct GameFightCol *fight);

//
// Looks for a target in the given direction relative to the given person.
// Returns a position and angle the person would like to be to have an optimum
// fighting stance against the person.
//

#define FIND_DIR_FRONT 1      // For a frontal attack
#define FIND_DIR_BACK 2       // For a backwards attack
#define FIND_DIR_LEFT 3       // For a left attack
#define FIND_DIR_RIGHT 4      // For a right attack
#define FIND_DIR_TURN_LEFT 5  // For turning to attack a person on your left  from the front
#define FIND_DIR_TURN_RIGHT 6 // For turning to attack a person on your right from the front

#define FIND_DIR_MASK (0xff)

#define FIND_DIR_DONT_TURN (1 << 10)

std::int32_t find_attack_stance(
    Entity *p_person,
    std::int32_t attack_direction,
    std::int32_t attack_distance, // Desired distance from a person to attack them. 8-bits per mapsquare.
    Entity **stance_target,
    GameCoord *stance_position, // 16-bits per mapsquare position at the desired distance.
    std::int32_t *stance_angle);

//
// Turns so you are facing someone in the given direction.
//

std::int32_t turn_to_target(
    Entity *p_person,
    std::int32_t find_dir);

//
// Looks for someone in front of you to punch, turns and positions yourself
// to punch them and then does the punch- the same except you kick.
//

std::int32_t turn_to_target_and_punch(Entity *p_person);
std::int32_t turn_to_target_and_kick(Entity *p_person);

//
// Finds the best anims for the person to punch or kick someone. If no
// anim is any good the funcion either returns nullptr, or, if (flag &
// FUND_BEST_USE_DEFAULT) it returns a default punch/kick.
//

#define FIND_BEST_USE_DEFAULT (1 << 0)

std::int32_t find_best_punch(Entity *p_person, std::uint32_t flag);
std::int32_t find_best_kick(Entity *p_person, std::uint32_t flag);

//
// If somebody is attacking the given person, it returns a pointer to
// the attacker. If there is more than one attacker- it picks the nearest
// and returns him.
//

Entity *is_person_under_attack(Entity *p_person);
Entity *is_anyone_nearby_alive(Entity *p_person); // The nearest person alive to you.

#endif
