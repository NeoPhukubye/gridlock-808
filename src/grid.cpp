#include "grid.h"
#include "network.h"
#include <gba_video.h>
#include <stdlib.h>

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

    // Generate random static obstacles (firewalls)
    int obstacles_placed = 0;
    while (obstacles_placed < 6) {
        int rx = rand() % (GRID_WIDTH - 2) + 1;
        int ry = rand() % (GRID_HEIGHT - 2) + 1;

        // Skip spawn points and central node
        if ((rx == 1 && ry == 1) ||
            (rx == GRID_WIDTH - 2 && ry == 1) ||
            (rx == 1 && ry == GRID_HEIGHT - 2) ||
            (rx == GRID_WIDTH - 2 && ry == GRID_HEIGHT - 2) ||
            (rx == GRID_WIDTH / 2 && ry == GRID_HEIGHT / 2)) {
            continue;
        }

        // Place obstacle if cell is neutral
        if (tiles[rx][ry] == TILE_NEUTRAL) {
            tiles[rx][ry] = TILE_FIREWALL;
            obstacles_placed++;
        }
    }
}

void Grid::update() {
    // Game logic for the grid, e.g., decaying tiles
}

void Grid::render() {
    static int frame = 0;
    frame++;

    // 1. Draw neon horizon line in magenta/purple
    draw_line(0, 75, 239, 75, RGB5(12, 0, 12));

    // 2. Trigonometric tables (sine/cosine scaled by 256) for octahedron rotation
    static const int sin_tbl[] = {0, 181, 256, 181, 0, -181, -256, -181};
    static const int cos_tbl[] = {256, 181, 0, -181, -256, -181, 0, 181};
    int rot_idx = (frame / 2) % 8;

    // 3. Render 3D Perspective Grid
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            // Project the 4 bottom corners of the cell
            int x0, y0, x1, y1, x2, y2, x3, y3;
            project(2 * x,     2 * y,     0, &x0, &y0);
            project(2 * x + 2, 2 * y,     0, &x1, &y1);
            project(2 * x + 2, 2 * y + 2, 0, &x2, &y2);
            project(2 * x,     2 * y + 2, 0, &x3, &y3);

            TileType tile = tiles[x][y];

            if (tile == TILE_NEUTRAL) {
                // Draw bottom and right edges in dim blue-gray
                draw_line(x1, y1, x2, y2, RGB5(4, 6, 10));
                draw_line(x2, y2, x3, y3, RGB5(4, 6, 10));

                // Central tech dot
                int cx, cy;
                project(2 * x + 1, 2 * y + 1, 0, &cx, &cy);
                m3_plot(cx, cy, RGB5(5, 8, 12));

            } else if (tile == TILE_FIREWALL) {
                // Project top 4 corners at height 10
                int tx0, ty0, tx1, ty1, tx2, ty2, tx3, ty3;
                project(2 * x,     2 * y,     10, &tx0, &ty0);
                project(2 * x + 2, 2 * y,     10, &tx1, &ty1);
                project(2 * x + 2, 2 * y + 2, 10, &tx2, &ty2);
                project(2 * x,     2 * y + 2, 10, &tx3, &ty3);

                // Base color: crimson red. Pillar lines: glowing red/orange
                unsigned short side_color = RGB5(16, 0, 0);
                unsigned short top_color = RGB5(31, 8, 0);

                // Base wireframe
                draw_line(x0, y0, x1, y1, side_color);
                draw_line(x1, y1, x2, y2, side_color);
                draw_line(x2, y2, x3, y3, side_color);
                draw_line(x3, y3, x0, y0, side_color);

                // Top wireframe
                draw_line(tx0, ty0, tx1, ty1, top_color);
                draw_line(tx1, ty1, tx2, ty2, top_color);
                draw_line(tx2, ty2, tx3, ty3, top_color);
                draw_line(tx3, ty3, tx0, ty0, top_color);

                // Vertical corner pillars
                draw_line(x0, y0, tx0, ty0, side_color);
                draw_line(x1, y1, tx1, ty1, side_color);
                draw_line(x2, y2, tx2, ty2, side_color);
                draw_line(x3, y3, tx3, ty3, side_color);

                // Draw firewall "X" security barrier on the front faces
                draw_line(x3, y3, tx2, ty2, side_color);
                draw_line(x2, y2, tx3, ty3, side_color);

            } else if (tile == TILE_NODE) {
                // Draw dim background grid lines
                draw_line(x1, y1, x2, y2, RGB5(4, 6, 10));
                draw_line(x2, y2, x3, y3, RGB5(4, 6, 10));

                // Spinning 3D octahedron floating in mid-air
                int ttx, tty, btx, bty;
                // Top and bottom tips
                project(2 * x + 1, 2 * y + 1, 10, &ttx, &tty);
                project(2 * x + 1, 2 * y + 1, 0,  &btx, &bty);

                // Compute 4 rotated middle vertices in X-Z space (radius 6)
                int mx[4], my[4];
                for (int i = 0; i < 4; i++) {
                    int idx = (rot_idx + i * 2) % 8;
                    int dx = (cos_tbl[idx] * 7) / 256;
                    int dz = (sin_tbl[idx] * 7) / 256;

                    // Direct 3D calculations for offset point
                    int px_3d = (2 * x + 1 - 14) * 10 + dx;
                    int pz_3d = 170 - ((2 * y + 1) * 11) / 2 + dz;
                    int py_3d = 45 - 5; // Float height 5

                    mx[i] = 120 + (px_3d * 120) / pz_3d;
                    my[i] = 75 + (py_3d * 120) / pz_3d;
                }

                unsigned short core_color = RGB5(0, 31, 31);   // Glowing Cyan
                unsigned short inner_color = RGB5(31, 31, 31); // White ring

                // Draw links from tips to middle vertices
                for (int i = 0; i < 4; i++) {
                    draw_line(ttx, tty, mx[i], my[i], core_color);
                    draw_line(btx, bty, mx[i], my[i], core_color);
                }
                // Draw middle ring
                draw_line(mx[0], my[0], mx[1], my[1], inner_color);
                draw_line(mx[1], my[1], mx[2], my[2], inner_color);
                draw_line(mx[2], my[2], mx[3], my[3], inner_color);
                draw_line(mx[3], my[3], mx[0], my[0], inner_color);

            } else {
                // Captured tiles: TILE_CAPTURED_P1 to P4
                unsigned short bright_color = 0;
                unsigned short med_color = 0;
                unsigned short dark_color = 0;
                int p_idx = tile - TILE_CAPTURED_P1;

                switch (p_idx) {
                    case 0: // P1 Red
                        bright_color = RGB5(31, 6, 6);
                        med_color    = RGB5(16, 2, 2);
                        dark_color   = RGB5(6, 0, 0);
                        break;
                    case 1: // P2 Blue
                        bright_color = RGB5(6, 6, 31);
                        med_color    = RGB5(2, 2, 16);
                        dark_color   = RGB5(0, 0, 6);
                        break;
                    case 2: // P3 Green
                        bright_color = RGB5(6, 31, 6);
                        med_color    = RGB5(2, 16, 2);
                        dark_color   = RGB5(0, 6, 0);
                        break;
                    case 3: // P4 Yellow
                        bright_color = RGB5(31, 31, 6);
                        med_color    = RGB5(16, 16, 2);
                        dark_color   = RGB5(6, 6, 0);
                        break;
                }

                // Outer wireframe border
                draw_line(x0, y0, x1, y1, bright_color);
                draw_line(x1, y1, x2, y2, bright_color);
                draw_line(x2, y2, x3, y3, bright_color);
                draw_line(x3, y3, x0, y0, bright_color);

                // Nested inner concentric quadrilaterals (3D inset effect)
                int cx, cy;
                project(2 * x + 1, 2 * y + 1, 0, &cx, &cy);

                // Middle quad (66% size)
                int mx0 = (2 * x0 + cx) / 3, my0 = (2 * y0 + cy) / 3;
                int mx1 = (2 * x1 + cx) / 3, my1 = (2 * y1 + cy) / 3;
                int mx2 = (2 * x2 + cx) / 3, my2 = (2 * y2 + cy) / 3;
                int mx3 = (2 * x3 + cx) / 3, my3 = (2 * y3 + cy) / 3;
                draw_line(mx0, my0, mx1, my1, med_color);
                draw_line(mx1, my1, mx2, my2, med_color);
                draw_line(mx2, my2, mx3, my3, med_color);
                draw_line(mx3, my3, mx0, my0, med_color);

                // Inner quad (33% size)
                int ix0 = (x0 + 2 * cx) / 3, iy0 = (y0 + 2 * cy) / 3;
                int ix1 = (x1 + 2 * cx) / 3, iy1 = (y1 + 2 * cy) / 3;
                int ix2 = (x2 + 2 * cx) / 3, iy2 = (y2 + 2 * cy) / 3;
                int ix3 = (x3 + 2 * cx) / 3, iy3 = (y3 + 2 * cy) / 3;
                draw_line(ix0, iy0, ix1, iy1, dark_color);
                draw_line(ix1, iy1, ix2, iy2, dark_color);
                draw_line(ix2, iy2, ix3, iy3, dark_color);
                draw_line(ix3, iy3, ix0, iy0, dark_color);
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
