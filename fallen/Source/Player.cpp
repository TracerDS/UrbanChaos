// Player.cpp
// Guy Simmons, 5th December 1997.

#include "Game.h"
#include "Darci.h"
#include "Roper.h"
#include "statedef.h"
#include "furn.h"
#include "interfac.h"
#include "cnet.h"

GenusFunctions player_functions[] =
    {
        {PLAYER_NONE, NULL},
        {PLAYER_DARCI, darci_states},
        {PLAYER_ROPER, roper_states}};

GameCoord player_pos;

//---------------------------------------------------------------
#ifndef PSX
void init_players() {
    memset((std::uint8_t *) PLAYERS, 0, sizeof(Player) * MAX_PLAYERS);
    PLAYER_COUNT = 0;
}
#endif
//---------------------------------------------------------------

Thing *alloc_player(std::uint8_t type) {
    std::int32_t c0;
    Player *new_player;
    Thing *player_thing = NULL;

    // Run through the player array & find an unused one.
    for (c0 = 0; c0 < MAX_PLAYERS; c0++) {
        if (PLAYERS[c0].PlayerType == PLAYER_NONE) {
            player_thing = alloc_thing(CLASS_PLAYER);
            if (player_thing) {
                new_player = TO_PLAYER(c0);
                if (new_player) {
                    player_thing->Genus.Player = new_player;
                    player_thing->StateFn = process_hardware_level_input_for_player; // Bodge for now
                    // set_state_function(player_thing,STATE_INIT);

                    new_player->PlayerType = type;
                    new_player->Thing = THING_NUMBER(player_thing);
                } else {
                    free_thing(player_thing);
                    player_thing = NULL;
                }
            }
            break;
        }
    }
    return player_thing;

    /*

                                    new_player->PlayerPerson	=   alloc_person(type);

                                    if(new_player->PlayerPerson)
                                    {
                                            player_thing->Genus.Player	=	new_player;
                                            player_thing->StateFn		=	process_hardware_level_input_for_player; // Bodge for now

                                            // Create a camera & attach it to the player.
                                            new_player->CameraThing	=	alloc_camera(CAMERA_TRACKER);
                                            if(new_player->CameraThing)
                                            {
                                                    attach_camera_to_thing(new_player->CameraThing,new_player->PlayerPerson);

                                                    //
                                                    // Make the camera start near Darci.
                                                    //

                                                    new_player->CameraThing->WorldPos.X	=	16384;
                                                    new_player->CameraThing->WorldPos.Z	=	16384;
                                            }
                                    }
                                    //set_state_function(player_thing,STATE_INIT);
                            }
                            break;
                    }
            }
            return	player_thing->Genus.Player->PlayerPerson;
    */
}

//---------------------------------------------------------------

void free_player(Thing *player_thing) {
    // Set the player type to none & free the thing.
    player_thing->Genus.Player->PlayerType = PLAYER_NONE;
    remove_thing_from_map(player_thing);
    free_thing(player_thing);
}

//---------------------------------------------------------------
extern void set_up_camera(Thing *t_camera, GameCoord *start_pos, Thing *track_thing);
// Temporary create player.
Thing *create_player(std::uint8_t type, MAPCO16 x, MAPCO16 y, MAPCO16 z, std::int32_t player_id) {
    std::uint8_t person_type;
    std::int32_t person_index;
    Thing *person_thing = NULL,
          *player_thing = NULL;

    // Try to allocate a player.
    player_thing = alloc_player(type);
    if (player_thing) {
        // Got one, now create the players 'person'.

        switch (type) {
            case PLAYER_NONE: return NULL;
            case PLAYER_DARCI: person_type = PERSON_DARCI; break;
            case PLAYER_ROPER: person_type = PERSON_ROPER; break;
            case PLAYER_COP: person_type = PERSON_COP; break;
            case PLAYER_THUG: person_type = PERSON_THUG_RASTA; break;
        }

        person_index = create_person(
            person_type,
            0,
            x << 8,
            y << 8,
            z << 8);

        if (person_index) {
            person_thing = TO_THING(person_index);

            NET_PLAYER(player_id) = player_thing;

            // Now set up the players 'person'.

            player_thing->Genus.Player->PlayerPerson = person_thing;
            player_thing->Genus.Player->PlayerID = player_id;

            GAME_SCORE(player_id) = 0;

            person_thing->Genus.Person->PlayerID = player_id + 1;
        } else {
            free_player(player_thing);
            player_thing = NULL;
        }
    }

    // return	player_thing;	// This is what this function should return.
    return person_thing; // However Mike's got his hands on the system & the player thing that
                         // calls this function will only work if the person thing is returned.
                         // Why do I bother? :-)
}

