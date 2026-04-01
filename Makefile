CXX = riscv-none-elf-g++
CC  = riscv-none-elf-gcc
AS  = riscv-none-elf-gcc
LD  = riscv-none-elf-g++

CXXFLAGS = -g -Wall -Wextra -std=c++20 -ffreestanding -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -march=rv64imac -mabi=lp64 -mcmodel=medany -msmall-data-limit=0
CFLAGS   = -g -Wall -Wextra -ffreestanding -march=rv64imac -mabi=lp64 -mcmodel=medany -msmall-data-limit=0
ASFLAGS  = -g -Wall -Wextra -ffreestanding -march=rv64imac -mabi=lp64 -mcmodel=medany -msmall-data-limit=0
LDFLAGS  = -nostdlib -march=rv64imac -mabi=lp64 -mcmodel=medany -msmall-data-limit=0 -T linker/kernel.ld -Wl,-Map=build/kernel.map

OBJCOPY = riscv-none-elf-objcopy
BIN_FILE = $(BUILD_DIR)/kernel.bin

BUILD_DIR = build
INCLUDES  = -Iinclude

KERNEL_ELF = $(BUILD_DIR)/kernel.elf

CPP_SRCS = $(wildcard src/kernel/*.cpp)
ASM_SRCS = $(wildcard src/arch/riscv/*.S)

CPP_OBJS = $(patsubst src/kernel/%.cpp,$(BUILD_DIR)/kernel/%.o,$(CPP_SRCS))
ASM_OBJS = $(patsubst src/arch/riscv/%.S,$(BUILD_DIR)/arch/riscv/%.o,$(ASM_SRCS))

OBJS = $(CPP_OBJS) $(ASM_OBJS)

all: build
	@echo Done!

build: setup $(KERNEL_ELF) $(BIN_FILE)

$(BIN_FILE): $(KERNEL_ELF)
	@echo Creating raw binary...
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(BIN_FILE)

setup:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(BUILD_DIR)/kernel" mkdir "$(BUILD_DIR)/kernel"
	@if not exist "$(BUILD_DIR)/arch" mkdir "$(BUILD_DIR)/arch"
	@if not exist "$(BUILD_DIR)/arch/riscv" mkdir "$(BUILD_DIR)/arch/riscv"

$(BUILD_DIR)/kernel/%.o: src/kernel/%.cpp
	@echo Compiling C++ $<...
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/arch/riscv/%.o: src/arch/riscv/%.S
	@echo Assembling $<...
	$(AS) $(ASFLAGS) $(INCLUDES) -c $< -o $@

$(KERNEL_ELF): $(OBJS)
	@echo Linking kernel...
	$(LD) $(OBJS) $(LDFLAGS) -o $@

clean:
	@echo Cleaning build output...
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"