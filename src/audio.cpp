#include "audio.h"

// GBA PSG Audio registers
#define REG_SOUNDCNT_X  *(volatile unsigned short*)0x04000084
#define REG_SOUNDCNT_L  *(volatile unsigned short*)0x04000080
#define REG_SOUNDCNT_H  *(volatile unsigned short*)0x04000082

// Channel 1 (Square wave with sweep)
#define REG_SOUND1CNT_L *(volatile unsigned short*)0x04000060
#define REG_SOUND1CNT_H *(volatile unsigned short*)0x04000062
#define REG_SOUND1CNT_X *(volatile unsigned short*)0x04000064

// Channel 2 (Square wave)
#define REG_SOUND2CNT_L *(volatile unsigned short*)0x04000068
#define REG_SOUND2CNT_X *(volatile unsigned short*)0x0400006c

// Channel 4 (Noise)
#define REG_SOUND4CNT_L *(volatile unsigned short*)0x04000078
#define REG_SOUND4CNT_H *(volatile unsigned short*)0x0400007c

static int music_timer = 0;
static int music_index = 0;

void audio_init() {
    REG_SOUNDCNT_X = 0x0080; // Master Sound Enable
    REG_SOUNDCNT_L = 0xFF77; // Stereo, max volume, all channels enabled
    REG_SOUNDCNT_H = 0x0002; // PSG mix ratio 100%
}

void audio_update() {
    music_timer++;
    if (music_timer >= 12) { // ~120 BPM at 60fps
        music_timer = 0;

        // Cyberpunk synthwave bass: A3, A3, C4, A3, D4, A3, C4, G3
        static const unsigned short bass_notes[] = {1452, 1452, 1547, 1452, 1602, 1452, 1547, 1380};
        unsigned short note = bass_notes[music_index];
        music_index = (music_index + 1) % 8;

        REG_SOUND1CNT_H = 0x6420; // Volume 6, decay 2
        REG_SOUND1CNT_X = 0xC000 | note;
    }
}

void play_move_sound() {
    REG_SOUND2CNT_L = 0x8100; // Volume 8, fast decay
    REG_SOUND2CNT_X = 0xC550; // Short cyber-chirp
}

void play_capture_sound() {
    REG_SOUND4CNT_L = 0xF100; // Volume F, fast decay
    REG_SOUND4CNT_H = 0xC030; // Digital noise crunch
}

void play_payload_sound() {
    REG_SOUND2CNT_L = 0xD200; // Volume D, medium decay
    REG_SOUND2CNT_X = 0xC700; // Higher chime
}

void play_countdown_beep() {
    REG_SOUND2CNT_L = 0xA300; // Volume A, medium decay
    REG_SOUND2CNT_X = 0xC600; // Mid-range beep
}

void play_game_over_sound() {
    REG_SOUND1CNT_H = 0xF800; // Volume F, slow decay
    REG_SOUND1CNT_X = 0xC400; // Low drone
}
