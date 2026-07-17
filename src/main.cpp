#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "render.h"
#include "audio.h"
#include "game_state.h"
#include "grid.h"
#include "player.h"
#include "link.h"
#include "text.h"

// VBlank register
#define REG_VCOUNT (*(volatile unsigned short*)0x04000006)

static GameState gameState = STATE_TITLE;
static int stateTimer = 0;
static int gameTimer = 0;
static int num_players = 1;
static int local_id = 0;

static Grid grid;
static Player players[4] = {Player(0), Player(1), Player(2), Player(3)};

// Seed the RNG from an unpredictable source (frame timing + VCOUNT)
static void seed_rng() {
    unsigned int seed = 0;
    for (volatile int i = 0; i < 100; i++) {
        seed ^= REG_VCOUNT;
        seed = (seed << 3) | (seed >> 29);
    }
    srand(seed);
}

static void vblank_wait() {
    while (REG_VCOUNT >= 160);
    while (REG_VCOUNT < 160);
}

static void render_title() {
    // Dark background with scanline effect
    for (int y = 0; y < 160; y++) {
        unsigned short bg = (y % 2 == 0) ? RGB5(1, 0, 2) : RGB5(0, 0, 1);
        for (int x = 0; x < 240; x++) {
            MEM_VRAM[y * 240 + x] = bg;
        }
    }

    // Neon grid lines (perspective floor)
    for (int i = 0; i < 8; i++) {
        int y_line = 100 + i * 8;
        if (y_line < 160) {
            draw_line(0, y_line, 239, y_line, RGB5(4, 0, 8));
        }
    }
    for (int i = 0; i < 12; i++) {
        int x_start = 120 + (i - 6) * 5;
        int x_end = 120 + (i - 6) * 30;
        draw_line(x_start, 95, x_end, 159, RGB5(4, 0, 8));
    }

    // Horizon line
    draw_line(0, 95, 239, 95, RGB5(20, 0, 20));

    // Title text
    draw_text(52, 20, "GRIDLOCK", RGB5(0, 31, 31));
    draw_text(88, 32, "808", RGB5(31, 0, 20));

    // Blinking prompt
    if ((stateTimer / 30) % 2 == 0) {
        draw_text(52, 70, "PRESS START", RGB5(31, 31, 31));
    }

    // Credits
    draw_text(44, 145, "GBA JAM 2026", RGB5(10, 10, 15));
}

static void render_waiting_link() {
    memset((void*)0x06000000, 0, 240 * 160 * 2);
    draw_text(40, 60, "SCANNING", RGB5(0, 31, 31));
    draw_text(60, 75, "NETWORK", RGB5(0, 31, 31));

    // Animated dots
    int dots = (stateTimer / 20) % 4;
    char dot_str[5] = "    ";
    for (int i = 0; i < dots; i++) dot_str[i] = '.';
    draw_text(140, 75, dot_str, RGB5(0, 31, 31));

    char pcount[12];
    sprintf(pcount, "P:%d", num_players);
    draw_text(100, 100, pcount, RGB5(31, 31, 31));

    if (stateTimer > 120) {
        draw_text(36, 130, "START: BEGIN", RGB5(20, 20, 20));
    }
}

static void render_countdown() {
    memset((void*)0x06000000, 0, 240 * 160 * 2);
    int seconds_left = COUNTDOWN_SECONDS - (stateTimer / 60);
    if (seconds_left <= 0) seconds_left = 1;

    char count_str[4];
    sprintf(count_str, "%d", seconds_left);
    draw_text(112, 70, count_str, RGB5(31, 31, 0));
}

