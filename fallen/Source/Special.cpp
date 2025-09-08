//	Special.cpp
//	Guy Simmons, 28th March 1998.
//
//
//
// WILLIAM SHAKESPEARE (1564-1616)
//
// Shall I compare thee to a summer's day?
// Thou art more lovely and more temperate.
// Rough winds do shake the darling buds of May,
// And summer's lease hath all too short a date.
// Sometime too hot the eye of heaven shines,
// And often is his gold complexion dimm'd;
// And every fair from fair sometime declines,
// By chance or nature's changing course untrimm'd;
// But thy eternal summer shall not fade
// Nor lose possession of that fair thou ow'st;
// Nor shall Death brag thou wander'st in his shade,
// When in eternal lines to time thou grow'st:
// So long as men can breathe or eyes can see,
// So long lives this, and this gives life to thee.
//

#include "Game.h"
#include "StateDef.h"
#include "Special.h"
#include "eway.h"
#include "pcom.h"
#include "night.h"
#include "dirt.h"
#include "animate.h"
#include "cnet.h"
#include "pow.h"
#include "memory.h"
#include "xlat_str.h"
#include "sound.h"
#include "grenade.h"
#ifndef PSX
#include "..\ddengine\headers\panel.h"
#else
#include "..\psxeng\headers\panel.h"
#endif

extern void add_damage_text(std::int16_t x, std::int16_t y, std::int16_t z, char* text);

SPECIAL_Info SPECIAL_info[SPECIAL_NUM_TYPES] =
    {
        {"None",           0,                          0                             },
        {"Key",            PRIM_OBJ_ITEM_KEY,          SPECIAL_GROUP_USEFUL          },
        {"Gun",            PRIM_OBJ_ITEM_GUN,          SPECIAL_GROUP_ONEHANDED_WEAPON},
        {"Health",         PRIM_OBJ_ITEM_HEALTH,       SPECIAL_GROUP_COOKIE          },
        {"Bomb",           PRIM_OBJ_THERMODROID,       SPECIAL_GROUP_STRANGE         },
        {"Shotgun",        PRIM_OBJ_ITEM_SHOTGUN,      SPECIAL_GROUP_TWOHANDED_WEAPON},
        {"Knife",          PRIM_OBJ_ITEM_KNIFE,        SPECIAL_GROUP_ONEHANDED_WEAPON},
        {"Explosives",     PRIM_OBJ_ITEM_EXPLOSIVES,   SPECIAL_GROUP_USEFUL          },
        {"Grenade",        PRIM_OBJ_ITEM_GRENADE,      SPECIAL_GROUP_ONEHANDED_WEAPON},
        {"Ak47",           PRIM_OBJ_ITEM_AK47,         SPECIAL_GROUP_TWOHANDED_WEAPON},
        {"Mine",           PRIM_OBJ_MINE,              SPECIAL_GROUP_STRANGE         },
        {"Thermodroid",    PRIM_OBJ_THERMODROID,       SPECIAL_GROUP_STRANGE         },
        {"Baseballbat",    PRIM_OBJ_ITEM_BASEBALLBAT,  SPECIAL_GROUP_TWOHANDED_WEAPON},
        {"Ammo pistol",    PRIM_OBJ_ITEM_AMMO_PISTOL,  SPECIAL_GROUP_AMMO            },
        {"Ammo shotgun",   PRIM_OBJ_ITEM_AMMO_SHOTGUN, SPECIAL_GROUP_AMMO            },
        {"Ammo AK47",      PRIM_OBJ_ITEM_AMMO_AK47,    SPECIAL_GROUP_AMMO            },
        {"Keycard",        PRIM_OBJ_ITEM_KEYCARD,      SPECIAL_GROUP_USEFUL          },
        {"File",           PRIM_OBJ_ITEM_FILE,         SPECIAL_GROUP_USEFUL          },
        {"Floppy_disk",    PRIM_OBJ_ITEM_FLOPPY_DISK,  SPECIAL_GROUP_USEFUL          },
        {"Crowbar",        PRIM_OBJ_ITEM_CROWBAR,      SPECIAL_GROUP_USEFUL          },
        {"Video",          PRIM_OBJ_ITEM_VIDEO,        SPECIAL_GROUP_USEFUL          },
        {"Gloves",         PRIM_OBJ_ITEM_GLOVES,       SPECIAL_GROUP_USEFUL          },
        {"WeedAway",       PRIM_OBJ_ITEM_WEEDKILLER,   SPECIAL_GROUP_USEFUL          },
        {"Badge",          PRIM_OBJ_ITEM_TREASURE,     SPECIAL_GROUP_COOKIE          },
        {"Red Car Keys",   PRIM_OBJ_ITEM_KEY,          SPECIAL_GROUP_USEFUL          },
        {"Blue Car Keys",  PRIM_OBJ_ITEM_KEY,          SPECIAL_GROUP_USEFUL          },
        {"Green Car Keys", PRIM_OBJ_ITEM_KEY,          SPECIAL_GROUP_USEFUL          },
        {"Black Car Keys", PRIM_OBJ_ITEM_KEY,          SPECIAL_GROUP_USEFUL          },
        {"White Car Keys", PRIM_OBJ_ITEM_KEY,          SPECIAL_GROUP_USEFUL          },
        {"Wire Cutters",   PRIM_OBJ_ITEM_WRENCH,       SPECIAL_GROUP_USEFUL          },
};

void free_special(Entity* s_thing);

//
// Adds/removes the special from a person's
//

void special_pickup(Entity* p_special, Entity* p_person) {
    ASSERT(p_special->Class == CLASS_SPECIAL);
    ASSERT(p_person->Class == CLASS_PERSON);

    ASSERT(p_special->Genus.Special->SpecialType != SPECIAL_MINE);

    p_special->Genus.Special->OwnerThing = THING_NUMBER(p_person);
    p_special->Genus.Special->NextSpecial = p_person->Genus.Person->SpecialList;
    p_person->Genus.Person->SpecialList = THING_NUMBER(p_special);

    if (p_special->SubState == SPECIAL_SUBSTATE_PROJECTILE) {
        p_special->SubState = SPECIAL_SUBSTATE_NONE;
    }
}

