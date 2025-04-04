// Game.h
// Guy Simmons, 17th October 1997.

#pragma once

#include <cstdint>

#ifdef PSX
#define DRAW_DIST 13
#define BUILD_PSX
#else
#define DRAW_DIST 22
#endif

//---------------------------------------------------------------
// PC - D3D Specific defines.

#ifndef PSX
#pragma warning(disable : 4244) // truncation warning : useless
#pragma warning(disable : 4101) // unreferenced local : no-one ever bothers deleting these
#pragma warning(disable : 4554) // yes, well ... some people use brackets, others disable this warning
#endif

#define THING_INDEX std::uint16_t
#define COMMON_INDEX std::uint16_t

//---------------------------------------------------------------
// PC - Glide specific defines.
/*
#define	VERSION_GLIDE
#define	THING_INDEX			std::int32_t
#define	COMMON_INDEX		std::int32_t
*/
//---------------------------------------------------------------
// PSX specific defines.
/*
#define	VERSION_PsX
#define	THING_INDEX			std::int16_t
#define	COMMON_INDEX		std::int16_t
*/
//---------------------------------------------------------------

#include <MFStdLib.h>

#ifndef PSX
#undef LogText
#define LogText
#endif

using MAPCO8 = std::int32_t;
using MAPCO16 = std::int32_t;
using MAPCO24 = std::int32_t;

//
// Memory table, the source of all our power
//
struct MemTable {
    char* Name;
    void** Point;
    std::uint8_t Type;
    std::int32_t* CountL;
    std::uint16_t* CountW;
    std::int32_t Maximum;
    std::int16_t StructSize;
    std::int16_t Extra;
};
extern struct MemTable save_table[];

#define SAVE_TABLE_VEHICLE 29
#define SAVE_TABLE_PEOPLE 30
#define SAVE_TABLE_SPECIAL 36
#define SAVE_TABLE_BAT 38
#define SAVE_TABLE_DTWEEN 40
#define SAVE_TABLE_DMESH 41
#define SAVE_TABLE_WMOVE 46
#define SAVE_TABLE_POINTS 15
#define SAVE_TABLE_FACE4 16
#define SAVE_TABLE_PLATS 45

#include "Structs.h"
#include "State.h"
// #include "Level.h"

#include "drawtype.h"
#include "building.h"
#include "bike.h"
#include "Furn.h"
#include "Vehicle.h"
#include "inline.h"
#include "Person.h"
#include "Animal.h"
#include "barrel.h"
#include "Chopper.h"
#include "Pyro.h"
#include "Player.h"
#include "Plat.h"
#include "Pjectile.h"
#include "Special.h"
#include "bat.h"
#include "Switch.h"
#include "tracks.h"
#include "Thing.h"
#include "Controls.h"
#include "Map.h"
#include "collide.h"
#include "interact.h"

#ifdef VERSION_D3D
#include "aeng.h"
#endif

#ifdef PSX
#include "psxeng.h"
#define _MAX_PATH 260
#include "memory.h"
#endif

#ifdef EDITOR
// #include "Editor.h"
#endif

//---------------------------------------------------------------
#define MAX_RADIUS (24)

// Thing classes.
#define CLASS_NONE 0
#define CLASS_PLAYER 1
#define CLASS_CAMERA 2
#define CLASS_PROJECTILE 3
#define CLASS_BUILDING 4
#define CLASS_PERSON 5
#define CLASS_ANIMAL 6
#define CLASS_FURNITURE 7
#define CLASS_SWITCH 8
#define CLASS_VEHICLE 9
#define CLASS_SPECIAL 10
#define CLASS_ANIM_PRIM 11
#define CLASS_CHOPPER 12
#define CLASS_PYRO 13
#define CLASS_TRACK 14
#define CLASS_PLAT 15
#define CLASS_BARREL 16
#define CLASS_BIKE 17
#define CLASS_BAT 18
#define CLASS_END 19

// face flags

#define POLY_FLAG_GOURAD (1 << 0)
#define POLY_FLAG_TEXTURED (1 << 1)
#define POLY_FLAG_MASKED (1 << 2)
#define POLY_FLAG_SEMI_TRANS (1 << 3)
#define POLY_FLAG_ALPHA (1 << 4)
#define POLY_FLAG_TILED (1 << 5)
#define POLY_FLAG_DOUBLESIDED (1 << 6)
#define POLY_FLAG_WALKABLE (1 << 7)