/*
Thing *create_player_car(std::uint8_t type,std::int32_t x,std::int32_t y,std::int32_t z)
{
        std::uint8_t		person_type;
        Thing		*camera_thing,
                                *car_thing	=	NULL,
                                *player_thing	=	NULL;

//	camera_thing	=	alloc_camera(CAMERA_TRACKER);

        // Try to allocate a player.
        player_thing	=	alloc_player(type);
        if(player_thing)
        {
                THING_INDEX	car;
                // Got one, now create the players 'person'.
                car=FURN_create(x<<8,y<<8,z<<8,0,0, 0,45);


                car_thing	=	TO_THING(car);
                if(car_thing)
                {
                        // Now set up the players 'person'.
                        player_thing->Genus.Player->PlayerPerson	=	car_thing;

                        set_state_function(car_thing, STATE_FDRIVING);

//			set_up_camera(camera_thing,&TO_THING(car)->WorldPos,TO_THING(car));
                        car_thing->Genus.Furniture=FURN_alloc_furniture();

                }
                else
                {
                        free_player(player_thing);
                        car_thing	=	NULL;
                }
        }

        // return	player_thing;	// This is what this function should return.
        return	car_thing;		// However Mike's got his hands on the system & the player thing that
                                                                // calls this function will only work if the person thing is returned.
                                                                // Why do I bother? :-)
}
*/
//---------------------------------------------------------------

void store_player_pos() {
    /*
    player_pos.X	=	the_def->X<<8;
    player_pos.Y	=	the_def->Y<<8;
    player_pos.Z	=	the_def->Z<<8;
    */
}

//---------------------------------------------------------------

//
// The player uses this to decide weather to auto sneak
//

//
// If people arround you, but can't see you then sneak
//
#ifdef UNUSED
extern THING_INDEX col_with_things[];
#define MAX_COL_WITH 16

std::int32_t should_i_sneak(Thing *p_person) {
    std::int32_t col_with_upto;
    std::int32_t i;
    std::int32_t found_people = 0;
    Thing *col_thing;

    col_with_upto = THING_find_sphere(
        p_person->WorldPos.X >> 8,
        p_person->WorldPos.Y >> 8,
        p_person->WorldPos.Z >> 8,
        256 * 7, // eight block radius
        col_with_things,
        MAX_COL_WITH,
        1 << CLASS_PERSON);

    for (i = 0; i < col_with_upto; i++) {
        col_thing = TO_THING(col_with_things[i]);

        if (col_thing->State == STATE_DEAD ||
            col_thing->State == STATE_DYING) {
            //
            // Dead or dying things are of no interest
            //

            continue;
        }

        switch (col_thing->Class) {
            case CLASS_PERSON:

                if (col_thing != p_person) {
                    found_people++;

                    if (can_a_see_b(col_thing, p_person)) {
                        //
                        // this person can see me so why bother to sneak
                        //
                        return (0);
                    }
                }

                break;
        }
    }

    if (found_people) {
        //
        // found people and none of them could see me, so sneak might be a good idea so we arent heard either
        //
        return (1);
    } else {
        //
        // no people so why sneak?
        //
        return (0);
    }
}
#endif

void PLAYER_redmark(std::int32_t playerid, std::int32_t dredmarks) {
#ifndef PSX
    std::int32_t redmarks;
    Thing *p_player;

    if (NO_PLAYERS != 1) {
        //
        // No red marks in multiplayer?
        //

        return;
    }

    p_player = NET_PLAYER(playerid);

    ASSERT(playerid == 0); // player should only equal zero these days
    ASSERT(p_player);
    ASSERT(p_player->Class == CLASS_PLAYER);

    redmarks = p_player->Genus.Player->RedMarks;
    redmarks += dredmarks;

    SATURATE(redmarks, 0, 10);

    p_player->Genus.Player->RedMarks = redmarks;

    if (redmarks == 10) {
        //
        // Too many red marks! Game over.
        //

        GAME_STATE = GS_LEVEL_LOST;
    }
#endif
}
