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

class Player {
public:
    Player(int id);

    void update(unsigned short input, Grid& grid);
    void usePayload(Grid& grid);
    void render();
    void setPayload(Payload p);

    int getX() const;
    int getY() const;
    int getScore() const;
    int getPlayerId() const;
    bool isShielded() const;

private:
    int playerId;
    int x, y; // Position on the grid
    int score;
    Payload currentPayload;
    bool shielded;
    int glitchEffectTimer;
};

#endif // PLAYER_H
