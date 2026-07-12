#ifndef PLAYER_H
#define PLAYER_H

#include "network.h"

// Forward declaration of the Grid class to avoid circular dependency
class Grid;

// Payload types
enum Payload {
    PAYLOAD_NONE,
    PAYLOAD_FIREWALL,
    PAYLOAD_GLITCH,
    PAYLOAD_SHIELD
};

// Player direction
enum Direction {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

class Player {
public:
    Player(int id);

    void update(unsigned short input, Grid& grid);
    void usePayload(Grid& grid, Player* players, int num_players);
    void render();
    void setPayload(Payload p);
    void increaseScore(int amount);

    int getX() const;
    int getY() const;
    int getScore() const;
    int getPlayerId() const;
    bool isShielded() const;
    void activateGlitch(int duration);

    void toggleInfiniteShield();
    void toggleHyperSpeed();
    bool isHyperSpeed() const;
    void teleportTo(int tx, int ty);

private:
    int playerId;
    int x, y; // Position on the grid
    int score;
    Payload currentPayload;
    bool shielded;
    int shieldTimer;
    int glitchEffectTimer;
    Direction lastDirection;
    bool infiniteShield;
    bool hyperSpeed;
};

#endif // PLAYER_H