static void render_game_over() {
    // Dim the screen by overlaying dark pixels on odd lines
    for (int y = 0; y < 160; y += 2) {
        for (int x = 0; x < 240; x += 2) {
            m3_plot(x, y, RGB5(0, 0, 0));
        }
    }

    draw_text(60, 40, "GAME OVER", RGB5(31, 0, 10));

    // Find winner
    int best_score = -1;
    int winner = 0;
    for (int i = 0; i < num_players; i++) {
        if (players[i].getScore() > best_score) {
            best_score = players[i].getScore();
            winner = i;
        }
    }

    char win_str[20];
    sprintf(win_str, "P%d WINS", winner + 1);
    draw_text(80, 60, win_str, RGB5(0, 31, 31));

    // Show all scores
    for (int i = 0; i < num_players; i++) {
        char score_str[20];
        sprintf(score_str, "P%d: %d", i + 1, players[i].getScore());
        unsigned short color = (i == winner) ? RGB5(31, 31, 0) : RGB5(20, 20, 20);
        draw_text(80, 85 + i * 12, score_str, color);
    }

    if (stateTimer > 180 && (stateTimer / 30) % 2 == 0) {
        draw_text(44, 140, "START: RETRY", RGB5(31, 31, 31));
    }
}

static void render_hud() {
    // Timer (top-left area, below scores)
    int seconds_left = (GAME_DURATION_FRAMES - gameTimer) / 60;
    if (seconds_left < 0) seconds_left = 0;
    int mins = seconds_left / 60;
    int secs = seconds_left % 60;

    char time_str[10];
    sprintf(time_str, "%d:%02d", mins, secs);
    unsigned short time_color = (seconds_left <= 10) ? RGB5(31, 0, 0) : RGB5(20, 20, 20);
    draw_text(10, num_players * 10 + 4, time_str, time_color);

    // Payload indicator (bottom-left)
    const char* payload_name = "NONE";
    unsigned short payload_color = RGB5(10, 10, 10);
    switch (players[local_id].getPayload()) {
        case PAYLOAD_FIREWALL:
            payload_name = "FIRE";
            payload_color = RGB5(31, 8, 0);
            break;
        case PAYLOAD_GLITCH:
            payload_name = "GLTCH";
            payload_color = RGB5(20, 0, 31);
            break;
        case PAYLOAD_SHIELD:
            payload_name = "SHLD";
            payload_color = RGB5(0, 31, 31);
            break;
        default:
            break;
    }
    draw_text(10, 148, payload_name, payload_color);

    // Link status indicator (top-right, near minimap)
    if (num_players > 1) {
        LinkState ls = link_get_state();
        if (ls == LINK_STATE_ACTIVE) {
            m3_plot(235, 2, RGB5(0, 31, 0));
            m3_plot(236, 2, RGB5(0, 31, 0));
        } else if (ls == LINK_STATE_ERROR) {
            m3_plot(235, 2, RGB5(31, 0, 0));
            m3_plot(236, 2, RGB5(31, 0, 0));
        }
    }
}

static void reset_game() {
    grid = Grid();
    for (int i = 0; i < 4; i++) {
        players[i] = Player(i);
    }
    gameTimer = 0;

    // Place additional starting nodes
    for (int n = 0; n < NODE_COUNT_INITIAL - 1; n++) {
        int rx = rand() % (GRID_WIDTH - 4) + 2;
        int ry = rand() % (GRID_HEIGHT - 4) + 2;
        grid.setTile(rx, ry, TILE_NODE);
    }
}

