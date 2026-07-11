#include "network.h"

// Placeholder for low-level GBA link state
static LinkState currentLinkState = LINK_STATE_INIT;

void link_init() {
    REG_RCNT = 0; // General purpose I/O
    REG_SIOCNT = SIO_MODE_MULTIPLAYER; // Set Multi-Play mode
    currentLinkState = LINK_STATE_READY;
}

// Synchronize inputs across all connected GBAs
// Returns the number of connected players
int link_sync_inputs(unsigned short my_input, unsigned short* all_inputs) {
    // In a real implementation, this is a complex state machine.
    // This is a simplified placeholder.

    // For now, we just copy our own input for single-player testing
    for(int i=0; i<4; i++) {
        all_inputs[i] = my_input;
    }
    
    currentLinkState = LINK_STATE_ACTIVE;

    // Pretend we found 1 player (ourselves)
    return 1; 
}

LinkState link_get_state() {
    return currentLinkState;
}