void special_drop(Entity* p_special, Entity* p_person) {
#ifndef NDEBUG
    std::int32_t count = 0;
#endif

    ASSERT(p_special->Class == CLASS_SPECIAL);
    ASSERT(p_person->Class == CLASS_PERSON);
    ASSERT(p_special->Genus.Special->SpecialType != SPECIAL_MINE);

    std::uint16_t next = p_person->Genus.Person->SpecialList;
    std::uint16_t* prev = &p_person->Genus.Person->SpecialList;

    while (1) {
#ifndef NDEBUG
        ASSERT(count++ < 100);
#endif

        if (next == NULL) {
            //
            // We haven't found the special... is this an error? I guess so.
            //

            //			ASSERT(0);

            return;
        }

        Entity* p_next = TO_THING(next);
        ASSERT(p_next->Class == CLASS_SPECIAL);

        if (next == THING_NUMBER(p_special)) {
            //
            // This is the special we want to drop. Take it out the the linked list.
            //

            *prev = p_special->Genus.Special->NextSpecial;
            p_special->Genus.Special->NextSpecial = NULL;
            p_special->Genus.Special->OwnerThing = NULL;

            {
                std::int32_t gx;
                std::int32_t gy;
                std::int32_t gz;

                extern void find_nice_place_near_person( // In person.cpp
                    Entity * p_person,
                    std::int32_t* nice_x, // 8-bits per mapsquare
                    std::int32_t* nice_y,
                    std::int32_t* nice_z);

                find_nice_place_near_person(
                    p_person,
                    &gx,
                    &gy,
                    &gz);

                p_special->WorldPos.X = gx << 8;
                p_special->WorldPos.Y = gy << 8;
                p_special->WorldPos.Z = gz << 8;
            }

            //
            // Put it on the map.
            //

            add_thing_to_map(p_special);

            if (p_person->Genus.Person->PlayerID == 0) {
                //
                // Don't drop a gun with max ammo.
                //

                switch (p_special->Genus.Special->SpecialType) {
                case SPECIAL_SHOTGUN:
                    p_special->Genus.Special->ammo = (Random() & 0x1) + 2;
                    break;

                case SPECIAL_AK47:
                    p_special->Genus.Special->ammo = (Random() & 0x7) + 6;
                    break;
                }
            }
            p_special->Velocity = 5;               // can't pick it up for 5 gameturns!
            p_special->Genus.Special->counter = 0; // Can't pick it up for one second.
            p_special->SubState = SPECIAL_SUBSTATE_NONE;

            if (next == p_person->Genus.Person->SpecialUse) {
                p_person->Genus.Person->SpecialUse = NULL;
            }

            if (next == p_person->Genus.Person->SpecialDraw) {
                p_person->Genus.Person->SpecialDraw = NULL;
            }

            return;
        }

        prev = &p_next->Genus.Special->NextSpecial;
        next = p_next->Genus.Special->NextSpecial;
    }
}

//
// Returns true if the person is carrying a two-handed weapon.
//

std::int32_t person_has_twohanded_weapon(Entity* p_person) {
    return (person_has_special(p_person, SPECIAL_SHOTGUN) ||
            person_has_special(p_person, SPECIAL_AK47) ||
            person_has_special(p_person, SPECIAL_BASEBALLBAT));
}

//
// Should this person pick up the item?
//

std::int32_t should_person_get_item(Entity* p_person, Entity* p_special) {
    Entity* p_grenade;
    Entity* p_explosives;
    Entity* p_has;

    /*
    if(p_special->Velocity)
            return(0);
    */

    if (p_person->State == STATE_MOVEING &&
        p_person->SubState == SUB_STATE_RUNNING_SKID_STOP) {
        return false;
    }

    switch (p_special->Genus.Special->SpecialType) {
    case SPECIAL_GUN:

        if (p_person->Flags & FLAGS_HAS_GUN) {
            // return p_person->Genus.Person->Ammo < SPECIAL_AMMO_IN_A_PISTOL;		// If less than 15 rounds of ammo
            return (p_person->Genus.Person->ammo_packs_pistol < 255 - SPECIAL_AMMO_IN_A_PISTOL);
        } else {
            //
            // Always pick up a gun if you haven't got one.
            //

            return true;
        }

    case SPECIAL_HEALTH:
        return (p_person->Genus.Person->Health < health[p_person->Genus.Person->PersonType]); // If not at maximum health

    case SPECIAL_BOMB:
        return false;

    case SPECIAL_AK47:

        p_has = person_has_special(p_person, SPECIAL_AK47);

        return (!p_has || p_person->Genus.Person->ammo_packs_ak47 < 255 - SPECIAL_AMMO_IN_A_AK47);

    case SPECIAL_BASEBALLBAT:

        //
        // We can only carry one two-handed weapon at once.
        //

        return !person_has_special(p_person, SPECIAL_BASEBALLBAT);

    case SPECIAL_SHOTGUN:

        p_has = person_has_special(p_person, SPECIAL_SHOTGUN);

        return (!p_has || p_person->Genus.Person->ammo_packs_shotgun < 255 - SPECIAL_AMMO_IN_A_SHOTGUN);

    case SPECIAL_KNIFE:

        //
        // We can only have one knife.
        //

        return !person_has_special(p_person, SPECIAL_KNIFE);

    case SPECIAL_EXPLOSIVES:

        p_explosives = person_has_special(p_person, SPECIAL_EXPLOSIVES);

        //
        // You can only carry 4 explosives.
        //

        return (p_explosives == NULL || p_explosives->Genus.Special->ammo < 4);

    case SPECIAL_GRENADE:

        p_grenade = person_has_special(p_person, SPECIAL_GRENADE);

        //
        // You can only carry 8 grenades.
        //

        return (p_grenade == NULL || p_grenade->Genus.Special->ammo < 8);

        //
        // Only pickup ammo when you have that weapon.
        //

    case SPECIAL_AMMO_SHOTGUN:
        return (p_person->Genus.Person->ammo_packs_shotgun < 255 - SPECIAL_AMMO_IN_A_SHOTGUN);
        return (std::int32_t) person_has_special(p_person, SPECIAL_SHOTGUN);
    case SPECIAL_AMMO_AK47:
        return (p_person->Genus.Person->ammo_packs_ak47 < 255 - SPECIAL_AMMO_IN_A_AK47);
        return (std::int32_t) person_has_special(p_person, SPECIAL_AK47);
    case SPECIAL_AMMO_PISTOL:
        return (p_person->Genus.Person->ammo_packs_pistol < 255 - SPECIAL_AMMO_IN_A_PISTOL);
        return p_person->Flags & FLAGS_HAS_GUN;

    case SPECIAL_MINE:
        return (false); // added my MD

    default:
        return true;
    }
}

