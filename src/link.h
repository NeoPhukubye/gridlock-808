#ifndef LINK_H
#define LINK_H

#include "network.h"

// Function prototypes
void link_init();
int link_sync_inputs(unsigned short my_input, unsigned short* all_inputs);
LinkState link_get_state();

#endif // LINK_H
