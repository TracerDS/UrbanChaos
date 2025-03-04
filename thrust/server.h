//
// The server.
//

#pragma once



#include "gamestate.h"




//
// Creates a new session. Returns true on success.
//

#define SERVER_CONNECT_TYPE_LAN      0
#define SERVER_CONNECT_TYPE_INTERNET 1

std::int32_t SERVER_session_create(
		char* name,
		std::int32_t  max_players,
		std::int32_t  connection_type,
		char* internet_address);

//
// Ends the current session.
//

void SERVER_session_destroy();



//
// Process the server. Gets keypresses from all the players and
// sends them out to eachother.  Receives requests for new players
// and handles them joining and leaving.
//

void SERVER_process();


//
// Draws the dedicated server screen- this only draws stuff a few
// times every second. Most of the time it just returns.
//

void SERVER_draw();



// ========================================================
//
// HOW TO JOIN A GAME
//
// ========================================================

//
// - Ping with the server to establish a common time.
// - Request joining the game.
// - Wait for a copy of the gamestate from the server.
// - Send the server a message when you have recieved gamestate.
// - Start processing the game with the keylist messages the
//   server will start sending you.
// - You will now receive an activate player message telling you
//   on which game turn your ship can start moving.
//



// ========================================================
//
// SERVER MESSAGES
//
// ========================================================

//
// Here is a description of the messages that the server sends and
// expects to receive.
//

//
// Each message start with a std::int32_t game turn followed by a number of blocks.
// The first byte of a block is the block type, and then there follows the
// block data.
//

#define SERVER_BLOCK_TYPE_REQUEST_JOIN			1
#define SERVER_BLOCK_TYPE_NEW_PLAYER			2
#define SERVER_BLOCK_TYPE_ACTIVATE_NEW_PLAYER	3
#define SERVER_BLOCK_TYPE_MY_KEYPRESS_LIST		4
#define SERVER_BLOCK_TYPE_REMOTE_KEYPRESS_LIST	5
#define SERVER_BLOCK_TYPE_PING					6
#define SERVER_BLOCK_TYPE_GAMESTATE				7
#define SERVER_BLOCK_TYPE_RECEIVED_GAMESTATE	8
#define SERVER_BLOCK_TYPE_PLAYER_LEFT           9

//
// JOIN. Send this message to the server when you want to join
// in the session.  Send this message after you have pinged
// with the server to establish a common time.
//

typedef struct
{
	std::uint8_t type;			// i.e. SERVER_BLOCK_TYPE_REQUEST_JOIN
	std::uint8_t red;
	std::uint8_t green;
	std::uint8_t blue;
	float mass;
	float power;
	char name[32];
 
} SERVER_Block_request_join;




//
// NEW_PLAYER.  The server is telling you that a new player has joined
// in the game.
//

typedef struct
{
	std::uint8_t type;			// i.e. SERVER_BLOCK_TYPE_NEW_PLAYER
	std::uint8_t red;
	std::uint8_t green;
	std::uint8_t blue;
	char name[32];
	std::uint8_t ship_index;	// Index into the SHIP_ship structure.
	std::uint8_t local;		// if true, then this is you!
	std::uint16_t padding;
	std::int32_t active;		// The gameturn when this ship becomes active.
	float x;
	float y;
	float mass;
	float power;

} SERVER_Block_new_player;





//
// MY KEYPRESS LIST. Tells the server about your last few keypresses.
//

typedef struct
{
	std::uint8_t type;			// SERVER_BLOCK_TYPE_MY_KEYPRESS_LIST
	std::uint8_t ship_index;	// Index into the SHIP_ship structure.
	std::uint8_t num_keys;
	std::uint8_t padding;
	float hash;			// So we know if we're in sync.
	std::uint8_t key[];		// The array of keys...

} SERVER_Block_my_keypress_list;



//
// REMOTE KEYPRESS LIST.  This is the server telling you about a remote player's keypresses.
//

typedef struct
{
	std::uint8_t type;				// SERVER_BLOCK_TYPE_REMOTE_KEYPRESS_LIST
	std::uint8_t ship_index;		// Index into the SHIP_ship structure.
	std::uint8_t num_keys;
	std::uint8_t padding;
	std::int32_t gameturn;			// The turn the first key is for.
	std::uint8_t key[];			// The array of keys...


} SERVER_Block_remote_keypress_list;



//
// PING. Use ping as a NOP message to establish a common time with the server.
// This is the only message where you don't have to send a valid initial std::int32_t gameturn.
//

typedef struct
{
	std::uint8_t type;				// SERVER_BLOCK_TYPE_PING
	std::uint8_t padding[3];		
	std::int32_t id;				// When the server returns your ping, it will use the same id.
	std::int32_t game_process;		// The current high resolution game turn.

} SERVER_Block_ping;


//
// GAMESTATE. The gamestate at a particular time.
//

typedef struct
{
	std::uint8_t type;				// SERVER_BLOCK_TYPE_GAMESTATE
	std::uint8_t padding[3];		

	std::int32_t gameturn;	// The gameturn when this gamestate is for. It may not
					// be the same as the gameturn when the message is sent.

	GAMESTATE_State gs;

} SERVER_Block_gamestate;



//
// RECEIVED_GAMESTATE. Tell the server that you have received gamestate. Start
// processing gamestate after sending this message.  You will receive remote
// kepress list messages and should send keypress messages yourself.
//

typedef struct
{
	std::uint8_t type;				// SERVER_BLOCK_TYPE_RECEIVED
	std::uint8_t padding[3];

} SERVER_Block_received_gamestate;



//
// PLAYER_LEFT.  A player has left the game.
//

typedef struct
{
	std::uint8_t type;			// SERVER_BLOCK_TYPE_PLAYER_LEFT
	std::uint8_t player_index;	// Index into the PLAYER_player structure.
	std::uint8_t padding[2];
	std::int32_t game_turn_when_the_player_leaves;

} SERVER_Block_player_left;













