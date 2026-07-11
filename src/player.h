#ifndef PLAYER_H
#define PLAYER_H

#include "network.h"

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

    void update(unsigned short input);
    void usePayload();

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
