#
# Makefile for gridlock-808
#
# This file is a part of the gridlock-808 project.
#

# The name of your desired target file (e.g., gridlock-808.gba)
TARGET := gridlock-808

# The directory where source files are located
SRCDIR := src

# The list of C++ source files
CPP_FILES := main.cpp link.cpp player.cpp grid.cpp
SRCS := $(addprefix $(SRCDIR)/, $(CPP_FILES))

# The list of object files
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

# Build flags
ARCH := -mthumb -mthumb-interwork
SPECS := -specs=gba.specs
CFLAGS := $(ARCH) -O2 -Wall -fomit-frame-pointer
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS := $(SPECS) $(ARCH) -Wl,-Map,$(TARGET).map

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
