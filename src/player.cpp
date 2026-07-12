#include "player.h"
#include "grid.h"
#include <gba_video.h>

Player::Player(int id) : playerId(id), x(0), y(0), score(0), currentPayload(PAYLOAD_NONE), shielded(false), glitchEffectTimer(0) {
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
        if (input & KEY_UP) newY++;
        if (input & KEY_DOWN) newY--;
        if (input & KEY_LEFT) newX++;
        if (input & KEY_RIGHT) newX--;
    } else {
        if (input & KEY_UP) newY--;
        if (input & KEY_DOWN) newY++;
        if (input & KEY_LEFT) newX--;
        if (input & KEY_RIGHT) newX++;
    }

    if (grid.isMoveValid(newX, newY)) {
        x = newX;
        y = newY;
    }

    if (input & KEY_A) {
        usePayload(grid);
    }
}

void Player::usePayload(Grid& grid) {
    switch (currentPayload) {
        case PAYLOAD_FIREWALL:
            // Place a firewall in the direction the player is facing
            // This is a simplified implementation
            grid.setTile(x, y - 1, TILE_FIREWALL);
            break;
        case PAYLOAD_GLITCH:
            // In a real multiplayer game, this would send a glitch packet
            // to other players. For now, it does nothing.
            break;
        case PAYLOAD_SHIELD:
            shielded = true;
            break;
        case PAYLOAD_NONE:
            break;
    }
    currentPayload = PAYLOAD_NONE; // Use payload only once
}

void Player::render() {
    // Placeholder to render the player as a pixel on the screen
    // This would be replaced with sprite rendering in a real game
    unsigned short player_color = 0;
    switch(playerId) {
        case 0: player_color = RGB5(31, 0, 0); break; // Red
        case 1: player_color = RGB5(0, 0, 31); break; // Blue
        case 2: player_color = RGB5(0, 31, 0); break; // Green
        case 3: player_color = RGB5(31, 31, 0); break; // Yellow
    }
    m3_plot(x * 16, y * 16, player_color);
}

void Player::setPayload(Payload p) {
    currentPayload = p;
}

int Player::getX() const { return x; }
int Player::getY() const { return y; }
int Player::getScore() const { return score; }
int Player::getPlayerId() const { return playerId; }
bool Player::isShielded() const { return shielded; }