int main() {
    REG_DISPCNT = MODE_3 | BG2_ENABLE;
    audio_init();
    seed_rng();

    while (1) {
        vblank_wait();
        stateTimer++;

        scanKeys();
        unsigned short pressed = keysDown();
        unsigned short held = keysHeld();

        switch (gameState) {
        case STATE_TITLE:
            audio_update();
            render_title();

            if (pressed & KEY_START) {
                link_init();
                gameState = STATE_WAITING_LINK;
                stateTimer = 0;
            }
            break;

        case STATE_WAITING_LINK:
            num_players = link_get_num_players();
            local_id = link_get_player_id();
            render_waiting_link();

            if (pressed & KEY_START) {
                reset_game();
                gameState = STATE_COUNTDOWN;
                stateTimer = 0;
            }
            break;

        case STATE_COUNTDOWN: {
            render_countdown();
            int prev_sec = (stateTimer - 1) / 60;
            int curr_sec = stateTimer / 60;
            if (curr_sec != prev_sec) {
                play_countdown_beep();
            }
            if (stateTimer >= COUNTDOWN_SECONDS * 60) {
                gameState = STATE_PLAYING;
                stateTimer = 0;
                gameTimer = 0;
            }
            break;
        }

        case STATE_PLAYING: {
            // Clear screen
            memset((void*)0x06000000, 0, 240 * 160 * 2);
            audio_update();
            gameTimer++;

            // Check win condition
            if (gameTimer >= GAME_DURATION_FRAMES) {
                gameState = STATE_GAME_OVER;
                stateTimer = 0;
                play_game_over_sound();
                break;
            }

#ifdef DEBUG
            // Developer cheats (only in debug builds)
            if (held & KEY_SELECT) {
                if (pressed & KEY_R) {
                    players[local_id].toggleInfiniteShield();
                    play_payload_sound();
                }
                if (pressed & KEY_L) {
                    players[local_id].toggleHyperSpeed();
                    play_payload_sound();
                }
            }
#endif

            // Movement input with smooth repeating
            static int move_delay = 0;
            unsigned short my_input = 0;

            if (held & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)) {
                if (move_delay == 0) {
                    my_input = held & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT);
                    move_delay = 8;
                } else {
                    move_delay--;
                }
            } else {
                move_delay = 0;
            }

            if (pressed & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT)) {
                my_input = pressed & (KEY_UP | KEY_DOWN | KEY_LEFT | KEY_RIGHT);
                move_delay = 8;
            }

            my_input |= (pressed & KEY_A);

            // Sync with other players
            unsigned short inputs[4];
            num_players = link_sync_inputs(my_input, inputs);

            // Update all players
            for (int i = 0; i < num_players; ++i) {
                players[i].update(inputs[i], grid);

                if (inputs[i] & KEY_A) {
                    players[i].usePayload(grid, players, num_players);
                }

                // Tile interactions
                TileType currentTile = grid.getTile(players[i].getX(), players[i].getY());
                if (currentTile == TILE_NODE) {
                    play_payload_sound();
                    players[i].setPayload((Payload)(rand() % 3 + 1));
                    grid.setTile(players[i].getX(), players[i].getY(), TILE_NEUTRAL);
                    // Respawn node at random valid location
                    int rx, ry;
                    do {
                        rx = rand() % (GRID_WIDTH - 2) + 1;
                        ry = rand() % (GRID_HEIGHT - 2) + 1;
                    } while (grid.getTile(rx, ry) != TILE_NEUTRAL);
                    grid.setTile(rx, ry, TILE_NODE);
                } else if (currentTile == TILE_NEUTRAL) {
                    play_capture_sound();
                    grid.setTile(players[i].getX(), players[i].getY(), (TileType)(TILE_CAPTURED_P1 + i));
                    players[i].increaseScore(10);
                }
            }

            // Update grid (tile decay, etc.)
            grid.update();

            // Render
            grid.render();
            for (int i = 0; i < num_players; ++i) {
                players[i].render();
            }

            // Display scores
            char score_str[20];
            for (int i = 0; i < num_players; ++i) {
                if (i == local_id && num_players > 1) {
                    sprintf(score_str, "P%d*:%d", i + 1, players[i].getScore());
                    draw_text(10, i * 10, score_str, RGB5(10, 31, 10));
                } else {
                    sprintf(score_str, "P%d: %d", i + 1, players[i].getScore());
                    draw_text(10, i * 10, score_str, RGB5(31, 31, 31));
                }
            }

            render_hud();
            break;
        }

        case STATE_GAME_OVER:
            render_game_over();
            if (pressed & KEY_START) {
                reset_game();
                gameState = STATE_COUNTDOWN;
                stateTimer = 0;
            }
            break;
        }
    }

    return 0;
}