//---------------------------------------------------------------

// Game States.
#define GS_ATTRACT_MODE (1 << 0)
#define GS_PLAY_GAME (1 << 1)
#define GS_CONFIGURE_NET (1 << 2)
#define GS_LEVEL_WON (1 << 3)
#define GS_LEVEL_LOST (1 << 4)
#define GS_GAME_WON (1 << 5)
#define GS_RECORD (1 << 6)
#define GS_PLAYBACK (1 << 7)
#define GS_REPLAY (1 << 8)

#define GS_EDITOR (1 << 16)

// Game Flags.
#define GF_INDOORS (1 << 0)
#define GF_SEWERS (1 << 1)
#define GF_PAUSED (1 << 2)
#define GF_RAINING (1 << 3)
#define GF_WINDY (1 << 4)
#define GF_SHOW_MAP (1 << 5)
#define GF_SHOW_CRIMERATE (1 << 6)
#define GF_PLAYER_FIRED_GUN (1 << 7)
#define GF_CONE_PENALTIES (1 << 8)
#define GF_CARS_WITH_ROAD_PRIMS (1 << 9)
#define GF_SIDE_ON_COMBAT (1 << 10)
#define GF_NO_FLOOR (1 << 11)
#define GF_DISABLE_BENCH_HEALTH (1 << 12)

#define GAME_SEASON_SPRING 0
#define GAME_SEASON_SUMMER 1
#define GAME_SEASON_AUTUMN 2
#define GAME_SEASON_WINTER 3

#define GAME_TIME_DAWN 5
#define GAME_TIME_DUSK 20

#define DETAIL_DIRT1 (1 << 0)
#define DETAIL_DIRT2 (1 << 1)
#define DETAIL_PUDDLE (1 << 2)
#define DETAIL_REFLECTIONS (1 << 3)
#define DETAIL_SHADOWS (1 << 4)
#define DETAIL_SPLASH (1 << 5)
#define DETAIL_MIST (1 << 6)
#define DETAIL_RAIN (1 << 7)

// Game structure.
struct ENGINE_Col {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
};

struct Game {
    std::int32_t GameState;
    std::int32_t GameTurn;
    std::int32_t GameFlags;

    std::int32_t CameraCount;
    std::int32_t PersonCount;
    std::int32_t PlayerCount;
    std::int32_t AnimalCount;
    std::int32_t ChopperCount;
    std::int32_t PyroCount;
    std::int32_t ProjectileCount;
    std::int32_t SpecialCount;
    std::int32_t SwitchCount;
    std::int32_t BatCount;

    std::int32_t DrawTweenCount;
    std::int32_t DrawMeshCount;
    std::int32_t PrimaryThingCount;
    std::int32_t SecondaryThingCount;
    std::int32_t TickTock;
    std::int32_t TickRatio;
    std::int32_t TickInvRatio;
    std::int32_t RandSeed;
    std::int32_t Time; // In 24-hours in fixed point 8...
    std::int32_t Season;

    // Map members.
#if defined(PSX) || defined(TARGET_DC)
    MapElement Map[1];
#else
    MapElement Map[MAP_SIZE];
#endif

    // Thing members.
    Vehicle* Vehicles;       // [MAX_VEHICLES];
    Furniture* Furnitures;   // [MAX_FURNITURE];
    Person* People;          // [MAX_PEOPLE];
    Animal* Animals;         // [MAX_ANIMALS];
    Chopper* Choppers;       // [MAX_CHOPPERS];
    Pyro* Pyros;             // [MAX_PYROS];
    Player* Players;         // [MAX_PLAYERS];
    Projectile* Projectiles; // [MAX_PROJECTILES];
    Special* Specials;       // [MAX_SPECIALS];
    Switch* Switches;        // [MAX_SWITCHES];
    Bat* Bats;               // [BAT_MAX_BATS];
    Thing* Things;           // [MAX_THINGS];

    //
    // The gourad palette
    //
    ENGINE_Col GamePal[256];

    // draw types
    DrawTween* DrawTweens; // [MAX_DRAW_TWEENS];
    DrawMesh* DrawMeshes;  // [MAX_DRAW_MESHES];

    THING_INDEX UsedPrimaryThings, UnusedPrimaryThings, UsedSecondaryThings, UnusedSecondaryThings;

    Thing** net_persons; // [10];
    Thing** net_players; // [10];

