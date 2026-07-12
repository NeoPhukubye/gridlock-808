#
# Makefile for gridlock-808
#
# This file is a part of the gridlock-808 project.
#

# Default devkitPro path if not set in the environment
DEVKITPRO ?= /opt/devkitpro

# The name of your desired target file (e.g., gridlock-808.gba)
TARGET := gridlock-808

# The directory where source files are located
SRCDIR := src

# The list of C++ source files
CPP_FILES := main.cpp link.cpp player.cpp grid.cpp text.cpp
SRCS := $(addprefix $(SRCDIR)/, $(CPP_FILES))

# The list of object files
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

# Build flags
ARCH := -mthumb -mthumb-interwork
CFLAGS := $(ARCH) -O2 -Wall -fomit-frame-pointer -I$(DEVKITPRO)/libgba/include
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS := $(ARCH) -T$(DEVKITPRO)/devkitARM/arm-none-eabi/lib/gba_cart.ld -Wl,-Map,$(TARGET).map -L$(DEVKITPRO)/libgba/lib -lgba -lc -lgcc -lnosys

# The toolchain prefix
PREFIX := arm-none-eabi-

# The tools
CC := $(PREFIX)gcc
CXX := $(PREFIX)g++
LD := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy

# The final GBA file
GBA_FILE := $(TARGET).gba
ELF_FILE := $(TARGET).elf

.PHONY: all clean

all: $(GBA_FILE)

$(GBA_FILE): $(ELF_FILE)
	@echo "Creating GBA file..."
	@$(OBJCOPY) -v -O binary $(ELF_FILE) $(GBA_FILE)
	@echo "Done."

$(ELF_FILE): $(OBJS)
	@echo "Linking..."
	@$(LD) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -f $(SRCDIR)/*.o $(ELF_FILE) $(GBA_FILE) $(TARGET).map
	@echo "Done."

# Phony targets for running the game
run: $(GBA_FILE)
	@echo "Running in mGBA..."
	@mgba $(GBA_FILE)

# Phony target for debugging
debug: $(GBA_FILE)
	@echo "Running in mGBA with debugging enabled..."
	@mgba -d $(GBA_FILE)
