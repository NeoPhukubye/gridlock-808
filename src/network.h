#ifndef NETWORK_H
#define NETWORK_H

// GBA Hardware Registers for Serial Communication
#define REG_SIOCNT *(volatile unsigned short*)0x4000128
#define REG_SIODATA8 *(volatile unsigned char*)0x400012a
#define REG_RCNT *(volatile unsigned short*)0x4000134

// SIO (Serial Input/Output) Control Bits
#define SIO_MODE_MULTIPLAYER 0x3000 // Multi-Play Mode
#define SIO_START 0x0080           // Start Transfer
#define SIO_RDY 0x0004             // Ready Flag
#define SIO_ERROR 0x0040           // Error Flag

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