    std::int32_t indoors_height_floor; // The heights of the storey you are in.
    std::int32_t indoors_height_ceiling;
    std::int32_t indoors_dbuilding; // The index of the FBuilding you are in.
    std::uint16_t indoors_index_fade;
    std::uint16_t indoors_index_fade_ext;
    std::uint16_t indoors_index_fade_ext_dir;
    std::uint16_t indoors_index_next;
    std::uint16_t indoors_index;
    std::uint16_t indoors_room;
    std::uint16_t indoors_room_next;
    std::int32_t NumberPlayers;
    std::uint32_t Packets[16];
    std::int32_t Scores[16];
    std::int32_t MyID;
    std::int32_t UserInterface;
    std::uint16_t DetailLevel;
    std::uint16_t TextureSet;
    std::int32_t CrimeRate;
    std::int32_t CrimeRateScoreMul;
    std::uint8_t FakeCivs;
    std::uint8_t SaveValid;
    std::uint8_t MusicWorld;
    std::uint8_t BoredomRate;

    std::uint8_t DarciStrength;
    std::uint8_t DarciConstitution;
    std::uint8_t DarciSkill;
    std::uint8_t DarciStamina;
    std::uint8_t RoperStrength;
    std::uint8_t RoperConstitution;
    std::uint8_t RoperSkill;
    std::uint8_t RoperStamina;

    std::uint8_t DarciDeadCivWarnings;
    std::uint8_t padding[2];
};

extern Game the_game;

extern std::uint8_t VIOLENCE;

// Defines for 'Game' member access.

//
// Multiplayer stuff
//

#define USER_INTERFACE the_game.UserInterface
#define NO_PLAYERS CNET_num_players
#define PACKET_DATA(x) the_game.Packets[x]
#define PLAYER_ID CNET_player_id
#define GAME_SCORE(x) the_game.Scores[x]
#define MY_SCORE GAME_SCORE(PLAYER_ID)

#define ENGINE_palette the_game.GamePal
#define DETAIL_LEVEL the_game.DetailLevel

#define TICK_SHIFT (8)
#define NORMAL_TICK_TOCK (1000 / 15)
#define TICK_TOCK (the_game.TickTock)
#define TICK_RATIO (the_game.TickRatio)
#define TICK_INV_RATIO (the_game.TickInvRatio)
#define GAME_STATE (the_game.GameState)
#define GAME_TURN the_game.GameTurn
#define GAME_FLAGS (the_game.GameFlags)

#define GAME_TIME (the_game.GameTime) // In 24-hours in fixed point 8...
#define GAME_SEASON (the_game.GameSeason)

#define CRIME_RATE (the_game.CrimeRate)
#define CRIME_RATE_SCORE_MUL (the_game.CrimeRateScoreMul)
#define MUSIC_WORLD (the_game.MusicWorld)
#define BOREDOM_RATE (the_game.BoredomRate)

#define TEXTURE_SET (the_game.TextureSet)
#define FAKE_CIVS (the_game.FakeCivs)
#define SAVE_VALID (the_game.SaveValid)

#define RAND_SEED (the_game.RandSeed)

#define DRAW_TWEEN_COUNT (the_game.DrawTweenCount)
#define DRAW_MESH_COUNT (the_game.DrawMeshCount)

#define PERSON_COUNT (the_game.PersonCount)
#define ANIMAL_COUNT (the_game.AnimalCount)
#define CHOPPER_COUNT (the_game.ChopperCount)
#define PYRO_COUNT (the_game.PyroCount)
#define PLAYER_COUNT (the_game.PlayerCount)
#define PROJECTILE_COUNT (the_game.ProjectileCount)
#define SPECIAL_COUNT (the_game.SpecialCount)
#define SWITCH_COUNT (the_game.SwitchCount)
#define BAT_COUNT (the_game.BatCount)
#define PRIMARY_COUNT (the_game.PrimaryThingCount)
#define SECONDARY_COUNT (the_game.SecondaryThingCount)

#define NET_PERSON(i) (the_game.net_persons[i])
#define NET_PLAYER(i) (the_game.net_players[i])

#ifdef TARGET_DC
// Don't use these!
#define MAP (ASSERT(false), the_game.Map)
#define MAP2(x, y) (ASSERT(false), the_game.Map[(y) + ((x) * MAP_WIDTH)])
#define MAP_WHO(i) (ASSERT(false), MAP[i].MapWho)
#else
#define MAP (the_game.Map)
#define MAP2(x, y) (the_game.Map[(y) + ((x) * MAP_WIDTH)])
#define MAP_WHO(i) (MAP[i].MapWho)
#endif

