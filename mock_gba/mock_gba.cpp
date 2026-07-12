#include <stdio.h>
#include <stdlib.h>
#include "gba_video.h"
#include "gba_system.h"
#include "gba_input.h"
#include "../src/network.h"

// Global mock register instances
volatile unsigned short REG_DISPCNT = 0;
int mock_frame_count = 0;

volatile unsigned short mock_REG_SIOCNT = 0;
volatile unsigned short mock_REG_SIOMLT_SEND = 0;
volatile unsigned short mock_REG_RCNT = 0;
volatile unsigned short mock_REG_SIOMULTI0 = 0xFFFF;
volatile unsigned short mock_REG_SIOMULTI1 = 0xFFFF;
volatile unsigned short mock_REG_SIOMULTI2 = 0xFFFF;
volatile unsigned short mock_REG_SIOMULTI3 = 0xFFFF;

static unsigned short mock_input_keys = 0;

void scanKeys() {
    // In our headless simulation, we will simulate various button presses over time
    mock_input_keys = 0;
    
    // Press SELECT to test glitch packets, and A to use payloads
    if (mock_frame_count == 15) {
        mock_input_keys |= KEY_START; // start game / menu
    }
    if (mock_frame_count == 25) {
        mock_input_keys |= KEY_RIGHT; // move right
    }
    if (mock_frame_count == 35) {
        mock_input_keys |= KEY_DOWN; // move down
    }
    if (mock_frame_count == 45) {
        mock_input_keys |= KEY_A; // use payload
    }
}

unsigned short keysDown() {
    return mock_input_keys;
}

void VBlankIntrWait() {
    mock_frame_count++;

    // Simulate SIO link cable communication if start bit is set
    if (mock_REG_SIOCNT & SIO_START) {
        // Master (us) data is copied to Multi0
        mock_REG_SIOMULTI0 = mock_REG_SIOMLT_SEND;

        // If it's a handshake query, respond with the handshake token from Player 2 (slot 1)
        if (mock_REG_SIOMLT_SEND == 0x55AA) {
            mock_REG_SIOMULTI1 = 0x55AA; // Player 2 ready
            mock_REG_SIOMULTI2 = 0xFFFF; // Player 3 disconnected
            mock_REG_SIOMULTI3 = 0xFFFF; // Player 4 disconnected
        } else {
            // During gameplay, simulate some simple inputs for Player 2
            unsigned short p2_keys = 0;
            if (mock_frame_count > 20 && mock_frame_count < 40) {
                p2_keys |= KEY_LEFT; // Player 2 moves left
            }
            mock_REG_SIOMULTI1 = p2_keys;
            mock_REG_SIOMULTI2 = 0xFFFF;
            mock_REG_SIOMULTI3 = 0xFFFF;
        }

        // Hardware clears the start bit on completion
        mock_REG_SIOCNT &= ~SIO_START;
    }

    // Print periodic test logs
    if (mock_frame_count % 10 == 0) {
        printf("[Frame %d] Simulated tick. REG_SIOCNT: 0x%04X, SIOMULTI1: 0x%04X\n", 
               mock_frame_count, mock_REG_SIOCNT, mock_REG_SIOMULTI1);
    }

    // Exit after 100 frames so testing completes automatically
    if (mock_frame_count >= 100) {
        printf("Headless simulation test finished successfully after 100 frames.\n");
        exit(0);
    }
}