//
// The player person has picked up the given item.
//

void person_get_item(Entity* p_person, Entity* p_special) {
    Entity* p_gun;

    std::int32_t keep = false;
    std::int32_t x_message = 0;
    std::int32_t overflow = 0; // ammo left over

    //	PANEL_new_text(0,4000," PICKUP special %d \n",p_special->Genus.Special->SpecialType);

    switch (p_special->Genus.Special->SpecialType) {
    case SPECIAL_GUN:

        if (p_person->Flags & FLAGS_HAS_GUN) {
            x_message = X_AMMO;

            overflow = (std::int16_t) p_person->Genus.Person->Ammo + p_special->Genus.Special->ammo;
            while (overflow > 15) {
                p_person->Genus.Person->ammo_packs_pistol += 15;
                overflow -= 15;
            }
            p_person->Genus.Person->Ammo = overflow;

            /*				if ((std::int16_t)p_person->Genus.Person->Ammo + p_special->Genus.Special->ammo<256)
                                                    p_person->Genus.Person->Ammo += p_special->Genus.Special->ammo;
                                            else
                                                    p_person->Genus.Person->Ammo = 255;*/
        } else {
            x_message = X_GUN;

            p_person->Genus.Person->Ammo = p_special->Genus.Special->ammo;
            p_person->Flags |= FLAGS_HAS_GUN;
        }

        break;

    case SPECIAL_HEALTH:

        x_message = X_HEALTH;

        p_person->Genus.Person->Health += 100;

        if (p_person->Genus.Person->Health > health[p_person->Genus.Person->PersonType]) {
            p_person->Genus.Person->Health = health[p_person->Genus.Person->PersonType];
            //				p_person->Genus.Person->Health = 200;
        }

        break;

    case SPECIAL_AMMO_SHOTGUN:
        p_person->Genus.Person->ammo_packs_shotgun += SPECIAL_AMMO_IN_A_SHOTGUN;

        x_message = X_AMMO;

        break;

    case SPECIAL_AMMO_AK47:

        p_person->Genus.Person->ammo_packs_ak47 += SPECIAL_AMMO_IN_A_AK47;

        x_message = X_AMMO;

        break;

    case SPECIAL_AMMO_PISTOL:

        p_person->Genus.Person->ammo_packs_pistol += SPECIAL_AMMO_IN_A_PISTOL;

        x_message = X_AMMO;

        break;

    case SPECIAL_MINE:

        //
        // You can't pick up mines any more.
        //

        ASSERT(0);

        /*

        ASSERT(p_special->SubState == SPECIAL_SUBSTATE_ACTIVATED);

        //
        // When the special comes to draw itself, it will draw itself at that
        // person's hand.  We don't remove it from the map or free up the special.
        //

        special_pickup(p_special, p_person);

        keep = true;

        */

        break;

    case SPECIAL_SHOTGUN:

    {
        Entity* p_gun = person_has_special(p_person, SPECIAL_SHOTGUN);

        if (p_gun) {
            /*					p_has->Genus.Special->ammo += p_special->Genus.Special->ammo;

                                                    if (p_has->Genus.Special->ammo > SPECIAL_AMMO_IN_A_SHOTGUN)
                                                    {
                                                            p_has->Genus.Special->ammo = SPECIAL_AMMO_IN_A_SHOTGUN;
                                                    }*/
            overflow = (std::int16_t) p_gun->Genus.Special->ammo + p_special->Genus.Special->ammo;
            while (overflow > SPECIAL_AMMO_IN_A_SHOTGUN) {
                p_person->Genus.Person->ammo_packs_shotgun += SPECIAL_AMMO_IN_A_SHOTGUN;
                overflow -= SPECIAL_AMMO_IN_A_SHOTGUN;
            }
            p_gun->Genus.Special->ammo = overflow;

            x_message = X_AMMO;

            break;
        } else {
            special_pickup(p_special, p_person);

            //
            // Make sure it isn't drawn!
            //

            remove_thing_from_map(p_special);

            //
            // Dont free up the special.
            //

            keep = true;

            x_message = X_SHOTGUN;

            break;
        }
    }

    case SPECIAL_AK47:

    {
        Entity* p_gun = person_has_special(p_person, SPECIAL_AK47);

        if (p_gun) {
            /*					p_has->Genus.Special->ammo += p_special->Genus.Special->ammo;

                                                    if (p_has->Genus.Special->ammo > SPECIAL_AMMO_IN_A_AK47)
                                                    {
                                                            p_has->Genus.Special->ammo = SPECIAL_AMMO_IN_A_AK47;
                                                    }*/
            overflow = (std::int16_t) p_gun->Genus.Special->ammo + p_special->Genus.Special->ammo;
            while (overflow > SPECIAL_AMMO_IN_A_AK47) {
                p_person->Genus.Person->ammo_packs_ak47 += SPECIAL_AMMO_IN_A_AK47;
                overflow -= SPECIAL_AMMO_IN_A_AK47;
            }
            p_gun->Genus.Special->ammo = overflow;

            x_message = X_AMMO;
        } else {
            special_pickup(p_special, p_person);

            //
            // Make sure it isn't drawn!
            //

            remove_thing_from_map(p_special);

            //
            // Dont free up the special.
            //

            keep = true;

            x_message = X_AK;
        }

        break;
    }

    case SPECIAL_GRENADE:

    {
        Entity* p_has = person_has_special(p_person, SPECIAL_GRENADE);

        if (p_has) {
            if (p_has->Genus.Special->ammo < 100) // just to be on the safe side
                p_has->Genus.Special->ammo += SPECIAL_AMMO_IN_A_GRENADE;

            x_message = X_AMMO;
        } else {
            special_pickup(p_special, p_person);

            //
            // Make sure it isn't drawn!
            //

            remove_thing_from_map(p_special);

            //
            // Dont free up the special.
            //

            keep = true;

            x_message = X_GRENADE;
        }

        break;
    }

    case SPECIAL_EXPLOSIVES:

    {
        Entity* p_has = person_has_special(p_person, SPECIAL_EXPLOSIVES);

        if (p_has) {
            p_has->Genus.Special->ammo += 1;
        } else {
            special_pickup(p_special, p_person);

            //
            // Make sure it isn't drawn!
            //

            remove_thing_from_map(p_special);

            //
            // Dont free up the special.
            //

            keep = true;
        }
        if (p_special->SubState == SPECIAL_SUBSTATE_ACTIVATED) {
            // This is active lets make it inactive
            p_special->SubState = SPECIAL_SUBSTATE_NONE;
        }

        x_message = X_EXPLOSIVES;

        break;
    }
    case SPECIAL_TREASURE:

    {
        Entity* darci = NET_PERSON(0);

        //
        // Has a player picked us up?
        //

        if (!(darci->Genus.Person->Flags & FLAG_PERSON_DRIVING)) {
            switch (p_special->Draw.Mesh->ObjectId) {
            case 81:

                x_message = X_STA_INCREASED;

                NET_PLAYER(0)->Genus.Player->Stamina++;

                break;

            case 94:

                x_message = X_STR_INCREASED;

                NET_PLAYER(0)->Genus.Player->Strength++;

                break;

            case 71:

                x_message = X_REF_INCREASED;

                NET_PLAYER(0)->Genus.Player->Skill++;
                break;

            case 39:

                x_message = X_CON_INCREASED;
                NET_PLAYER(0)->Genus.Player->Constitution++;
                break;

            default:
                ASSERT(0);
                break;
            }
        }

#ifdef PSX
        PANEL_icon_time = 30;
#endif

        extern std::int32_t stat_count_bonus;
        stat_count_bonus++;
    }
        keep = false;
        break;

    default:

        if (p_special->Genus.Special->SpecialType == SPECIAL_KNIFE) {
            x_message = X_KNIFE;
        }
        if (p_special->Genus.Special->SpecialType == SPECIAL_BASEBALLBAT) {
            x_message = X_BASEBALL_BAT;
        }
        if (p_special->Genus.Special->SpecialType == SPECIAL_KEY) {
            x_message = X_KEYCARD;
        }
        if (p_special->Genus.Special->SpecialType == SPECIAL_VIDEO) {
            x_message = X_VIDEO;
        }

        //
        // Use it by default? Nope.  Add it to the person's linked list of
        // items carried.
        //

        special_pickup(p_special, p_person);

        //
        // Make sure it isn't drawn!
        //

        remove_thing_from_map(p_special);

        //
        // Dont free up the special.
        //

        keep = true;

        break;
    }

    if (x_message && p_person->Genus.Person->PlayerID) {
        /*
                        add_damage_text(
                                p_special->WorldPos.X          >> 8,
                                p_special->WorldPos.Y + 0x2000 >> 8,
                                p_special->WorldPos.Z          >> 8,
                                XLAT_str_ptr(x_message));
        */

        PANEL_new_info_message(XLAT_str(x_message));
    }

    if (p_special->Genus.Special->waypoint) {
        //
        // Tell the waypoint system.
        //

        EWAY_item_pickedup(p_special->Genus.Special->waypoint);
    }

    if (!keep) {
        //
        // Don't need the special any more.
        //

        free_special(p_special);
    }
}

