#ifndef MOCK_GBA_VIDEO_H
#define MOCK_GBA_VIDEO_H

#define MODE_3 0x0003
#define BG2_ENABLE 0x0400

extern volatile unsigned short REG_DISPCNT;
extern volatile unsigned short MEM_VRAM[240 * 160];

#define RGB5(r,g,b) (((r) & 31) | (((g) & 31) << 5) | (((b) & 31) << 10))

#endif // MOCK_GBA_VIDEO_H