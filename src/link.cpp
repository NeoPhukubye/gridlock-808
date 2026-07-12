#include "network.h"
#include "link.h"

// State variables for the multiplayer link
static LinkState currentLinkState = LINK_STATE_INIT;
static int numConnectedPlayers = 1;
static int myPlayerId = 0;

// Helper function to transfer data over SIO Multi-Player mode with a safety timeout
static bool link_transfer(unsigned short data, unsigned short* results) {
    // Write local player's data to the send register
    REG_SIOMLT_SEND = data;

    // Check if we are Parent (SI Terminal bit 2 is 0) or Child (SI Terminal bit 2 is 1)
    bool is_parent = !(REG_SIOCNT & SIO_SI_TERMINAL);

    if (is_parent) {
        // Start the transfer
        REG_SIOCNT |= SIO_START;

        // Wait for the transfer to complete
        volatile int timeout = 0;
        while (REG_SIOCNT & SIO_START) {
            timeout++;
            if (timeout > 40000) {
                return false; // Timed out
            }
        }
    } else {
        // Child GBA: wait for Parent to start transfer, then wait for completion
        volatile int timeout = 0;
        
        // 1. Wait for busy bit to become 1 (transfer started by Parent)
        while (!(REG_SIOCNT & SIO_START)) {
            timeout++;
            if (timeout > 40000) {
                return false; // Timed out
            }
        }

        // 2. Wait for busy bit to become 0 (transfer finished)
        timeout = 0;
        while (REG_SIOCNT & SIO_START) {
            timeout++;
            if (timeout > 40000) {
                return false; // Timed out
            }
        }
    }

    // Check for transmission error flag
    if (REG_SIOCNT & SIO_ERROR) {
        return false;
    }

    // Read received results for all 4 slots
    results[0] = REG_SIOMULTI0;
    results[1] = REG_SIOMULTI1;
    results[2] = REG_SIOMULTI2;
    results[3] = REG_SIOMULTI3;

    return true;
}

void link_init() {
    REG_RCNT = 0; // Select SIO communication mode
    REG_SIOCNT = SIO_MODE_MULTIPLAYER | 3; // Multiplayer mode, 115200 bps

    currentLinkState = LINK_STATE_SYNCING;
    numConnectedPlayers = 1;
    myPlayerId = 0;

    int handshake_success_count = 0;
    unsigned short results[4];

    // Try to perform handshakes to detect connected players
    for (int attempts = 0; attempts < 60; ++attempts) {
        // Short delay to let other GBAs initialize (roughly 1 frame equivalent)
        for (volatile int d = 0; d < 8000; d++);

        // Send a handshake magic token
        if (link_transfer(0x55AA, results)) {
            // Count how many players responded with the correct handshake token
            int players_detected = 1;
            if (results[1] == 0x55AA) players_detected = 2;
            if (results[2] == 0x55AA) players_detected = 3;
            if (results[3] == 0x55AA) players_detected = 4;

            if (players_detected > 1) {
                handshake_success_count++;
                numConnectedPlayers = players_detected;
                myPlayerId = (REG_SIOCNT & SIO_ID_MASK) >> SIO_ID_SHIFT;

                // Confirm connection if we have 10 successful transfers in a row
                if (handshake_success_count >= 10) {
                    currentLinkState = LINK_STATE_ACTIVE;
                    break;
                }
            } else {
                handshake_success_count = 0;
            }
        } else {
            handshake_success_count = 0;
            // Reset SIO control on error
            if (REG_SIOCNT & SIO_ERROR) {
                REG_SIOCNT = SIO_MODE_MULTIPLAYER | 3;
            }
        }
    }

    // If no multiplayer connection was established, fall back to single-player
    if (currentLinkState != LINK_STATE_ACTIVE) {
        currentLinkState = LINK_STATE_READY;
        numConnectedPlayers = 1;
        myPlayerId = 0;
    }
}

int link_sync_inputs(unsigned short my_input, unsigned short* all_inputs) {
    if (currentLinkState == LINK_STATE_ACTIVE) {
        unsigned short results[4];
        if (link_transfer(my_input, results)) {
            all_inputs[0] = results[0];
            all_inputs[1] = results[1];
            all_inputs[2] = results[2];
            all_inputs[3] = results[3];
            return numConnectedPlayers;
        } else {
            // Error occurred (e.g., player disconnected mid-game)
            currentLinkState = LINK_STATE_ERROR;
            all_inputs[0] = my_input;
            for (int i = 1; i < 4; ++i) {
                all_inputs[i] = 0;
            }
            return 1;
        }
    } else {
        // LINK_STATE_READY or LINK_STATE_ERROR: single-player fallback
        all_inputs[0] = my_input;
        for (int i = 1; i < 4; ++i) {
            all_inputs[i] = 0;
        }
        return 1;
    }
}

LinkState link_get_state() {
    return currentLinkState;
}

int link_get_player_id() {
    return myPlayerId;
}

int link_get_num_players() {
    return numConnectedPlayers;
}