void special_activate_mine(Entity* p_mine) {
    ASSERT(p_mine->Class == CLASS_SPECIAL);
    ASSERT(p_mine->Genus.Special->SpecialType == SPECIAL_MINE);

    if (NET_PERSON(0)->Genus.Person->Target == THING_NUMBER(p_mine)) {
        //
        // Make the player choose a new target.
        //

        NET_PERSON(0)->Genus.Person->Target = NULL;
    }

    /*

    PYRO_construct(
            p_mine->WorldPos,
       -1,
            0xa0);
    */

#ifdef PSX

    POW_create(
        POW_CREATE_LARGE_SEMI,
        p_mine->WorldPos.X,
        p_mine->WorldPos.Y,
        p_mine->WorldPos.Z,
        0, 0, 0);
    PYRO_create(p_mine->WorldPos, PYRO_DUSTWAVE);

#else

    PYRO_create(p_mine->WorldPos, PYRO_FIREBOMB);
    PYRO_create(p_mine->WorldPos, PYRO_DUSTWAVE);

#endif

    MFX_play_xyz(THING_NUMBER(p_mine), SOUND_Range(S_EXPLODE_START, S_EXPLODE_END), 0, p_mine->WorldPos.X, p_mine->WorldPos.Y, p_mine->WorldPos.Z);

    create_shockwave(
        p_mine->WorldPos.X >> 8,
        p_mine->WorldPos.Y >> 8,
        p_mine->WorldPos.Z >> 8,
        0x200,
        250,
        NULL);

    free_special(p_mine);
}

DIRT_Info special_di;

