#ifndef MOCK_GBA_VIDEO_H
#define MOCK_GBA_VIDEO_H

#define MODE_3 0x0003
#define BG2_ENABLE 0x0400

extern volatile unsigned short REG_DISPCNT;

#define RGB5(r,g,b) (((r) & 31) | (((g) & 31) << 5) | (((b) & 31) << 10))

inline void m3_plot(int x, int y, unsigned short color) {
    // Mock pixel plot - headless no-op for tests
}

#endif // MOCK_GBA_VIDEO_H
