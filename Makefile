CXX = riscv-none-elf-g++
CC  = riscv-none-elf-gcc
AS  = riscv-none-elf-gcc
LD  = riscv-none-elf-g++

CXXFLAGS = -g -Wall -Wextra -std=c++20 -ffreestanding -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany -msmall-data-limit=0
CFLAGS   = -g -Wall -Wextra -ffreestanding -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany -msmall-data-limit=0
ASFLAGS  = -g -Wall -Wextra -ffreestanding -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany -msmall-data-limit=0
LDFLAGS_COMMON = -nostdlib -march=rv64imac_zicsr -mabi=lp64 -mcmodel=medany -msmall-data-limit=0

LDFLAGS_M = $(LDFLAGS_COMMON) -T linker/kernel-m.ld
LDFLAGS_S = $(LDFLAGS_COMMON) -T linker/kernel-s.ld

OBJCOPY = riscv-none-elf-objcopy

BUILD_DIR = build
INCLUDES  = -Iinclude -Iinclude/arch/riscv -Iinclude/utility -Ilib/libfdt -Iinclude/kernel

KERNEL_M_ELF = $(BUILD_DIR)/kernel-m.elf
KERNEL_S_ELF = $(BUILD_DIR)/kernel-s.elf
KERNEL_M_BIN = $(BUILD_DIR)/kernel-m.bin
KERNEL_S_BIN = $(BUILD_DIR)/kernel-s.bin

rwildcard = $(foreach d,$(wildcard $1/*),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# All sources
ALL_CPP_SRCS := $(call rwildcard,src,*.cpp)
ALL_ASM_SRCS := $(call rwildcard,src,*.S)

FDT_SRCS := $(call rwildcard,lib/libfdt,*.c)

# Boot-specific files
M_ENTRY_SRC     := src/arch/riscv/boot/m_entry.S
S_ENTRY_SRC     := src/arch/riscv/boot/s_entry.S
M_DISPATCH_SRC  := src/arch/riscv/boot/dispatch_m.cpp
S_DISPATCH_SRC  := src/arch/riscv/boot/dispatch_s.cpp

# Remove boot-specific files from common lists
COMMON_CPP_SRCS := $(filter-out $(M_DISPATCH_SRC) $(S_DISPATCH_SRC),$(ALL_CPP_SRCS))
COMMON_ASM_SRCS := $(filter-out $(M_ENTRY_SRC) $(S_ENTRY_SRC),$(ALL_ASM_SRCS))

# Object path helpers
COMMON_CPP_OBJS := $(patsubst src/%.cpp,$(BUILD_DIR)/common/%.o,$(COMMON_CPP_SRCS))
COMMON_ASM_OBJS := $(patsubst src/%.S,$(BUILD_DIR)/common/%.o,$(COMMON_ASM_SRCS))
FDT_OBJS        := $(patsubst lib/libfdt/%.c,$(BUILD_DIR)/libfdt/%.o,$(FDT_SRCS))

M_ENTRY_OBJ     := $(BUILD_DIR)/m/arch/riscv/boot/m_entry.o
S_ENTRY_OBJ     := $(BUILD_DIR)/s/arch/riscv/boot/s_entry.o
M_DISPATCH_OBJ  := $(BUILD_DIR)/m/arch/riscv/boot/dispatch_m.o
S_DISPATCH_OBJ  := $(BUILD_DIR)/s/arch/riscv/boot/dispatch_s.o

M_OBJS = $(COMMON_CPP_OBJS) $(COMMON_ASM_OBJS) $(FDT_OBJS) $(M_ENTRY_OBJ) $(M_DISPATCH_OBJ)
S_OBJS = $(COMMON_CPP_OBJS) $(COMMON_ASM_OBJS) $(FDT_OBJS) $(S_ENTRY_OBJ) $(S_DISPATCH_OBJ)

all: build
	@echo Done!

build: setup $(KERNEL_M_ELF) $(KERNEL_M_BIN) $(KERNEL_S_ELF) $(KERNEL_S_BIN)

# Raw binaries
$(KERNEL_M_BIN): $(KERNEL_M_ELF)
	@echo Creating raw binary $@...
	$(OBJCOPY) -O binary $(KERNEL_M_ELF) $(KERNEL_M_BIN)

$(KERNEL_S_BIN): $(KERNEL_S_ELF)
	@echo Creating raw binary $@...
	$(OBJCOPY) -O binary $(KERNEL_S_ELF) $(KERNEL_S_BIN)

setup:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"

# Common C++
$(BUILD_DIR)/common/%.o: src/%.cpp
	@echo Compiling common C++ $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Common ASM
$(BUILD_DIR)/common/%.o: src/%.S
	@echo Assembling common $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(AS) $(ASFLAGS) $(INCLUDES) -c $< -o $@

# M-specific C++
$(BUILD_DIR)/m/%.o: src/%.cpp
	@echo Compiling M-mode C++ $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# S-specific C++
$(BUILD_DIR)/s/%.o: src/%.cpp
	@echo Compiling S-mode C++ $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# M-specific ASM
$(BUILD_DIR)/m/%.o: src/%.S
	@echo Assembling M-mode $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(AS) $(ASFLAGS) $(INCLUDES) -c $< -o $@

# S-specific ASM
$(BUILD_DIR)/s/%.o: src/%.S
	@echo Assembling S-mode $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(AS) $(ASFLAGS) $(INCLUDES) -c $< -o $@

# libfdt
$(BUILD_DIR)/libfdt/%.o: lib/libfdt/%.c
	@echo Compiling FDT $<...
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Link M-mode kernel
$(KERNEL_M_ELF): $(M_OBJS)
	@echo Linking M-mode kernel...
	$(LD) $(M_OBJS) $(LDFLAGS_M) -Wl,-Map=$(BUILD_DIR)/kernel-m.map -o $@

# Link S-mode kernel
$(KERNEL_S_ELF): $(S_OBJS)
	@echo Linking S-mode kernel...
	$(LD) $(S_OBJS) $(LDFLAGS_S) -Wl,-Map=$(BUILD_DIR)/kernel-s.map -o $@

clean:
	@echo Cleaning build output...
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"

run-m: $(KERNEL_M_ELF)
	qemu-system-riscv64 -machine virt -cpu rv64 -m 128M -nographic -bios none -kernel $(KERNEL_M_ELF)

run-s: $(KERNEL_S_ELF)
	qemu-system-riscv64 -machine virt -cpu rv64 -m 128M -nographic -bios default -kernel $(KERNEL_S_ELF)

run-m-bin: $(KERNEL_M_BIN)
	qemu-system-riscv64 -machine virt -cpu rv64 -m 128M -nographic -bios none -kernel $(KERNEL_M_BIN)

print-src:
	@echo ALL_CPP_SRCS=$(ALL_CPP_SRCS)
	@echo ALL_ASM_SRCS=$(ALL_ASM_SRCS)
	@echo COMMON_CPP_SRCS=$(COMMON_CPP_SRCS)
	@echo COMMON_ASM_SRCS=$(COMMON_ASM_SRCS)