void special_normal(Entity* s_thing) {
    std::int32_t i;

    std::int32_t dx;
    std::int32_t dy;
    std::int32_t dz;

    std::int32_t dist;

    if (s_thing->Flags & FLAG_SPECIAL_HIDDEN) {
        //
        // Counter is the ob this specail is inside!
        //
    } else {
        s_thing->Genus.Special->counter += 16 * TICK_RATIO >> TICK_SHIFT;
    }

    if (s_thing->SubState == SPECIAL_SUBSTATE_PROJECTILE) {
        SpecialPtr ss;
        std::int32_t velocity;
        std::int32_t ground;

        ss = s_thing->Genus.Special;
        velocity = ss->timer;
        velocity -= 0x8000;
        velocity -= 256 * TICK_RATIO >> TICK_SHIFT;

        s_thing->WorldPos.Y += velocity * TICK_RATIO >> TICK_SHIFT;
        ground = PAP_calc_map_height_at(s_thing->WorldPos.X >> 8, s_thing->WorldPos.Z >> 8) + 0x30 << 8;

        if (s_thing->WorldPos.Y < ground) {
            velocity = abs(velocity);
            velocity >>= 1;
            s_thing->WorldPos.Y = ground;

            if (velocity < 0x100) {
                s_thing->SubState = SPECIAL_SUBSTATE_NONE;
                s_thing->Genus.Special->timer = 0;

                return;
            }
        }

        velocity += 0x8000;

        SATURATE(velocity, 0, 0xffff);

        ss->timer = velocity;

        return;
    } else {
        /*
        ASSERT(s_thing->Velocity>=0 &&s_thing->Velocity<10);
        if(s_thing->Velocity)
                s_thing->Velocity--;   // dropped things cant be picked up for a few gameturns.
        */
    }

    if (s_thing->Genus.Special->OwnerThing && s_thing->Genus.Special->SpecialType != SPECIAL_EXPLOSIVES) {
        Entity* p_owner = TO_THING(s_thing->Genus.Special->OwnerThing);
        ASSERT(s_thing->Genus.Special->SpecialType != SPECIAL_MINE);
        ASSERT(s_thing->Genus.Special->SpecialType != SPECIAL_NONE);

        ASSERT((s_thing->Flags & FLAGS_ON_MAPWHO) == 0);

        //
        // This special is being carried by someone- it can't be on the mapwho
        //

        s_thing->WorldPos = p_owner->WorldPos;

        if (s_thing->Genus.Special->SpecialType == SPECIAL_GRENADE &&
            s_thing->SubState == SPECIAL_SUBSTATE_ACTIVATED) {
            std::int32_t ticks = 16 * TICK_RATIO >> TICK_SHIFT;

            if (s_thing->Genus.Special->timer < ticks) {
                //
                // The grenade has gone off!
                //
#ifndef PSX
                CreateGrenadeExplosion(s_thing->WorldPos.X, s_thing->WorldPos.Y + 2256, s_thing->WorldPos.Z, p_owner);
#else
                POW_create(
                    POW_CREATE_LARGE_SEMI,
                    s_thing->WorldPos.X,
                    s_thing->WorldPos.Y,
                    s_thing->WorldPos.Z, 0, 0, 0);

                create_shockwave(
                    s_thing->WorldPos.X >> 8,
                    s_thing->WorldPos.Y >> 8,
                    s_thing->WorldPos.Z >> 8,
                    0x300,
                    500,
                    p_owner);
#endif

                p_owner->Genus.Person->SpecialUse = NULL;

                if (s_thing->Genus.Special->ammo == 1) {
                    special_drop(s_thing, p_owner);
                    free_special(s_thing);
                } else {
                    s_thing->SubState = SPECIAL_SUBSTATE_NONE; // let's reset the grenade
                    s_thing->Genus.Special->ammo -= 1;
                }
            } else {
                s_thing->Genus.Special->timer -= ticks;
            }
        }
    } else {
        //
        // This special is lying on the ground. Mostly they rotate.
        //

        if (s_thing->Genus.Special->SpecialType == SPECIAL_BOMB &&
            s_thing->SubState == SPECIAL_SUBSTATE_NONE) {
            //
            // Deactivated bombs don't rotate.
            //
        } else {
            if (s_thing->Genus.Special->SpecialType != SPECIAL_MINE)                                                              // mines no longer rotate
                if (s_thing->Genus.Special->SpecialType != SPECIAL_EXPLOSIVES || s_thing->SubState != SPECIAL_SUBSTATE_ACTIVATED) // explosives waiting to go off no longer rotate
                    s_thing->Draw.Mesh->Angle = (s_thing->Draw.Mesh->Angle + 32) & 2047;
        }

        switch (s_thing->Genus.Special->SpecialType) {
        case SPECIAL_BOMB:

            //
            // Has the waypoint that created us gone active?
            //

            if (s_thing->Genus.Special->waypoint) {
                if (s_thing->SubState == SPECIAL_SUBSTATE_ACTIVATED) {
                    if (EWAY_is_active(s_thing->Genus.Special->waypoint)) {
                        //
                        // Explode this bomb.
                        //
#ifndef PSX
                        PYRO_construct(
                            s_thing->WorldPos,
                            -1,
                            256);
#else
                        POW_create(
                            POW_CREATE_LARGE_SEMI,
                            s_thing->WorldPos.X,
                            s_thing->WorldPos.Y,
                            s_thing->WorldPos.Z, 0, 0, 0);
#endif
                        //
                        // Don't need the special any more.
                        //

                        free_special(s_thing);
                    }
                }
            }

            break;

        case SPECIAL_EXPLOSIVES:

            if (s_thing->SubState == SPECIAL_SUBSTATE_ACTIVATED) {
                std::int32_t tickdown = 0x10 * TICK_RATIO >> TICK_SHIFT;

                //
                // Time to explode?
                //

                if (s_thing->Genus.Special->timer <= tickdown) {
                    //
                    // Bang!
                    //

#ifndef PSX
                    PYRO_construct(
                        s_thing->WorldPos,
                        1 | 4 | 8 | 64,
                        0xa0);
#else
                    POW_create(
                        POW_CREATE_LARGE_SEMI,
                        s_thing->WorldPos.X,
                        s_thing->WorldPos.Y,
                        s_thing->WorldPos.Z, 0, 0, 0);
                    PYRO_construct(
                        s_thing->WorldPos, PYRO_DUSTWAVE, 0xa0);
#endif
                    MFX_play_xyz(THING_NUMBER(s_thing), SOUND_Range(S_EXPLODE_MEDIUM, S_EXPLODE_BIG), 0, s_thing->WorldPos.X, s_thing->WorldPos.Y, s_thing->WorldPos.Z);

                    {
                        std::int32_t cx, cz;
                        cx = s_thing->WorldPos.X >> 8;
                        cz = s_thing->WorldPos.Z >> 8;
                        DIRT_gust(s_thing, cx, cz, cx + 0xa0, cz);
                        DIRT_gust(s_thing, cx + 0xa0, cz, cx, cz);
                    }

                    create_shockwave(
                        s_thing->WorldPos.X >> 8,
                        s_thing->WorldPos.Y >> 8,
                        s_thing->WorldPos.Z >> 8,
                        0x500,
                        500,
                        TO_THING(s_thing->Genus.Special->OwnerThing));

                    //
                    // This is the end of the explosives.
                    //

                    free_special(s_thing);
                } else {
                    s_thing->Genus.Special->timer -= tickdown;
                }
            } else {
                if (!s_thing->Genus.Special->OwnerThing) {
                    //
                    // not being carried round may as well see if anyone wants to pick me up
                    //
                    goto try_pickup; // I love goto
                }
            }

            break;

        case SPECIAL_MINE:

            if (s_thing->Genus.Special->ammo > 0) {
                //
                // This special has a counter ticking down to explode.
                //

                if (s_thing->Genus.Special->ammo == 1) {
                    special_activate_mine(s_thing);
                } else {
                    s_thing->Genus.Special->ammo -= 1;
                }
            } else if ((GAME_TURN & 1) == (THING_NUMBER(s_thing) & 1)) {
                std::int32_t i;
                std::int32_t j;

                std::int32_t wx;
                std::int32_t wy;
                std::int32_t wz;

                std::int32_t num_found = THING_find_sphere(
                    s_thing->WorldPos.X >> 8,
                    s_thing->WorldPos.Y >> 8,
                    s_thing->WorldPos.Z >> 8,
                    0x300,
                    THING_array,
                    THING_ARRAY_SIZE,
                    (1 << CLASS_PERSON) | (1 << CLASS_VEHICLE) | (1 << CLASS_BAT));

                Entity* p_found;

                for (i = 0; i < num_found; i++) {
                    p_found = TO_THING(THING_array[i]);

                    switch (p_found->Class) {
                    case CLASS_BAT:

                        if (p_found->Genus.Bat->type == BAT_TYPE_BALROG) {
                            dx = abs(p_found->WorldPos.X - s_thing->WorldPos.X);
                            dz = abs(p_found->WorldPos.Z - s_thing->WorldPos.Z);

                            dist = QDIST2(dx, dz);

                            if (dist < 0x6000) {
                                special_activate_mine(s_thing);

                                return;
                            }
                        }

                        break;

                    case CLASS_PERSON:

                    {
                        Entity* p_person = p_found;

                        dx = abs(p_person->WorldPos.X - s_thing->WorldPos.X);
                        dz = abs(p_person->WorldPos.Z - s_thing->WorldPos.Z);

                        dist = QDIST2(dx, dz);

                        if (dist < 0x3000) {
                            std::int32_t fx;
                            std::int32_t fy;
                            std::int32_t fz;

                            calc_sub_objects_position(
                                p_person,
                                p_person->Draw.Tweened->AnimTween,
                                SUB_OBJECT_LEFT_FOOT,
                                &fx,
                                &fy,
                                &fz);

                            fx += p_person->WorldPos.X >> 8;
                            fy += p_person->WorldPos.Y >> 8;
                            fz += p_person->WorldPos.Z >> 8;

                            if (fy > (s_thing->WorldPos.Y + 0x2000 >> 8)) {
                                //
                                // This person has jumped over the mine!
                                //
                            } else {
                                //
                                // Activate the mine.
                                //

                                if (p_person->Genus.Person->pcom_ai == PCOM_AI_BODYGUARD && TO_THING(EWAY_get_person(p_person->Genus.Person->pcom_ai_other))->Genus.Person->PlayerID) {
                                    //
                                    // players bodyguards dont trigger mines
                                    //
                                } else {
                                    special_activate_mine(s_thing);

                                    return;
                                }
                            }
                        }
                    }

                    break;

                    case CLASS_VEHICLE:

                    {
                        std::int32_t wx;
                        std::int32_t wy;
                        std::int32_t wz;

                        Entity* p_vehicle = p_found;

                        //
                        // Find out where the wheels are.
                        //

                        vehicle_wheel_pos_init(p_vehicle);

                        for (j = 0; j < 4; j++) {
                            vehicle_wheel_pos_get(
                                j,
                                &wx,
                                &wy,
                                &wz);

                            dx = abs(wx - (s_thing->WorldPos.X >> 8));
                            dy = abs(wy - (s_thing->WorldPos.Y >> 8));
                            dz = abs(wz - (s_thing->WorldPos.Z >> 8));

                            dist = QDIST3(dx, dy, dz);

                            if (dist < 0x50) {
                                special_activate_mine(s_thing);

                                return;
                            }
                        }
                    }

                    break;

                    default:
                        ASSERT(0);
                        break;
                    }
                }
            }
            break;

        case SPECIAL_THERMODROID:
            break;

        case SPECIAL_TREASURE:

            for (i = 0; i < NO_PLAYERS; i++) {
                Entity* darci = NET_PERSON(i);

                //
                // Has a player picked us up?
                //

                if (!(darci->Genus.Person->Flags & FLAG_PERSON_DRIVING)) {
                    dx = darci->WorldPos.X - s_thing->WorldPos.X >> 8;
                    dy = darci->WorldPos.Y - s_thing->WorldPos.Y >> 8;
                    dz = darci->WorldPos.Z - s_thing->WorldPos.Z >> 8;

                    dist = abs(dx) + abs(dy) + abs(dz);

                    if (dist < 0xa0) {
                        std::int32_t x_message;

                        //
                        // Near enough to pick it up.
                        //

                        switch (s_thing->Draw.Mesh->ObjectId) {
                        case 81:

                            x_message = X_STA_INCREASED;

                            add_damage_text(
                                s_thing->WorldPos.X >> 8,
                                s_thing->WorldPos.Y + 0x6000 >> 8,
                                s_thing->WorldPos.Z >> 8,
                                XLAT_str(X_STA_INCREASED));

                            NET_PLAYER(0)->Genus.Player->Stamina++;

                            break;

                        case 94:

                            x_message = X_STR_INCREASED;

                            add_damage_text(
                                s_thing->WorldPos.X >> 8,
                                s_thing->WorldPos.Y + 0x6000 >> 8,
                                s_thing->WorldPos.Z >> 8,
                                XLAT_str(X_STR_INCREASED));

                            NET_PLAYER(0)->Genus.Player->Strength++;

                            break;

                        case 71:

                            x_message = X_REF_INCREASED;

                            add_damage_text(
                                s_thing->WorldPos.X >> 8,
                                s_thing->WorldPos.Y + 0x6000 >> 8,
                                s_thing->WorldPos.Z >> 8,
                                XLAT_str(X_REF_INCREASED));
                            NET_PLAYER(0)->Genus.Player->Skill++;
                            break;

                        case 39:

                            x_message = X_CON_INCREASED;

                            add_damage_text(
                                s_thing->WorldPos.X >> 8,
                                s_thing->WorldPos.Y + 0x6000 >> 8,
                                s_thing->WorldPos.Z >> 8,
                                XLAT_str(X_CON_INCREASED));
                            NET_PLAYER(0)->Genus.Player->Constitution++;
                            break;

                        default:
                            ASSERT(0);
                            break;
                        }

                        PANEL_new_info_message(XLAT_str(x_message));
#ifdef PSX
                        PANEL_icon_time = 30;
#endif

                        //						CONSOLE_text(XLAT_str(X_FUSE_SET));
                        free_special(s_thing);

                        extern std::int32_t stat_count_bonus;
                        stat_count_bonus++;

                        //						NET_PLAYER(i)->Genus.Player->Treasure += 1;
                        //						darci->Genus.Person->Health = health[darci->Genus.Person->PersonType];

                        /*
                                                                        {
                                                                                char str[64];

                                                                                sprintf(str, "Badge %d", NET_PLAYER(i)->Genus.Player->Treasure);

                                                                                CONSOLE_text(str);
                                                                        }
                        */

                        //
                        // Reduce crime rate by 10%
                        //

                        //						CRIME_RATE -= 10;

                        //						SATURATE(CRIME_RATE, 0, 100);
                    }
                }
            }

            break;
        case SPECIAL_HEALTH:

            extern std::int16_t health[];
            if (NET_PERSON(0)->Genus.Person->Health > health[NET_PERSON(0)->Genus.Person->PersonType] - 100) {
                break;
            }

        case SPECIAL_GRENADE:

            if (s_thing->Genus.Special->SpecialType == SPECIAL_GRENADE &&
                s_thing->SubState == SPECIAL_SUBSTATE_ACTIVATED) {
                //
                // Don't pickup activated grenades.
                //

                break;
            }

        case SPECIAL_AK47:
        case SPECIAL_SHOTGUN:
        case SPECIAL_GUN:
        case SPECIAL_KNIFE:
        case SPECIAL_BASEBALLBAT:
        case SPECIAL_AMMO_AK47:
        case SPECIAL_AMMO_SHOTGUN:
        case SPECIAL_AMMO_PISTOL:
        default:

        try_pickup:;
            if (s_thing->Genus.Special->counter > 16 * 20) {
                if (s_thing->Flags & FLAGS_ON_MAPWHO) {
                    Entity* darci = NET_PERSON(0);

                    //
                    // Has a player picked us up?
                    //

                    if (!(darci->Genus.Person->Flags & FLAG_PERSON_DRIVING)) {
                        dx = darci->WorldPos.X - s_thing->WorldPos.X >> 8;
                        dy = darci->WorldPos.Y - s_thing->WorldPos.Y >> 8;
                        dz = darci->WorldPos.Z - s_thing->WorldPos.Z >> 8;

                        dist = abs(dx) + abs(dy) + abs(dz);

                        if (dist < 0xa0) {
                            if (should_person_get_item(darci, s_thing)) {
                                person_get_item(darci, s_thing);
                            }
                        }
                    }
                }
            }

            break;
        }
    }
}

