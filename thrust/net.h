//
// Network stuff...
//

#ifndef NET_H
#define NET_H



//
// The maximum length of message and name strings
//

#define NET_MAX_MESSAGE_LENGTH (4 * 1024)	// 4k
#define NET_MAX_NAME_LENGTH    (256)



// ========================================================
//
// Initialising and releasing all the DirectPlay stuff...
// Call once each at the start and end of the program.
// 
// ========================================================

std::int32_t NET_init();	// Returns false on failure.
void NET_kill();



// ========================================================
//
// CONNECTION STUFF
//
// ========================================================

//
// Establishes a connection or returns true is the connection is
// already active.  Return false on failure.
//

std::int32_t NET_connection_lan     ();		// Makes a connection to the LAN
std::int32_t NET_connection_internet(char* str);	// The internet using TCP/IP give is an string like "124.15.239.24" or "www.muckyfoot.com"




// ========================================================
//
// SERVER SESSION STUFF
//
// ========================================================

//
// Creates a session. Returns true on success.
//		 

std::int32_t NET_session_create (char* session_name, std::int32_t max_players);
void NET_session_destroy();


// ========================================================
//
// PLAYER SESSION STUFF
//
// ========================================================

//
// Returns the number of sessions available.
// Gets info about the given session.
//

typedef struct
{
	char name[NET_MAX_NAME_LENGTH];
	std::int32_t max_players;
	std::int32_t num_players;

} NET_Sinfo;

std::int32_t NET_session_get_number();
NET_Sinfo NET_session_get_info  (std::int32_t session);

//
// Joins the given session. Returns true on success.
//

std::int32_t NET_session_join(std::int32_t session);

//
// Leaves the session.
//

void NET_session_leave();



// ========================================================
//
// PLAYER MESSAGE STUFF
//
// ========================================================

//
// For a player to send a message to the server.
//

void NET_player_message_send(std::int32_t num_bytes, void* data, std::int32_t guaranteed = false);

//
// Receiving messages. If you get NET_MESSAGE_FROM_SERVER, then *num_bytes
// and *data will be filled in with the message received.
//

#define NET_PLAYER_MESSAGE_NONE            0
#define NET_PLAYER_MESSAGE_LOST_CONNECTION 1
#define NET_PLAYER_MESSAGE_FROM_SERVER     2

std::int32_t NET_player_message_receive(std::int32_t *num_bytes, void* *data);



// ========================================================
//
// SERVER MESSAGE STUFF
//
// ========================================================

typedef std::int32_t NET_Player;

//
// Sends a message to the given player.
//

void NET_server_message_to_player(NET_Player player, std::int32_t num_bytes, void* data, std::int32_t guaranteed = false);

//
// Receives a message.
//

#define NET_SERVER_MESSAGE_NONE            0
#define NET_SERVER_MESSAGE_LOST_CONNECTION 1
#define NET_SERVER_MESSAGE_PLAYER_LEFT     2	// *player contains the player who left
#define NET_SERVER_MESSAGE_PLAYER_JOINED   3	// *player contains the new player
#define NET_SERVER_MESSAGE_FROM_PLAYER     4	// A message from player *player.

std::int32_t NET_server_message_receive(NET_Player *player, std::int32_t *num_bytes, void* *data);



#endif

