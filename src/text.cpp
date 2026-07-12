#include "text.h"
#include <gba_video.h>
#include <string.h>

// A very simple font (e.g., 8x8) would be needed here.
// For the sake of simplicity, this function will not render real text,
// but will just simulate it by drawing colored blocks.
// A real implementation would have a bitmap font.

void draw_text(int x, int y, const char* str, unsigned short color) {
    // This is a placeholder. A real implementation would draw characters from a bitmap font.
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < 8; k++) {
                m3_plot(x + i * 8 + k, y + j, color);
            }
        }
    }
}
