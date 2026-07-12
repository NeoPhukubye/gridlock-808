#include "grid.h"
#include <gba_video.h>

Grid::Grid() {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (x == 0 || x == GRID_WIDTH - 1 || y == 0 || y == GRID_HEIGHT - 1) {
                tiles[x][y] = TILE_FIREWALL; // Use firewall as border
            } else {
                tiles[x][y] = TILE_NEUTRAL;
            }
        }
    }
    // Placeholder for nodes
    tiles[GRID_WIDTH / 2][GRID_HEIGHT / 2] = TILE_NODE;
}

void Grid::update() {
    // Game logic for the grid, e.g., decaying tiles
}

void Grid::render() {
    // Render the grid to the screen
    // This would involve writing to VRAM
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            unsigned short color = 0;
            switch (tiles[x][y]) {
                case TILE_NEUTRAL: color = RGB5(10, 10, 10); break; // Dark grey
                case TILE_CAPTURED_P1: color = RGB5(31, 15, 15); break; // Light red
                case TILE_CAPTURED_P2: color = RGB5(15, 15, 31); break; // Light blue
                case TILE_CAPTURED_P3: color = RGB5(15, 31, 15); break; // Light green
                case TILE_CAPTURED_P4: color = RGB5(31, 31, 15); break; // Light yellow
                case TILE_FIREWALL: color = RGB5(20, 20, 20); break; // Grey
                case TILE_NODE: color = RGB5(31, 31, 31); break; // White
            }
            // This is not efficient, a real implementation would use tilemaps
            for(int i = 0; i < 16; i++) {
                for (int j = 0; j < 16; j++) {
                    m3_plot(x * 16 + i, y * 16 + j, color);
                }
            }
        }
    }
}

bool Grid::isMoveValid(int x, int y) const {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) {
        return false;
    }
    return tiles[x][y] != TILE_FIREWALL;
}

void Grid::setTile(int x, int y, TileType type) {
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        tiles[x][y] = type;
    }
}

TileType Grid::getTile(int x, int y) const {
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        return tiles[x][y];
    }
    return TILE_FIREWALL;
}
