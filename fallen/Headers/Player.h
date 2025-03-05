// Player.h
// Guy Simmons, 2nd January 1998.

#ifndef PLAYER_H
#define PLAYER_H

//---------------------------------------------------------------

#ifndef PSX
#define MAX_PLAYERS 2
#else
#define MAX_PLAYERS 2
#endif

#define PLAYER_NONE 0
#define PLAYER_DARCI 1
#define PLAYER_ROPER 2
#define PLAYER_COP 3
#define PLAYER_THUG 4

//---------------------------------------------------------------

typedef struct
{
    COMMON(PlayerType)

    std::uint32_t Input;
    std::uint32_t InputDone;
    std::uint16_t PlayerID;
    std::uint8_t Stamina;
    std::uint8_t Constitution;

    std::uint32_t LastInput;       // The input last gameturn
    std::uint32_t ThisInput;       // The input this gameturn
    std::uint32_t Pressed;         // The keys pressed  this gameturn
    std::uint32_t Released;        // The keys released this gameturn
    std::uint32_t DoneSomething;   // Flag so you know when you've pressed left or done a left-punch.
    std::int32_t LastReleased[16]; // The GetTickCount() of when each key was last released
    std::uint8_t DoubleClick[16];  // The double-click count for each key.

    std::uint8_t Strength;
    std::uint8_t RedMarks;
    std::uint8_t TrafficViolations;
    std::uint8_t Danger; // How far from Danger is Darci? 0 => No danger, 1 = Max danger, 3 = min danger
    // temporarily marked out from the psx until it gets ported across
    std::uint8_t PopupFade; // Bringing the pop-up inventory in and out. Part of player cos of splitscreen mode
    std::int8_t ItemFocus;  // In the inventory, the item you're about to select when you let go
    std::uint8_t ItemCount; // Number of valid inventory items currently held
    std::uint8_t Skill;

    struct Thing *CameraThing,
        *PlayerPerson;

} Player;

typedef Player *PlayerPtr;

//---------------------------------------------------------------

extern GenusFunctions player_functions[];

void init_players();
Thing *alloc_player(std::uint8_t type);
void free_player(Thing *player_thing);
Thing *create_player(std::uint8_t type, std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t id);

//---------------------------------------------------------------

//
// Call when the player gains or looses a red mark.
//

void PLAYER_redmark(std::int32_t playerid, std::int32_t dredmarks);

#endif
