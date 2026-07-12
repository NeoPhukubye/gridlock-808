#ifndef GRID_H
#define GRID_H

#define GRID_WIDTH 15
#define GRID_HEIGHT 10

// Tile types
enum TileType {
    TILE_NEUTRAL,
    TILE_CAPTURED_P1,
    TILE_CAPTURED_P2,
    TILE_CAPTURED_P3,
    TILE_CAPTURED_P4,
    TILE_FIREWALL,
    TILE_NODE
};

class Grid {
public:
    Grid();

    void update();
    void render();

    bool isMoveValid(int x, int y) const;
    void setTile(int x, int y, TileType type);
    TileType getTile(int x, int y) const;

private:
    TileType tiles[GRID_WIDTH][GRID_HEIGHT];
};

#endif // GRID_H
