#include "player.h"
#include "grid.h"
#include <gba_video.h>

Player::Player(int id) : playerId(id), x(0), y(0), score(0), currentPayload(PAYLOAD_NONE), shielded(false), shieldTimer(0), glitchEffectTimer(0), lastDirection(DIR_DOWN) {
    // Initialize player position based on ID
    switch (playerId) {
        case 0: x = 1; y = 1; break;
        case 1: x = GRID_WIDTH - 2; y = 1; break;
        case 2: x = 1; y = GRID_HEIGHT - 2; break;
        case 3: x = GRID_WIDTH - 2; y = GRID_HEIGHT - 2; break;
    }
}

void Player::update(unsigned short input, Grid& grid) {
    int newX = x;
    int newY = y;

    if (glitchEffectTimer > 0) {
        glitchEffectTimer--;
        // Invert controls during glitch
        if (input & KEY_UP) { newY++; lastDirection = DIR_UP; }
        if (input & KEY_DOWN) { newY--; lastDirection = DIR_DOWN; }
        if (input & KEY_LEFT) { newX++; lastDirection = DIR_LEFT; }
        if (input & KEY_RIGHT) { newX--; lastDirection = DIR_RIGHT; }
    } else {
        if (input & KEY_UP) { newY--; lastDirection = DIR_UP; }
        if (input & KEY_DOWN) { newY++; lastDirection = DIR_DOWN; }
        if (input & KEY_LEFT) { newX--; lastDirection = DIR_LEFT; }
        if (input & KEY_RIGHT) { newX++; lastDirection = DIR_RIGHT; }
    }

    if (shieldTimer > 0) {
        shieldTimer--;
        if (shieldTimer == 0) {
            shielded = false;
        }
    }

    if (grid.isMoveValid(newX, newY)) {
        x = newX;
        y = newY;
    }
}

void Player::usePayload(Grid& grid, Player players[], int num_players) {
    switch (currentPayload) {
        case PAYLOAD_FIREWALL: {
            int targetX = x;
            int targetY = y;
            switch (lastDirection) {
                case DIR_UP: targetY--; break;
                case DIR_DOWN: targetY++; break;
                case DIR_LEFT: targetX--; break;
                case DIR_RIGHT: targetX++; break;
            }
            if (grid.getTile(targetX, targetY) == TILE_NEUTRAL) {
                grid.setTile(targetX, targetY, TILE_FIREWALL);
            }
            break;
        }
        case PAYLOAD_GLITCH:
            for (int i = 0; i < num_players; ++i) {
                if (players[i].getPlayerId() != playerId) {
                    players[i].activateGlitch(120); // 2 seconds
                }
            }
            break;
        case PAYLOAD_SHIELD:
            shielded = true;
            shieldTimer = 180; // 3 seconds at 60fps
            break;
        case PAYLOAD_NONE:
            break;
    }
    currentPayload = PAYLOAD_NONE; // Use payload only once
}

void Player::render() {
    unsigned short color = 0;
    switch(playerId) {
        case 0: color = RGB5(31, 10, 10); break; // Glowing Red/Pink
        case 1: color = RGB5(10, 20, 31); break; // Glowing Blue
        case 2: color = RGB5(10, 31, 10); break; // Glowing Green
        case 3: color = RGB5(31, 31, 10); break; // Glowing Yellow
    }

    // Apply visual glitch shift if glitched
    int x_shift = 0;
    int y_shift = 0;
    if (glitchEffectTimer > 0) {
        x_shift = (glitchEffectTimer % 4 == 0) ? -2 : ((glitchEffectTimer % 4 == 2) ? 2 : 0);
        y_shift = (glitchEffectTimer % 3 == 0) ? -1 : ((glitchEffectTimer % 3 == 2) ? 1 : 0);
    }

    // Project base corners of a 3D pyramid (triangle on the ground)
    int bx0, by0, bx1, by1, bx2, by2;
    project(2 * x + 1, 2 * y,     0, &bx0, &by0); // Far-middle of cell
    project(2 * x,     2 * y + 2, 0, &bx1, &by1); // Near-left
    project(2 * x + 2, 2 * y + 2, 0, &bx2, &by2); // Near-right

    // Project top tip of the pyramid (height 8)
    int tx, ty;
    project(2 * x + 1, 2 * y + 1, 8, &tx, &ty);

    // Apply vibration shift
    bx0 += x_shift; by0 += y_shift;
    bx1 += x_shift; by1 += y_shift;
    bx2 += x_shift; by2 += y_shift;
    tx  += x_shift; ty  += y_shift;

    // Draw base triangle
    draw_line(bx0, by0, bx1, by1, color);
    draw_line(bx1, by1, bx2, by2, color);
    draw_line(bx2, by2, bx0, by0, color);

    // Draw vertical edges linking base corners to top tip
    draw_line(bx0, by0, tx, ty, color);
    draw_line(bx1, by1, tx, ty, color);
    draw_line(bx2, by2, tx, ty, color);

    // Draw thin vertical location beacon shooting up from the player avatar
    unsigned short beacon_color = 0;
    switch (playerId) {
        case 0: beacon_color = RGB5(12, 3, 3); break;  // Dim red
        case 1: beacon_color = RGB5(3, 6, 12); break;  // Dim blue
        case 2: beacon_color = RGB5(3, 12, 3); break;  // Dim green
        case 3: beacon_color = RGB5(12, 12, 3); break; // Dim yellow
    }
    draw_line(tx, ty, tx, 15, beacon_color);

    // Draw central white core dot
    int cx, cy;
    project(2 * x + 1, 2 * y + 1, 0, &cx, &cy);
    cx += x_shift;
    cy += y_shift;
    m3_plot(cx, cy, RGB5(31, 31, 31));

    // Draw player position on the minimap radar (2x2 dot)
    int rx_start = 190;
    int ry_start = 5;
    int mpx = rx_start + x * 3;
    int mpy = ry_start + y * 3;
    m3_plot(mpx, mpy, color);
    m3_plot(mpx + 1, mpy, color);
    m3_plot(mpx, mpy + 1, color);
    m3_plot(mpx + 1, mpy + 1, color);

    // Draw glowing cyan floating shield triangle if shielded
    if (shielded) {
        unsigned short shield_color = RGB5(0, 31, 31);
        int sx0, sy0, sx1, sy1, sx2, sy2;
        project(2 * x + 1, 2 * y - 1, 5, &sx0, &sy0);
        project(2 * x - 1, 2 * y + 3, 5, &sx1, &sy1);
        project(2 * x + 3, 2 * y + 3, 5, &sx2, &sy2);

        sx0 += x_shift; sy0 += y_shift;
        sx1 += x_shift; sy1 += y_shift;
        sx2 += x_shift; sy2 += y_shift;

        draw_line(sx0, sy0, sx1, sy1, shield_color);
        draw_line(sx1, sy1, sx2, sy2, shield_color);
        draw_line(sx2, sy2, sx0, sy0, shield_color);
    }
}

void Player::setPayload(Payload p) {
    currentPayload = p;
}

void Player::increaseScore(int amount) {
    score += amount;
}

void Player::activateGlitch(int duration) {
    if (!shielded) {
        glitchEffectTimer = duration;
    }
}

int Player::getX() const { return x; }
int Player::getY() const { return y; }
int Player::getScore() const { return score; }
int Player::getPlayerId() const { return playerId; }
bool Player::isShielded() const { return shielded; }
