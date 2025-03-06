//
// For doing network game stuff...
//

#ifndef CNET_H
#define CNET_H

//
// The configuring loop.
//

extern std::uint8_t CNET_network_game;
extern std::uint8_t CNET_i_am_host;
extern std::uint8_t CNET_num_players;
extern std::uint8_t CNET_player_id;

std::int32_t CNET_configure();

#endif
