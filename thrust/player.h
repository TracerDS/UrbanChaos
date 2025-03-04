//
// A player.
//

#ifndef _PLAYER_
#define _PLAYER_


#include "server.h"



//
// The keypress history for each player.
//

typedef struct
{
	//
	// A circular queue of keypresses where key[key_head] is
	// the keypresses for the current key_gameturn and
	// key[(key_head - 1) & (PLAYER_NUM_KEYS - 1)] is the
	// keypresses for the previous gameturn.
	//

	#define PLAYER_KEY_LEFT           (1 << 0)
	#define PLAYER_KEY_RIGHT          (1 << 1)
	#define PLAYER_KEY_THRUST         (1 << 2)
	#define PLAYER_KEY_TRACTOR_BEAM   (1 << 3)

	#define PLAYER_NUM_KEYS 128		// Power of 2 please!

	std::uint8_t key[PLAYER_NUM_KEYS];
 
	//
	// This is the gameturn upto which we know for sure what
	// the keys pressed down are for this player.
	//

	std::int32_t known;

} PLAYER_Key;

#define PLAYER_MAX_PLAYERS SHIP_MAX_SHIPS

extern PLAYER_Key PLAYER_key[PLAYER_MAX_PLAYERS];
extern std::int32_t      PLAYER_key_start;		// The first valid gameturn
extern std::int32_t      PLAYER_key_gameturn;	// The last valid gameturn



//
// The player joining/leaving history.
//

typedef struct player_message
{
	union
	{
		std::uint8_t type;

		SERVER_Block_new_player  new_player;
		SERVER_Block_player_left player_left; 
	};

	struct player_message *next;

} PLAYER_Message;

#define PLAYER_NUM_MESSAGES PLAYER_NUM_KEYS

extern PLAYER_Message *PLAYER_message[PLAYER_NUM_MESSAGES];
extern std::int32_t           PLAYER_message_start;	// The first valid gameturn
extern std::int32_t           PLAYER_message_gameturn;	// The last valid gameturn 


//
// The gamestate history. The gamestate stored is at it was before
// any processing occurred. So if a keypress changes in gameturn x,
// the gamestate is rolled back to gameturn x, not gameturn (x - 1).
//

typedef struct
{
	std::int32_t           gameturn; // The gameturn when this was the gamestate.
	GAMESTATE_State gs;

} PLAYER_Gamestate;

#define PLAYER_NUM_GAMESTATES 128

extern PLAYER_Gamestate PLAYER_gamestate[PLAYER_NUM_GAMESTATES];
extern std::int32_t            PLAYER_gamestate_start;		// The first valid gameturn
extern std::int32_t            PLAYER_gamestate_gameturn;	// The last valid gameturn 


//
// Initialises the player module.
//

void PLAYER_init();



//
// Whenever your about to do the processing for a gameturn, call this
// function so that the player module can store the gamestate.
//

void PLAYER_new_gameturn(std::int32_t gameturn);



//
// Informs the server that we would like a local player to control.
// It thens waits for a copy of gamestate and tells the server when
// it has finally got it.  The server will respond be sending out a
// an activate player packet that will be caught by PLAYER_process().
//

#define PLAYER_CREATE_OK              0
#define PLAYER_CREATE_LOST_CONNECTION 1
#define PLAYER_CREATE_TIMED_OUT       2

std::int32_t PLAYER_create_local(
		char* name,
		std::uint8_t  red,
		std::uint8_t  green,
		std::uint8_t  blue,
		float  ship_mass  = 1.0F,
		float  ship_power = 1.0F);



//
// Processes the keypresses for the local player and sends the info to the server.
// It gets the keypresses for the other players from the server.
//
// It gets server messages about remote players joining and leaving
// and puts them in the PLAYER_message() array.
//
// This function false if you lose connection with the server.
//
// If the server reports a keypress in the past that is different from what
// we guessed the keypress would be, then gamestate is rolled back to what
// it was before the change and *rollback is set to the gameturn that
// the game has been rolled back to, otherwise *rollback is set to 0.
//
// If (ignore server messages) then it doesn't take any notice of the
// server and just looks for local keypresses.
//

std::int32_t PLAYER_process(std::int32_t *rollback, std::int32_t ignore_server_messages);	// If (*rollback == 0) then no rollback occurred.


//
// Processes the player leaving\joining messages for the given gameturn.
// This function looks at the messages in the PLAYER_message[] queue.
//

void PLAYER_process_messages(std::int32_t gameturn);


//
// Puts the keypresses for the given gameturn into the ships of each player.
//

void PLAYER_press_keys(std::int32_t gameturn);



#endif