#define PRIMARY_USED (the_game.UsedPrimaryThings)
#define PRIMARY_UNUSED (the_game.UnusedPrimaryThings)
#define SECONDARY_USED (the_game.UsedSecondaryThings)
#define SECONDARY_UNUSED (the_game.UnusedSecondaryThings)

#define DRAW_TWEENS (the_game.DrawTweens)
#define DRAW_MESHES (the_game.DrawMeshes)

#define NETPERSON (the_game.net_persons)
#define NETPLAYERS (the_game.net_players)
#define FURNITURE (the_game.Furnitures)
#define PEOPLE (the_game.People)
#define ANIMALS (the_game.Animals)
#define CHOPPERS (the_game.Choppers)
#define PYROS (the_game.Pyros)
#define PLAYERS (the_game.Players)
#define PROJECTILES (the_game.Projectiles)
#define SPECIALS (the_game.Specials)
#define SWITCHES (the_game.Switches)
#define THINGS (the_game.Things)
#define VEHICLES (the_game.Vehicles)
#define BATS (the_game.Bats)

#ifdef TARGET_DC

#define INDOORS_HEIGHT_FLOOR (the_game.indoors_height_floor)
#define INDOORS_HEIGHT_CEILING (the_game.indoors_height_ceiling)
#define INDOORS_DBUILDING (the_game.indoors_dbuilding)
#define INDOORS_INDEX_FADE (the_game.indoors_index_fade)
#define INDOORS_INDEX_FADE_EXT (the_game.indoors_index_fade_ext)
#define INDOORS_INDEX_FADE_EXT_DIR (the_game.indoors_index_fade_ext_dir)
#define INDOORS_ROOM_NEXT (0)
#define INDOORS_INDEX_NEXT (0)
#define INDOORS_INDEX (0)
#define INDOORS_ROOM (0)

#else

#define INDOORS_HEIGHT_FLOOR (the_game.indoors_height_floor)
#define INDOORS_HEIGHT_CEILING (the_game.indoors_height_ceiling)
#define INDOORS_DBUILDING (the_game.indoors_dbuilding)
#define INDOORS_INDEX_FADE (the_game.indoors_index_fade)
#define INDOORS_INDEX_FADE_EXT (the_game.indoors_index_fade_ext)
#define INDOORS_INDEX_FADE_EXT_DIR (the_game.indoors_index_fade_ext_dir)
#define INDOORS_ROOM_NEXT (the_game.indoors_room_next)
#define INDOORS_INDEX_NEXT (the_game.indoors_index_next)
#define INDOORS_INDEX (the_game.indoors_index)
#define INDOORS_ROOM (the_game.indoors_room)

#endif

#define TO_DRAW_TWEEN(t) (&DRAW_TWEENS[t])
#define TO_DRAW_MESH(t) (&DRAW_MESHES[t])

#define TO_VEHICLE(t) (&VEHICLES[t])
#define TO_FURNITURE(t) (&FURNITURE[t])
#define TO_PERSON(t) (&PEOPLE[t])
#define TO_ANIMAL(t) (&ANIMALS[t])
#define TO_CHOPPER(t) (&CHOPPERS[t])
#define TO_PYRO(t) (&PYROS[t])
#define TO_PLAYER(t) (&PLAYERS[t])
#define TO_PROJECTILE(t) (&PROJECTILES[t])
#define TO_SPECIAL(t) (&SPECIALS[t])
#define TO_SWITCH(t) (&SWITCHES[t])
#define TO_THING(t) (&THINGS[t])
#define TO_BAT(t) (&BATS[t])

