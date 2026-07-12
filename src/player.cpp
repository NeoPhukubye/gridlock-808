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

    int px = x * 16;
    int py = y * 16;

    // Apply visual glitch shift if glitched
    int x_shift = 0;
    int y_shift = 0;
    if (glitchEffectTimer > 0) {
        // Shift left/right based on timer ticks to create a vibrating glitch effect
        x_shift = (glitchEffectTimer % 4 == 0) ? -2 : ((glitchEffectTimer % 4 == 2) ? 2 : 0);
        y_shift = (glitchEffectTimer % 3 == 0) ? -1 : ((glitchEffectTimer % 3 == 2) ? 1 : 0);
    }

    px += x_shift;
    py += y_shift;

    // 1. Draw corner brackets for the hacker avatar
    // Top-Left
    m3_plot(px + 2, py + 2, color);
    m3_plot(px + 3, py + 2, color);
    m3_plot(px + 4, py + 2, color);
    m3_plot(px + 2, py + 3, color);
    m3_plot(px + 2, py + 4, color);

    // Top-Right
    m3_plot(px + 13, py + 2, color);
    m3_plot(px + 12, py + 2, color);
    m3_plot(px + 11, py + 2, color);
    m3_plot(px + 13, py + 3, color);
    m3_plot(px + 13, py + 4, color);

    // Bottom-Left
    m3_plot(px + 2, py + 13, color);
    m3_plot(px + 3, py + 13, color);
    m3_plot(px + 4, py + 13, color);
    m3_plot(px + 2, py + 12, color);
    m3_plot(px + 2, py + 11, color);

    // Bottom-Right
    m3_plot(px + 13, py + 13, color);
    m3_plot(px + 12, py + 13, color);
    m3_plot(px + 11, py + 13, color);
    m3_plot(px + 13, py + 12, color);
    m3_plot(px + 13, py + 11, color);

    // 2. Draw crosshair center ticks
    m3_plot(px + 8, py + 5, color);
    m3_plot(px + 8, py + 6, color);
    m3_plot(px + 8, py + 9, color);
    m3_plot(px + 8, py + 10, color);
    m3_plot(px + 5, py + 8, color);
    m3_plot(px + 6, py + 8, color);
    m3_plot(px + 9, py + 8, color);
    m3_plot(px + 10, py + 8, color);

    // 3. Draw a solid central core (white)
    m3_plot(px + 7, py + 7, RGB5(31, 31, 31));
    m3_plot(px + 8, py + 7, RGB5(31, 31, 31));
    m3_plot(px + 7, py + 8, RGB5(31, 31, 31));
    m3_plot(px + 8, py + 8, RGB5(31, 31, 31));

    // 4. Draw circular shield ring if shielded
    if (shielded) {
        unsigned short shield_color = RGB5(0, 31, 31); // Glowing Cyan
        
        m3_plot(px + 8, py + 1, shield_color);
        m3_plot(px + 7, py + 1, shield_color);
        m3_plot(px + 9, py + 1, shield_color);
        
        m3_plot(px + 8, py + 14, shield_color);
        m3_plot(px + 7, py + 14, shield_color);
        m3_plot(px + 9, py + 14, shield_color);
        
        m3_plot(px + 1, py + 8, shield_color);
        m3_plot(px + 1, py + 7, shield_color);
        m3_plot(px + 1, py + 9, shield_color);
        
        m3_plot(px + 14, py + 8, shield_color);
        m3_plot(px + 14, py + 7, shield_color);
        m3_plot(px + 14, py + 9, shield_color);

        m3_plot(px + 3, py + 3, shield_color);
        m3_plot(px + 12, py + 3, shield_color);
        m3_plot(px + 3, py + 12, shield_color);
        m3_plot(px + 12, py + 12, shield_color);
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
