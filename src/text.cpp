#include "text.h"
#include "render.h"
#include <gba_video.h>
#include <string.h>

// A very simple font (e.g., 8x8) would be needed here.
// For the sake of simplicity, this function will not render real text,
// but will just simulate it by drawing colored blocks.
// A real implementation would have a bitmap font.

static const unsigned char* get_char_bitmap(char c) {
    static const unsigned char space[]    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    static const unsigned char asterisk[] = {0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, 0x00, 0x00};
    static const unsigned char colon[]    = {0x00, 0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00};
    static const unsigned char zero[]     = {0x3C, 0x66, 0x6E, 0x76, 0x66, 0x66, 0x3C, 0x00};
    static const unsigned char one[]      = {0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00};
    static const unsigned char two[]      = {0x3C, 0x66, 0x06, 0x0C, 0x18, 0x30, 0x7E, 0x00};
    static const unsigned char three[]    = {0x3C, 0x66, 0x06, 0x1C, 0x06, 0x66, 0x3C, 0x00};
    static const unsigned char four[]     = {0x0C, 0x1C, 0x3C, 0x6C, 0x7E, 0x0C, 0x0C, 0x00};
    static const unsigned char five[]     = {0x7E, 0x60, 0x7C, 0x06, 0x06, 0x66, 0x3C, 0x00};
    static const unsigned char six[]      = {0x3C, 0x66, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00};
    static const unsigned char seven[]    = {0x7E, 0x66, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x00};
    static const unsigned char eight[]    = {0x3C, 0x66, 0x66, 0x3C, 0x66, 0x66, 0x3C, 0x00};
    static const unsigned char nine[]     = {0x3C, 0x66, 0x66, 0x3E, 0x06, 0x66, 0x3C, 0x00};
    static const unsigned char p_char[]   = {0x7C, 0x66, 0x66, 0x7C, 0x60, 0x60, 0x60, 0x00};
    
    switch (c) {
        case ' ': return space;
        case '*': return asterisk;
        case ':': return colon;
        case '0': return zero;
        case '1': return one;
        case '2': return two;
        case '3': return three;
        case '4': return four;
        case '5': return five;
        case '6': return six;
        case '7': return seven;
        case '8': return eight;
        case '9': return nine;
        case 'P': case 'p': return p_char;
        default: return space;
    }
}

void draw_text(int x, int y, const char* str, unsigned short color) {
    int len = strlen(str);
    
    // Draw a dark semi-transparent/solid background box for high legibility
    for (int px = 0; px < len * 8 + 4; px++) {
        for (int py = 0; py < 10; py++) {
            m3_plot(x - 2 + px, y - 1 + py, RGB5(2, 2, 4));
        }
    }
    
    for (int i = 0; i < len; i++) {
        const unsigned char* bitmap = get_char_bitmap(str[i]);
        for (int row = 0; row < 8; row++) {
            unsigned char data = bitmap[row];
            for (int col = 0; col < 8; col++) {
                if (data & (0x80 >> col)) {
                    m3_plot(x + i * 8 + col, y + row, color);
                }
            }
        }
    }
}
