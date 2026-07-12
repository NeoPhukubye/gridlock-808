#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdlib.h>
#include <stdio.h>

#include "grid.h"
#include "player.h"
#include "link.h"
#include "text.h"

// Main game loop
int main() {
    // GBA hardware initialization
    REG_DISPCNT = MODE_3 | BG2_ENABLE;

    // Game objects
    Grid grid;
    Player players[4] = { Player(0), Player(1), Player(2), Player(3) };
    int num_players = 1;
    unsigned short inputs[4];

    link_init();

    while (1) {
        // Halt the CPU until the next VBlank
        VBlankIntrWait();
        scanKeys();

        // Get local input
        unsigned short my_input = keysDown();

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
                // Pick up a random payload
                players[i].setPayload((Payload)(rand() % 3 + 1)); // PAYLOAD_FIREWALL, GLITCH, or SHIELD
                // Relocate the node
                grid.setTile(players[i].getX(), players[i].getY(), TILE_NEUTRAL);
                grid.setTile(rand() % (GRID_WIDTH - 2) + 1, rand() % (GRID_HEIGHT - 2) + 1, TILE_NODE);

            } else if (currentTile == TILE_NEUTRAL) {
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
        int local_id = link_get_player_id();
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
