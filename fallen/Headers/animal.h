//
// Animals.
//

#ifndef ANIMAL_H
#define ANIMAL_H

#define MAX_ANIMALS 6

#define ANIMAL_NONE 0  // zilch, nada
#define ANIMAL_CANID 1 // dogs, wolves, coyotes and other 4-legged freaks
#define ANIMAL_NUMBER 2

typedef struct
{
    Thing *target;               // chasing, barking
    std::uint16_t counter;       // random delays etc
    std::uint16_t dist;          // generically useful when pathfinding, chasing, etc
    std::uint16_t starty;        // initial height? remove?
    std::uint8_t AnimalType;     // species
    std::uint8_t substate;       // animal behaviour code
    std::uint8_t map_x;          // these store the ready-shifted world position
    std::uint8_t map_z;          // of the animal
    std::uint8_t dest_x, dest_z; // same, for a temporary destination point
    std::uint8_t home_x, home_z; // spawn point for homing
    std::uint8_t extra;          // animal-specific
    std::uint8_t padding;
    //	DrawTween *dts[10];		// body parts;
} Animal;

// typedef struct Animal Animal;
typedef Animal *AnimalPtr;

void init_animals();
struct Thing *alloc_animal(std::uint8_t type);
void free_animal(struct Thing *animal_thing);

//
// The animal state functions.
//

extern GenusFunctions ANIMAL_functions[ANIMAL_NUMBER];

//
// Registers an animal's body part...
//

// GameKeyFrameChunk *ANIMAL_register(char *filename, float x, float y, float z);
GameKeyFrameChunk *ANIMAL_register(char *filename);

//
// Creates an animal thing of the given type.  It puts it at the
// given position on the mapwho and puts it into state STATE_INIT.
//

Thing *ANIMAL_create(GameCoord pos, std::uint8_t type);

//
// Animates the animal, returns 1 if the anim is finished
//

std::uint8_t ANIMAL_animate(Thing *animal);

//
// Changes the animation
//

void ANIMAL_set_anim(Thing *thing, std::int32_t anim);

//
// Draw the animal -- provided for AENG...
//

void ANIMAL_draw(Thing *p_thing);

//
// Returns the Animal structure associated with the given animal thing.
// Returns the DrawMesh structure associted with the given animal thing.
//

Animal *ANIMAL_get_animal(Thing *animal_thing);
// DrawTween *ANIMAL_get_drawtween(Animal *animal);
// std::int32_t ANIMAL_body_size(Animal *animal);

//
// Some useful centering stuff i had nowhere else convenient to shove
//

// GameCoord GetChunkCentre(GameKeyFrameChunk *chunk);
// SetChunkCentre(GameKeyFrameChunk *chunk, GameCoord centre);

#endif