//---------------------------------------------------------------
#ifndef PSX
void init_specials() {
    // memset((std::uint8_t*)SPECIALS,0,sizeof(SPECIALS));

    memset((std::uint8_t*) SPECIALS, 0, sizeof(Special) * MAX_SPECIALS);
    SPECIAL_COUNT = 0;
}

#endif
//---------------------------------------------------------------

std::int32_t find_empty_special() {
    std::int32_t c0;
    for (c0 = 1; c0 < MAX_SPECIALS; c0++) {
        if (SPECIALS[c0].SpecialType == SPECIAL_NONE) {
            return (c0);
        }
    }

    return NULL;
}

Entity* alloc_special(
    std::uint8_t type,
    std::uint8_t substate,
    std::int32_t world_x,
    std::int32_t world_y,
    std::int32_t world_z,
    std::uint16_t waypoint) {
    std::int32_t c0;
    DrawMesh* dm;
    Special* new_special;
    Entity* special_thing = NULL;
    std::int32_t special_index;

    ASSERT(WITHIN(type, 1, SPECIAL_NUM_TYPES - 1));

    // Run through the special array & find an unused one.

    special_index = find_empty_special();
    dm = alloc_draw_mesh();
    special_thing = alloc_thing(CLASS_SPECIAL);

    if (!special_index || !dm || !special_thing) {
        //
        // Oh dear! Dealloc anythings we've got.
        //

        if (special_index) {
            TO_SPECIAL(special_index)->SpecialType = SPECIAL_NONE;
        }

        if (dm) {
            free_draw_mesh(dm);
        }

        if (special_thing) {
            free_thing(special_thing);
        }

        //
        // Find another special and hijack it!
        //

        std::int32_t score;
        std::int32_t best_score = -1;
        Entity* best_thing = NULL;

        std::int32_t list = thing_class_head[CLASS_SPECIAL];

        while (list) {
            Entity* p_hijack = TO_THING(list);

            ASSERT(p_hijack->Class == CLASS_SPECIAL);

            list = p_hijack->NextLink;

            //
            // Good special to hijack?
            //

            score = -1;

            if (!(p_hijack->Flags & FLAGS_ON_MAPWHO)) {
                continue;
            }

            if (p_hijack->Genus.Special->OwnerThing) {
                continue;
            }

            switch (p_hijack->Genus.Special->SpecialType) {
            case SPECIAL_KNIFE:
            case SPECIAL_BASEBALLBAT:
                score += 4000;
                break;

            case SPECIAL_AMMO_PISTOL:
            case SPECIAL_AMMO_SHOTGUN:
            case SPECIAL_AMMO_AK47:
                score += 3000;
                break;

            case SPECIAL_GUN:
            case SPECIAL_GRENADE:
            case SPECIAL_HEALTH:
            case SPECIAL_SHOTGUN:
            case SPECIAL_AK47:
                score += 2000;
                break;

            default:

                //
                // Never hijack one of these...
                //

                continue;
            }

            std::int32_t dx = abs(p_hijack->WorldPos.X - NET_PERSON(0)->WorldPos.X) >> 16;
            std::int32_t dz = abs(p_hijack->WorldPos.Z - NET_PERSON(0)->WorldPos.Z) >> 16;

            if (dx + dz > 12) {
                score += dx;
                score += dz;

                if (score > best_score) {
                    best_score = score;
                    best_thing = p_hijack;
                }
            }
        }

        if (best_thing) {
            remove_thing_from_map(best_thing);

            special_index = SPECIAL_NUMBER(best_thing->Genus.Special);
            special_thing = best_thing;
            dm = special_thing->Draw.Mesh;
        } else {
            return NULL;
        }
    }

    new_special = TO_SPECIAL(special_index);
    new_special->SpecialType = type;
    new_special->Entity = THING_NUMBER(special_thing);
    new_special->waypoint = waypoint;
    new_special->counter = 0;
    new_special->OwnerThing = NULL;

    special_thing->Genus.Special = new_special;
    special_thing->State = STATE_NORMAL;
    special_thing->SubState = substate;
    special_thing->StateFn = special_normal;

    // Create the visible object.

    special_thing->Draw.Mesh = dm;
    special_thing->DrawType = DT_MESH;
    dm->Angle = 0;
    dm->Tilt = 0;
    dm->Roll = 0;

    switch (type) {
    case SPECIAL_GUN: new_special->ammo = SPECIAL_AMMO_IN_A_PISTOL; break;
    case SPECIAL_SHOTGUN: new_special->ammo = SPECIAL_AMMO_IN_A_SHOTGUN; break;
    case SPECIAL_AK47: new_special->ammo = SPECIAL_AMMO_IN_A_AK47; break;
    case SPECIAL_GRENADE: new_special->ammo = SPECIAL_AMMO_IN_A_GRENADE; break;
    case SPECIAL_EXPLOSIVES: new_special->ammo = 1; break;

    default:
        new_special->ammo = 0;
        break;
    }

    dm->ObjectId = SPECIAL_info[type].prim;

    if (dm->ObjectId == PRIM_OBJ_ITEM_TREASURE) {
        switch (Random() & 3) {
        case 0:
            dm->ObjectId = 71;
            break;
        case 1:
            dm->ObjectId = 94;
            break;
        case 2:
            dm->ObjectId = 81;
            break;
        case 3:
            dm->ObjectId = 39;
            break;
        }
    }

    special_thing->WorldPos.X = world_x << 8;
    special_thing->WorldPos.Y = world_y << 8;
    special_thing->WorldPos.Z = world_z << 8;

    add_thing_to_map(special_thing);

    if (world_y > PAP_calc_map_height_at(world_x, world_z) + 0x50) {
        //
        // Make this item start dropping to the ground...
        //

        special_thing->SubState = SPECIAL_SUBSTATE_PROJECTILE;
        special_thing->Genus.Special->timer = 0x8000; // 0x8000 => 0! (Its a std::uint16_t)
    }

    return special_thing;
}

