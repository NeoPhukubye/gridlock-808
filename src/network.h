#ifndef NETWORK_H
#define NETWORK_H

// GBA Hardware Registers for Serial Communication
#ifdef MOCK_GBA
extern volatile unsigned short mock_REG_SIOCNT;
extern volatile unsigned short mock_REG_SIOMLT_SEND;
extern volatile unsigned short mock_REG_RCNT;
extern volatile unsigned short mock_REG_SIOMULTI0;
extern volatile unsigned short mock_REG_SIOMULTI1;
extern volatile unsigned short mock_REG_SIOMULTI2;
extern volatile unsigned short mock_REG_SIOMULTI3;

#define REG_SIOCNT       mock_REG_SIOCNT
#define REG_SIOMLT_SEND  mock_REG_SIOMLT_SEND
#define REG_RCNT         mock_REG_RCNT
#define REG_SIOMULTI0    mock_REG_SIOMULTI0
#define REG_SIOMULTI1    mock_REG_SIOMULTI1
#define REG_SIOMULTI2    mock_REG_SIOMULTI2
#define REG_SIOMULTI3    mock_REG_SIOMULTI3
#else
#define REG_SIOCNT       *(volatile unsigned short*)0x04000128
#define REG_SIOMLT_SEND  *(volatile unsigned short*)0x0400012a
#define REG_RCNT         *(volatile unsigned short*)0x04000134
#define REG_SIOMULTI0    *(volatile unsigned short*)0x04000120
#define REG_SIOMULTI1    *(volatile unsigned short*)0x04000122
#define REG_SIOMULTI2    *(volatile unsigned short*)0x04000124
#define REG_SIOMULTI3    *(volatile unsigned short*)0x04000126

// GBA Mode 3 Framebuffer and pixel plot helper with safety clipping bounds
#define MEM_VRAM ((volatile unsigned short*)0x06000000)
static inline void m3_plot(int x, int y, unsigned short color) {
    if (x >= 0 && x < 240 && y >= 0 && y < 160) {
        MEM_VRAM[y * 240 + x] = color;
    }
}

// Fast integer-only Bresenham's line drawing algorithm
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

// 3D perspective projection helper using half-grid units with wider depth scaling
static inline void project(int gx_half, int gy_half, int height, int* sx, int* sy) {
    int x_3d = (gx_half - 14) * 10;     // Center is gx_half=14. Spacing is 10.
    int z_3d = 140 - (gy_half * 7) / 2; // gy_half=0 (far, z=140), gy_half=20 (near, z=70)
    int y_3d = 40 - height;             // Ground plane height

    // Projection centered at vanishing point (120, 80)
    *sx = 120 + (x_3d * 120) / z_3d;
    *sy = 80 + (y_3d * 120) / z_3d;
}
#endif

// SIO (Serial Input/Output) Control Bits
#define SIO_MODE_MULTIPLAYER 0x2000 // Multi-Play Mode (Mode 2)
#define SIO_START            0x0080 // Start Transfer (Parent only)
#define SIO_ERROR            0x0040 // Error Flag
#define SIO_SI_TERMINAL      0x0004 // SI Terminal: 0=Parent, 1=Child
#define SIO_SD_TERMINAL      0x0008 // SD Terminal: 1=Ready (all connected)
#define SIO_ID_MASK          0x0030 // Multiplayer ID bits
#define SIO_ID_SHIFT         4

// Player Input Bitmasks (matches GBA's REG_KEYINPUT)
#define KEY_A 1
#define KEY_B 2
#define KEY_SELECT 4
#define KEY_START 8
#define KEY_RIGHT 16
#define KEY_LEFT 32
#define KEY_UP 64
#define KEY_DOWN 128
#define KEY_R 256
#define KEY_L 512

// Network Link States
enum LinkState {
    LINK_STATE_INIT,
    LINK_STATE_READY,
    LINK_STATE_SYNCING,
    LINK_STATE_ACTIVE,
    LINK_STATE_ERROR
};

#endif // NETWORK_H
