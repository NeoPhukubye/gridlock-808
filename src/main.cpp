#include <gba_base.h>
#include <gba_video.h>
#include <gba_system.h>
#include <gba_input.h>
#include <stdlib.h>

#include "grid.h"
#include "player.h"
#include "link.h"

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
            }
        }

        // Update grid logic
        grid.update();

        // Render the game state
        grid.render();
        for (int i = 0; i < num_players; ++i) {
            players[i].render();
        }
    }

    return 0;
}