//---------------------------------------------------------------

void free_special(Entity* special_thing) {
    // Set the special type to none & free the thing.

    special_thing->Genus.Special->SpecialType = SPECIAL_NONE;
    free_draw_mesh(special_thing->Draw.Mesh);
    remove_thing_from_map(special_thing);
    free_thing(special_thing);
}

//---------------------------------------------------------------

Entity* person_has_special(Entity* p_person, std::int32_t special_type) {
    std::int32_t special;
    Entity* p_special;

    for (special = p_person->Genus.Person->SpecialList; special; special = p_special->Genus.Special->NextSpecial) {
        p_special = TO_THING(special);

        if (p_special->Genus.Special->SpecialType == special_type) {
            return p_special;
        }
    }

    return NULL;
}

void SPECIAL_throw_grenade(Entity* p_special) {
    Entity* p_person = TO_THING(p_special->Genus.Special->OwnerThing);

    //
    // Convert the grenade to some dirt.
    //

    if (!CreateGrenadeFromPerson(p_person, p_special->Genus.Special->timer)) {
        // no room in grenade array
        return;
    }

    if (p_special->Genus.Special->ammo == 1) {
        //
        // Get rid of our special.
        //

        special_drop(p_special, p_person);
        free_special(p_special);

        p_person->Genus.Person->SpecialUse = NULL;
    } else {
        p_special->Genus.Special->ammo -= 1;
        p_special->SubState = SPECIAL_SUBSTATE_NONE;
    }
}

