CXX = riscv-none-elf-g++
CC  = riscv-none-elf-gcc
AS  = riscv-none-elf-gcc
LD  = riscv-none-elf-g++

CXXFLAGS = -g -Wall -Wextra -std=c++20 -ffreestanding -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany -msmall-data-limit=0
CFLAGS   = -g -Wall -Wextra -ffreestanding -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany -msmall-data-limit=0
ASFLAGS  = -g -Wall -Wextra -ffreestanding -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany -msmall-data-limit=0
LDFLAGS  = -nostdlib -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany -msmall-data-limit=0 -T linker/kernel.ld -Wl,-Map=build/kernel.map

OBJCOPY = riscv-none-elf-objcopy

BUILD_DIR = build
INCLUDES  = -Iinclude -Iinclude/arch/riscv -Iinclude/utility -Ilib/libfdt -Iinclude/kernel

KERNEL_ELF = $(BUILD_DIR)/kernel.elf
BIN_FILE   = $(BUILD_DIR)/kernel.bin

rwildcard = $(foreach d,$(wildcard $1/*),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CPP_SRCS := $(call rwildcard,src,*.cpp)
ASM_SRCS := $(call rwildcard,src,*.S)

FDT_SRCS := $(call rwildcard,lib/libfdt,*.c)
FDT_OBJS := $(patsubst lib/libfdt/%.c,$(BUILD_DIR)/libfdt/%.o,$(FDT_SRCS))

CPP_OBJS := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SRCS))
ASM_OBJS := $(patsubst src/%.S,$(BUILD_DIR)/%.o,$(ASM_SRCS))

OBJS = $(CPP_OBJS) $(ASM_OBJS) $(FDT_OBJS)

all: build
	@echo Done!

build: setup $(KERNEL_ELF) $(BIN_FILE)

$(BIN_FILE): $(KERNEL_ELF)
	@echo Creating raw binary...
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(BIN_FILE)

setup:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"

$(BUILD_DIR)/%.o: src/%.cpp
	@echo Compiling C++ $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.S
	@echo Assembling $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(AS) $(ASFLAGS) $(INCLUDES) -c $< -o $@

$(KERNEL_ELF): $(OBJS)
	@echo Linking kernel...
	$(LD) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/libfdt/%.o: lib/libfdt/%.c
	@echo Compiling FDT $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo Cleaning build output...
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"

run: $(KERNEL_ELF)
	qemu-system-riscv64 -machine virt -cpu rv64 -m 128M -nographic -bios none -kernel $(KERNEL_ELF)

run-custom: $(BIN_FILE)
	rv64i $(BIN_FILE) --system opensbi --memory 12000

run-bin: $(BIN_FILE)
	qemu-system-riscv64 -machine virt -cpu rv64 -m 128M -nographic -bios none -kernel $(BIN_FILE)

print-src:
	@echo CPP_SRCS=$(CPP_SRCS)
	@echo ASM_SRCS=$(ASM_SRCS)