//
// Network stuff...
//

#ifndef NET_H
#define NET_H



// ========================================================
//
// Initialising and releasing all the DirectPlay stuff...
// Call once each at the start and end of the program.
// 
// ========================================================

void NET_init();
void NET_kill();


//
// The nullptr player ID.
// Use this player ID to send a message to everyone.
// If the system sends a message, then it comes from NET_PLAYER_SYSTEM
//

#define NET_PLAYER_NONE		255
#define NET_PLAYER_ALL		254
#define NET_PLAYER_SYSTEM	253


// ========================================================
//
// CONNECTION STUFF
//
// ========================================================

//
// Returns the number of connections available.
// Returns the name of the given connection.
//

std::int32_t  NET_get_connection_number();
char* NET_get_connection_name  (std::int32_t connection);

//
// Establishes a connection. Returns true on success.
//

std::int32_t NET_connection_make(std::int32_t connection);


// ========================================================
//
// SESSION STUFF
//
// ========================================================

//
// Creates a session. Makes this machine the host. Returns false
// on failure.
//		 

std::int32_t NET_create_session(char* name, std::int32_t max_players, char* my_player_name);

//
// Returns the number of sessions available.
// Gets info about the given session.
// Joins the given session.
//

#define NET_NAME_LENGTH 64

typedef struct
{
	char name[NET_NAME_LENGTH];

} NET_Sinfo;

std::int32_t      NET_get_session_number();
NET_Sinfo  NET_get_session_info  (std::int32_t session);

//
// Joins the given session. Returns false on failure.
//

std::int32_t NET_join_session(std::int32_t session, char* my_player_name);

//
// Leaves the session.
//

void NET_leave_session();

//
// Starts the game. It sends out a START_GAME system message
// and assigns player_ids to all the other players. Only the host
// should call this function.  Other players should wait for
// a START_GAME system message.
//
// This function returns your player_id. Do not wait for a START_GAME
// message because you won't get one. Returns NET_PLAYER_NONE on failure.
//

std::uint8_t NET_start_game();

// ========================================================
//
// PLAYER STUFF
//
// ========================================================

//
// To find the player in the current session. To update this list you
// have to process the message loop. The player argument to NET_get_player_name
// is not a player_id, just the n'th player in the session.
//

std::int32_t  NET_get_num_players();
char* NET_get_player_name(std::int32_t player);


// ========================================================
//
// MESSAGE STUFF
//
// ========================================================

//
// Maximum length of a message in bytes.
//

#define NET_MESSAGE_LENGTH (16 * 1024)

//
// Sends a message to the given player. Pass NET_PLAYER_ALL to send
// a broadcast message to everyone.
//

void NET_message_send(
		std::uint8_t  player_id,
		void  *data,
		std::uint16_t  num_bytes);

//
// Returns true if there is a message in the queue.
//

std::int32_t NET_message_waiting();

//
// Gets the next message in the queue.
//

#define NET_SYSMESS_NOP				0	// Do nothing!
#define NET_SYSMESS_START_GAME      1
#define NET_SYSMESS_LOST_CONNECTION	2
#define NET_SYSMESS_HOST_QUIT_OUT	3

typedef struct
{
	std::uint8_t player_id;
	std::uint8_t shit1;
	std::uint16_t shit2;

	union
	{
		struct	// For system messages, when player == NET_PLAYER_SYSTEM
		{
			std::uint8_t sysmess;
			std::uint8_t player_id;	// For the START_GAME system message.
			std::uint8_t shite;
			
		} system;

		struct	// For all other messages.
		{
			std::uint16_t num_bytes;
			std::uint16_t more_shit;
			void* data;
			
		} player;
	};

} NET_Message;

void NET_message_get(NET_Message *answer);


#endif
