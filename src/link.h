#ifndef LINK_H
#define LINK_H

#include "network.h"

// Function prototypes
void link_init();
int link_sync_inputs(unsigned short my_input, unsigned short* all_inputs);
LinkState link_get_state();
int link_get_player_id();
int link_get_num_players();

#endif // LINK_H
