#include "grid.h"
#include "network.h"
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
    static int frame = 0;
    frame++;

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            int px = x * 16;
            int py = y * 16;
            TileType tile = tiles[x][y];

            if (tile == TILE_NEUTRAL) {
                // Cybernavy space background
                for (int i = 0; i < 15; i++) {
                    for (int j = 0; j < 15; j++) {
                        m3_plot(px + i, py + j, RGB5(1, 1, 2));
                    }
                }
                // Grid borders (bottom and right only)
                for (int i = 0; i < 16; i++) {
                    m3_plot(px + i, py + 15, RGB5(4, 6, 10)); // Horizontal line
                    m3_plot(px + 15, py + i, RGB5(4, 6, 10)); // Vertical line
                }
                // Center crosshair (2x2)
                m3_plot(px + 7, py + 7, RGB5(5, 8, 12));
                m3_plot(px + 8, py + 7, RGB5(5, 8, 12));
                m3_plot(px + 7, py + 8, RGB5(5, 8, 12));
                m3_plot(px + 8, py + 8, RGB5(5, 8, 12));

            } else if (tile == TILE_FIREWALL) {
                // Hazard stripe diagonal security wall
                for (int i = 0; i < 16; i++) {
                    for (int j = 0; j < 16; j++) {
                        if ((i + j) % 4 == 0 || (i - j) % 4 == 0) {
                            m3_plot(px + i, py + j, RGB5(26, 4, 0)); // Glowing red
                        } else {
                            m3_plot(px + i, py + j, RGB5(5, 0, 0));  // Dark red
                        }
                    }
                }
                // Border lines
                for (int i = 0; i < 16; i++) {
                    m3_plot(px + i, py, RGB5(31, 12, 0));
                    m3_plot(px + i, py + 15, RGB5(31, 12, 0));
                    m3_plot(px, py + i, RGB5(31, 12, 0));
                    m3_plot(px + 15, py + i, RGB5(31, 12, 0));
                }

            } else if (tile == TILE_NODE) {
                // Pulsing data core
                for (int i = 0; i < 15; i++) {
                    for (int j = 0; j < 15; j++) {
                        m3_plot(px + i, py + j, RGB5(1, 1, 2));
                    }
                }
                for (int i = 0; i < 16; i++) {
                    m3_plot(px + i, py + 15, RGB5(4, 6, 10));
                    m3_plot(px + 15, py + i, RGB5(4, 6, 10));
                }

                // Core pulsing animation (based on frame count)
                int pulse = (frame / 6) % 4;
                unsigned short node_color = RGB5(0, 31, 31);   // Neon cyan
                unsigned short center_color = RGB5(31, 31, 31); // Pure white

                // Draw central data block
                for (int i = 6; i <= 9; i++) {
                    for (int j = 6; j <= 9; j++) {
                        m3_plot(px + i, py + j, center_color);
                    }
                }

                // Draw pulsing outer ring
                int r = 3 + pulse;
                for (int i = -r; i <= r; i++) {
                    m3_plot(px + 8 + i, py + 8 - r, node_color);
                    m3_plot(px + 8 + i, py + 8 + r, node_color);
                    m3_plot(px + 8 - r, py + 8 + i, node_color);
                    m3_plot(px + 8 + r, py + 8 + i, node_color);
                }

            } else {
                // Captured tiles: TILE_CAPTURED_P1 to P4
                unsigned short player_color = 0;
                unsigned short dark_color = 0;
                unsigned short stripe_color = 0;
                int p_idx = tile - TILE_CAPTURED_P1;

                switch (p_idx) {
                    case 0: // P1 Red
                        player_color = RGB5(31, 5, 5);
                        dark_color = RGB5(5, 0, 0);
                        stripe_color = RGB5(12, 1, 1);
                        break;
                    case 1: // P2 Blue
                        player_color = RGB5(5, 5, 31);
                        dark_color = RGB5(0, 0, 5);
                        stripe_color = RGB5(1, 1, 12);
                        break;
                    case 2: // P3 Green
                        player_color = RGB5(5, 31, 5);
                        dark_color = RGB5(0, 5, 0);
                        stripe_color = RGB5(1, 12, 1);
                        break;
                    case 3: // P4 Yellow
                        player_color = RGB5(31, 31, 5);
                        dark_color = RGB5(5, 5, 0);
                        stripe_color = RGB5(12, 12, 1);
                        break;
                }

                // Draw dark base and tech stripes
                for (int i = 1; i < 15; i++) {
                    for (int j = 1; j < 15; j++) {
                        if ((i + j) % 6 == 0 || j % 4 == 0) {
                            m3_plot(px + i, py + j, stripe_color);
                        } else {
                            m3_plot(px + i, py + j, dark_color);
                        }
                    }
                }

                // Draw bright glowing borders
                for (int i = 0; i < 16; i++) {
                    m3_plot(px + i, py, player_color);
                    m3_plot(px + i, py + 15, player_color);
                    m3_plot(px, py + i, player_color);
                    m3_plot(px + 15, py + i, player_color);
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