#define ANIMAL_NUMBERb(t) (COMMON_INDEX)(t - TO_ANIMAL(0))
#define CHOPPER_NUMBER(t) (COMMON_INDEX)(t - TO_CHOPPER(0))
#define DRAW_TWEEN_NUMBER(t) (COMMON_INDEX)(t - TO_DRAW_TWEEN(0))
#define DRAW_MESH_NUMBER(t) (COMMON_INDEX)(t - TO_DRAW_MESH(0))
#define VEHICLE_NUMBER(t) (COMMON_INDEX)(t - TO_VEHICLE(0))
#define FURNITURE_NUMBER(t) (COMMON_INDEX)(t - TO_FURNITURE(0))
#define PERSON_NUMBER(t) (COMMON_INDEX)(t - TO_PERSON(0))
#define PLAYER_NUMBER(t) (COMMON_INDEX)(t - TO_PLAYER(0))
#define PROJECTILE_NUMBER(t) (COMMON_INDEX)(t - TO_PROJECTILE(0))
#define SPECIAL_NUMBER(t) (COMMON_INDEX)(t - TO_SPECIAL(0))
#define SWITCH_NUMBER(t) (COMMON_INDEX)(t - TO_SWITCH(0))
#define PYRO_NUMBER(t) (COMMON_INDEX)(t - TO_PYRO(0))
#define BAT_NUMBER(t) (COMMON_INDEX)(t - TO_BAT(0))
#define THING_NUMBER(t) (THING_INDEX)(t - TO_THING(0))

// #define	NPLAYER_NUMBER(t)		(THING_INDEX)(t-NET_PLAYER(0))
// #define	NPERSON_NUMBER(t)		(THING_INDEX)(t-NET_PERSON(0))

extern void ResetSmoothTicks();
extern std::int32_t SmoothTicks(std::int32_t raw_ticks);

//---------------------------------------------------------------

inline void SetSeed(std::uint32_t seed) {
    RAND_SEED = seed;
}

inline std::uint32_t GetSeed() {
    return RAND_SEED;
}
/*
inline std::uint16_t Random()
{
        //
        // The numbers are as recommended by Knuth.
        //

        // RAND_SEED *= 65277;
        // RAND_SEED += 13849;

        //
        // These numbers are as recommened by somebody else.
        //
        // _seed = 69069 * _seed + 12359;
        //

        //
        // This is the super-duper random number generator.
        //

        RAND_SEED *= 69069;
        RAND_SEED += 1;

        return	(RAND_SEED >> 7);
}
*/
// #define	Random()    (DebugText("random  file %s line %d %d\n",__FILE__,__LINE__,RAND_SEED),(std::uint16_t)((RAND_SEED = ((RAND_SEED*69069)+1) )>>7))
#define Random() ((std::uint16_t) ((RAND_SEED = ((RAND_SEED * 69069) + 1)) >> 7))

//---------------------------------------------------------------

//---------------------------------------------------------------

void game_startup();
void game_shutdown();
bool game_init();
void game();
void game_attract_mode();
std::uint8_t game_loop();

//---------------------------------------------------------------

std::int32_t calc_angle(std::int32_t dx, std::int32_t dz);
std::int32_t angle_diff(std::int32_t angle1, std::int32_t angle2);

// Useful.
void stop_all_fx_and_music();

//---------------------------------------------------------------

//
// where should it go though?
//

#define POLY_50MGT (POLY_FLAG_SEMI_TRANS | POLY_FLAG_MASKED | POLY_FLAG_GOURAD | POLY_FLAG_TEXTURED)
#define POLY_50MT (POLY_FLAG_SEMI_TRANS | POLY_FLAG_MASKED | POLY_FLAG_GOURAD | POLY_FLAG_TEXTURED)
#define POLY_MGT (POLY_FLAG_GOURAD | POLY_FLAG_MASKED | POLY_FLAG_TEXTURED)
#define POLY_MT (POLY_FLAG_MASKED | POLY_FLAG_TEXTURED)
#define POLY_NULL (POLY_FLAG_SEMI_TRANS | POLY_FLAG_MASKED)

#define POLY_50F (POLY_FLAG_SEMI_TRANS)
#define POLY_50GT (POLY_FLAG_SEMI_TRANS | POLY_FLAG_GOURAD | POLY_FLAG_TEXTURED)
#define POLY_50T (POLY_FLAG_SEMI_TRANS | POLY_FLAG_TEXTURED)
#define POLY_50G (POLY_FLAG_SEMI_TRANS | POLY_FLAG_GOURAD)
#define POLY_TGT (POLY_FLAG_TILED | POLY_FLAG_GOURAD | POLY_FLAG_TEXTURED)
#define POLY_GT (POLY_FLAG_GOURAD | POLY_FLAG_TEXTURED)
#define POLY_TT (POLY_FLAG_TILED | POLY_FLAG_TEXTURED)
#define POLY_T (POLY_FLAG_TEXTURED)
#define POLY_G (POLY_FLAG_GOURAD)
#define POLY_F (0)