#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "grid.h"
#include "player.h"
#include "link.h"
#include "text.h"

// Main game loop
int main() {
    // GBA hardware initialization
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    audio_init();

    // Game objects
    Grid grid;
    Player players[4] = { Player(0), Player(1), Player(2), Player(3) };
    int num_players = 1;
    unsigned short inputs[4];

    link_init();

    while (1) {
        // Halt the CPU until the next VBlank
        // Wait for VBlank (VSync)
        while (REG_VCOUNT >= 160);
        while (REG_VCOUNT < 160);

        // Clear screen to black
        memset((void*)0x06000000, 0, 240 * 160 * 2);

        // Advance synthwave background music sequencer
        audio_update();

        scanKeys();

        // Check for local developer cheats
        int local_id = link_get_player_id();
        unsigned short held = keysHeld();
        unsigned short pressed = keysDown();

        if (held & KEY_SELECT) {
            if (pressed & KEY_R) {
                players[local_id].toggleInfiniteShield();
                play_payload_sound(); // Play chime confirmation
            }
            if (pressed & KEY_L) {
                players[local_id].toggleHyperSpeed();
                play_payload_sound(); // Play chime confirmation
            }
            if (pressed & KEY_START) {
                int px = players[local_id].getX();
                int py = players[local_id].getY();
                for (int ny = 0; ny < GRID_HEIGHT; ++ny) {
                    for (int nx = 0; nx < GRID_WIDTH; ++nx) {
                        if (grid.getTile(nx, ny) == TILE_NODE) {
                            grid.setTile(nx, ny, TILE_NEUTRAL);
                        }
                    }
                }
                grid.setTile(px, py, TILE_NODE);
                play_payload_sound(); // Play chime confirmation
            }
        }

        // Get local input with smooth repeating (hold keys to walk)
        static int move_delay = 0;
        unsigned short my_input = 0;

        if (held & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)) {
            if (move_delay == 0) {
                my_input = held & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT);
                move_delay = 8; // Repeat move every 8 frames (7.5 steps/sec)
            } else {
                move_delay--;
            }
        } else {
            move_delay = 0; // Reset delay when keys are released
        }

        // Also register instant moves on initial key press
        if (pressed & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)) {
            my_input = pressed & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT);
            move_delay = 8;
        }

        // Include any action button presses (A button) instantly
        my_input |= (pressed & KEY_A);

        // Synchronize inputs with other players
        num_players = link_sync_inputs(my_input, inputs);

        // Update all players
        for (int i = 0; i < num_players; ++i) {
            players[i].update(inputs[i], grid);

            if (inputs[i] & KEY_A) {
                players[i].usePayload(grid, players, num_players);
            }

            // Check for tile interactions
            TileType currentTile = grid.getTile(players[i].getX(), players[i].getY());
            if (currentTile == TILE_NODE) {
                play_payload_sound();
                // Pick up a random payload
                players[i].setPayload((Payload)(rand() % 3 + 1)); // PAYLOAD_FIREWALL, GLITCH, or SHIELD
                // Relocate the node
                grid.setTile(players[i].getX(), players[i].getY(), TILE_NEUTRAL);
                grid.setTile(rand() % (GRID_WIDTH - 2) + 1, rand() % (GRID_HEIGHT - 2) + 1, TILE_NODE);

            } else if (currentTile == TILE_NEUTRAL) {
                play_capture_sound();
                // Capture the tile
                grid.setTile(players[i].getX(), players[i].getY(), (TileType)(TILE_CAPTURED_P1 + i));
                players[i].increaseScore(10);
            }
        }

        // Update grid logic
        grid.update();

        // Render the game state
        grid.render();
        for (int i = 0; i < num_players; ++i) {
            players[i].render();
        }

        // Display scores
        char score_str[20];
        for (int i = 0; i < num_players; ++i) {
            if (i == local_id && num_players > 1) {
                sprintf(score_str, "P%d*:%d", i + 1, players[i].getScore());
                draw_text(10, i * 10, score_str, RGB5(10, 31, 10)); // Green for local player
            } else {
                sprintf(score_str, "P%d: %d", i + 1, players[i].getScore());
                draw_text(10, i * 10, score_str, RGB5(31, 31, 31)); // White for others
            }
        }
    }

    return 0;
}
