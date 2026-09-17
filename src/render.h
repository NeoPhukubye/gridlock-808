#ifndef RENDER_H
#define RENDER_H

// GBA Mode 3/5 framebuffer
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160

// MEM_VRAM: either real hardware VRAM address (GBA) or a desktop array (mock)
#ifndef MEM_VRAM
  #ifdef MOCK_GBA
    extern volatile unsigned short MEM_VRAM[SCREEN_WIDTH * SCREEN_HEIGHT];
  #else
    #define MEM_VRAM ((volatile unsigned short*)0x06000000)
  #endif
#endif

// Mode 5 page flipping (160x128 double-buffered)
#define PAGE_SIZE 0xA000
#define VRAM_PAGE_0 ((volatile unsigned short*)0x06000000)
#define VRAM_PAGE_1 ((volatile unsigned short*)0x0600A000)

// RGB color macro
#ifndef RGB5
#define RGB5(r, g, b) ((r) | ((g) << 5) | ((b) << 10))
#endif

// Plot a single pixel with bounds clipping
static inline void m3_plot(int x, int y, unsigned short color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        MEM_VRAM[y * SCREEN_WIDTH + x] = color;
    }
}

// Fast Bresenham line drawing
static inline void draw_line(int x0, int y0, int x1, int y1, unsigned short color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = adx - ady;

    while (1) {
        m3_plot(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -ady) {
            err -= ady;
            x0 += sx;
        }
        if (e2 < adx) {
            err += adx;
            y0 += sy;
        }
    }
}

// 3D perspective projection (half-grid units)
static inline void project(int gx_half, int gy_half, int height, int* sx, int* sy) {
    int x_3d = (gx_half - 14) * 10;
    int z_3d = 140 - (gy_half * 7) / 2;
    int y_3d = 40 - height;

    *sx = 120 + (x_3d * 120) / z_3d;
    *sy = 80 + (y_3d * 120) / z_3d;
}

// Draw a filled rectangle (for HUD backgrounds, title elements)
static inline void draw_rect_fill(int x, int y, int w, int h, unsigned short color) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            m3_plot(px, py, color);
        }
    }
}

// Draw a rectangle outline
static inline void draw_rect(int x, int y, int w, int h, unsigned short color) {
    draw_line(x, y, x + w - 1, y, color);
    draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
    draw_line(x + w - 1, y + h - 1, x, y + h - 1, color);
    draw_line(x, y + h - 1, x, y, color);
}

#endif // RENDER_H
