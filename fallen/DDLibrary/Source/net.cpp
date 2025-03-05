//
// Direct play stuff.
//

#include "DDLib.h"
#include "net.h"

//
// Error checking...
//

#ifndef VERIFY
#ifdef NDEBUG
#define VERIFY(x) \
    {             \
        x;        \
    }
#else
#define VERIFY(x)  \
    {              \
        ASSERT(x); \
    }
#endif
#endif

//
// true if this machine is the host.
// The local player number.
//

std::uint8_t NET_i_am_host;
std::uint8_t NET_local_player;

//
// All the players.
//

typedef struct
{
    std::uint8_t used;
    std::uint8_t player_id;
    std::uint16_t shit;
    // DPID  dpid;
    char name[NET_NAME_LENGTH];

} NET_Player;

#define NET_MAX_PLAYERS 32

NET_Player NET_player[NET_MAX_PLAYERS];

//
// The buffer into which messages are recieved.
//

char NET_buffer[NET_MESSAGE_LENGTH];

std::int32_t net_init_done = 0;

void NET_init() {
}

void NET_kill() {
}

std::int32_t NET_get_connection_number() {
    return 0;
}

char* NET_get_connection_name(std::int32_t connection) {
    return nullptr;
}

std::int32_t NET_connection_make(std::int32_t connection) {
    return false;
}

std::int32_t NET_create_session(char* session_name, std::int32_t max_players, char* my_player_name) {
    return false;
}

std::int32_t NET_get_session_number() {
    return 0;
}

NET_Sinfo NET_get_session_info(std::int32_t session) {
    return {};
}

std::int32_t NET_join_session(std::int32_t session, char* my_player_name) {
    return false;
}

std::uint8_t NET_start_game() {
    return 0;
}

void NET_leave_session() {
}

std::int32_t NET_get_num_players() {
    return 1;
}

char* NET_get_player_name(std::int32_t player) {
    return "Unknown";
}

void NET_message_send(
    std::uint8_t player_id,
    void* data,
    std::uint16_t num_bytes) {
}

std::int32_t NET_message_waiting() {
    return 0;
}

void NET_message_get(NET_Message* ans) {
    return;
}
