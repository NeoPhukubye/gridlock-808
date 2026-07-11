#include "grid.h"

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
