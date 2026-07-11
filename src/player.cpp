#include "player.h"

Player::Player(int id) : playerId(id), x(0), y(0), score(0), currentPayload(PAYLOAD_NONE), shielded(false), glitchEffectTimer(0) {
    // Initialize player position based on ID
    switch (playerId) {
        case 0: x = 1; y = 1; break;
        case 1: x = GRID_WIDTH - 2; y = 1; break;
        case 2: x = 1; y = GRID_HEIGHT - 2; break;
        case 3: x = GRID_WIDTH - 2; y = GRID_HEIGHT - 2; break;
    }
}

void Player::update(unsigned short input) {
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

    // A simple placeholder for grid validation
    // A proper implementation would use the Grid class
    if (newX >= 0 && newX < 15 && newY >= 0 && newY < 10) {
        x = newX;
        y = newY;
    }

    if (input & KEY_A) {
        usePayload();
    }
}

void Player::usePayload() {
    // Handle payload usage
}

int Player::getX() const { return x; }
int Player::getY() const { return y; }
int Player::getScore() const { return score; }
int Player::getPlayerId() const { return playerId; }
bool Player::isShielded() const { return shielded; }
