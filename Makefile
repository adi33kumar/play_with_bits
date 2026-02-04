# Set default RPI version to 3 if not specified
RPI_VERSION ?= 3

# Set default mount point for RPi boot partition
BOOTMNT ?= /home/adityakr/baremetal_image

# Set the cross-compiler prefix
ARMGNU ?= aarch64-linux-gnu

# Compiler flags for C files: define RPI_VERSION, warnings, no stdlib, freestanding, include path, general regs only
COPS = -DRPI_VERSION=$(RPI_VERSION) -Wall -nostdlib -nostartfiles -ffreestanding \
	   -Iinclude -mgeneral-regs-only

# Assembler flags: include path
ASMOPS = -Iinclude

# Build directory
BUILD_DIR = build

# Source directory
SRC_DIR = src

# Default target: build kernel8.img
all : kernel8.img

# Clean target: remove build artifacts
clean :
	rm -rf $(BUILD_DIR) *.img *.list *.nm

# Rule to compile C files to object files
$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

# Rule to compile assembly files to object files
$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

# Find all C source files in src/
C_FILES = $(wildcard $(SRC_DIR)/*.c)

# Find all assembly source files in src/
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)

# Convert C files to object files
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)

# Add assembly object files
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

# Dependency files
DEP_FILES = $(OBJ_FILES:%.o=%.d)

# Include dependency files
-include $(DEP_FILES)

# Rule to build kernel8.img from linker script and object files
kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	@echo "Building for RPI $(value RPI_VERSION)"
	@echo "Deploy to $(value BOOTMNT)"
	@echo ""
	$(ARMGNU)-ld -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img
	$(ARMGNU)-objdump -d $(BUILD_DIR)/kernel8.elf > kernel8.list
	$(ARMGNU)-nm -n $(BUILD_DIR)/kernel8.elf > kernel8.nm

# Conditional copy based on RPI version
ifeq ($(RPI_VERSION), 4)
	cp kernel8.img $(BOOTMNT)/kernel8-rpi4.img
else
	cp kernel8.img $(BOOTMNT)/
endif
	cp config.txt $(BOOTMNT)/
	sync

# Rule for armstub object
armstub/build/armstub_s.o: armstub/src/armstub.S
	mkdir -p $(@D)
	$(ARMGNU)-gcc $(COPS) -MMD -c $< -o $@

# Target to build armstub
armstub: armstub/build/armstub_s.o
	$(ARMGNU)-ld --section-start=.text=0 -o armstub/build/armstub.elf armstub/build/armstub_s.o
	$(ARMGNU)-objcopy armstub/build/armstub.elf -O binary armstub-new.bin
	cp armstub-new.bin $(BOOTMNT)/
	sync
