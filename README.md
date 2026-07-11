====================================================================
  _______ _____  _____ _____  _      ____   _____ _  __   ___   ___   ___ 
 / ____  |  __ \|_   _|  __ \| |    / __ \ / ____| |/ /  / _ \ / _ \ / _ 
| |  __| | |__) | | | | |  | | |   | |  | | |    | ' /  | (_) | (_) | (_) |
| | |_ | |  _  /  | | | |  | | |   | |  | | |    |  <    > _ < > _ < > _ <
| |__| | | | \ \ _| |_| |__| | |___| |__| | |____| . \  | (_) | (_) | (_) |
 \_____|_|_|  \_\_____|_____/|______\____/ \_____|_|\_\  \___/ \___/ \___/
====================================================================
           CYBERPUNK TERMINAL HACKING COMPETITION // GBA JAM 2026

    A high-energy, multiplayer cyberpunk hacking race built from scratch for the Game Boy Advance console's 25th anniversary.  

gridlock-808 is an ultra-fast, grid-based competitive arcade game built under the strict constraints of the GBA Jam 2026. Players connect via the classic GBA Game Link Cable (Multi-Pak mode) to battle head-to-head in a shifting neon terminal, racing to seize data nodes and lock down subnets before their code gets purged.  
🕹️ Gameplay & Architecture

Think Bomberman meets TRON inside a localized network grid.

    The Matrix: 2-4 players navigate a dynamic 15×10 tile matrix perfectly scaled to the GBA's 240×160 screen resolution.

    Node Capture: Secure neutral data tiles across the board to grow your network presence and drive up your payload score.

    Offensive Payloads:

        Firewalls: Drop rigid network blocks to drop-trap opponents or cut off pathways.

        Glitch Packets: Inject localized disruptions to temporarily flip your opponent's D-pad directions.

        Surge Shields: Deploy momentary hardware protections to deflect incoming terminal spikes.

🛠️ Technical Profile

This project is built directly down to raw hardware with zero AI-generated code, art, or audio, focusing entirely on strict, authentic retro-engineering:  

    Language Stack: C++ utilizing highly clean, memory-efficient Object-Oriented Design patterns.

    Toolchain: Compiled via arm-none-eabi-gcc using the industry-standard devkitPro development payload.  

    Deterministic Input Syncing: To preserve VRAM and CPU cycles on a link connection, the architecture uses low-latency hardware synchronization via the GBA's Serial Communication registers (REG_SIOCNT Multi-Play Mode). The network state engine exchanges only raw player buttons each frame, executing perfectly mirrored game states across all connected units.

    Graphics Infrastructure: Pushes authentic 2D rendering optimized around hardware sprite palettes, background tilemaps, and strict VRAM memory layouts.

📂 Project Structure

├── src/
│   ├── main.cpp         # Hardware initialization & central system loop
│   ├── link.cpp         # Low-level serial link data transfer & synchronization
│   ├── network.h        # Network constants, bitmask definitions, and link states
│   ├── player.cpp       # Player class logic, state machines, and inventory tracking
│   ├── player.h         
│   ├── grid.cpp         # 2D array grid validation, node metrics, and collision maps
│   └── grid.h          
├── graphics/            # RAW asset source sheets (Sprites, Background tiles)
├── Makefile             # Tailored devkitPro build configuration script
└── README.md

🚀 Getting Started
Prerequisites

You need the devkitPro environment initialized on your desktop path with the GBA development suite installed.
Building from Source

    Clone the repository:
    Bash

    git clone https://github.com/YOUR_USERNAME/gridlock-808.git
    cd gridlock-808

    Compile the binary:
    Bash

    make

    This generates a fully optimized gridlock-808.gba file ready to boot.  

Emulation & Testing

Run the output ROM on real hardware using a modern flash cartridge, or launch it via mGBA. For multiplayer network debugging, open multiple instances of mGBA and select File -> New Multiplayer Window to simulate the local Game Link hardware directly on your system.  
📝 License & Submission Profile

    Jam: GBA Jam 2026  

    Submission Format: Pure .gba binary file  

    Content Rule Compliance: 100% human-authored logic, graphics, and audio track integration..