void SPECIAL_prime_grenade(Entity* p_special) {
    p_special->SubState = SPECIAL_SUBSTATE_ACTIVATED;
    p_special->Genus.Special->timer = 16 * 20 * 6; // 6 second so self destruct.
}

/*

void SPECIAL_throw_mine(Entity *p_special)
{
        std::uint16_t dirt;
        std::uint16_t owner;

        ASSERT(p_special->Genus.Special->SpecialType == SPECIAL_MINE);
        ASSERT(p_special->SubState                   == SPECIAL_SUBSTATE_ACTIVATED);
        ASSERT(p_special->Genus.Special->OwnerThing);

        //
        // Remember the owner...
        //

        owner = p_special->Genus.Special->OwnerThing;

        //
        // Create some dirt that is going to process the physics of the mine
        // being thrown through the air.
        //

        dirt = DIRT_create_mine(TO_THING(p_special->Genus.Special->OwnerThing));

        //
        // Take it out of the person's special list.
        //

        special_drop(p_special, TO_THING(p_special->Genus.Special->OwnerThing));

        //
        // Link the special to the dirt that is going to process its physics.
        //

        p_special->SubState                  = SPECIAL_SUBSTATE_IS_DIRT;
        p_special->Genus.Special->waypoint   = dirt;
        p_special->Genus.Special->OwnerThing = owner;	// So we know who to blame for the explosion.
}

*/

void SPECIAL_set_explosives(Entity* p_person) {
    Entity* p_special;

    p_special = person_has_special(p_person, SPECIAL_EXPLOSIVES);

    ASSERT(p_special->Genus.Special->SpecialType == SPECIAL_EXPLOSIVES);

    if (p_special) {
        if (p_special->Genus.Special->ammo == 1) {
            //
            // Drop the special.
            //

            p_special->WorldPos = p_person->WorldPos;

            special_drop(p_special, p_person);

            if (p_person->Genus.Person->SpecialUse == THING_NUMBER(p_special)) {
                p_person->Genus.Person->SpecialUse = NULL;
            }
        } else {
            p_special->Genus.Special->ammo -= 1;

            //
            // Create a new explosives special...
            //

            p_special = alloc_special(
                SPECIAL_EXPLOSIVES,
                SPECIAL_SUBSTATE_NONE,
                p_person->WorldPos.X >> 8,
                p_person->WorldPos.Y >> 8,
                p_person->WorldPos.Z >> 8,
                NULL);
        }

        //
        // Prime it!
        //

        p_special->SubState = SPECIAL_SUBSTATE_ACTIVATED;
        p_special->Genus.Special->timer = 16 * 20 * 5;                 // 10 seconds so self destruct.
        p_special->Genus.Special->OwnerThing = THING_NUMBER(p_person); // So we know who is responsible for the explosion.

        // CONSOLE_text("Five second fuse set...");
        /*
                        add_damage_text(
                                p_special->WorldPos.X          >> 8,
                                p_special->WorldPos.Y + 0x2000 >> 8,
                                p_special->WorldPos.Z          >> 8,
                                XLAT_str(X_FUSE_SET));
        */

        PANEL_new_info_message(XLAT_str(X_FUSE_SET));
    }
}
