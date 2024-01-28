
# config
INCLUDE_PATHS := \
	-I./../.. \
	-I.

BUILD_FLAGS := \
	-d \
	-u \
	-e _start \
	-a nasm_x86_64_linux

SOURCE_FILE = main.mira
OUTPUT_NAME = figures

# -------------------------------------------------- #
# do not change anything below this line (unless you
# know what you are doing).

# commands
CPP = cpp
MIRAC = ./../../../mirac/build/mirac
NASM = nasm
LD = ld

# flags
CPP_FLAGS = -P $(INCLUDE_PATHS)
MIRAC_FLAGS += $(BUILD_FLAGS)

# directories
SOURCE_DIR = ./
BUILD_DIR = ./build

# output files
OUTPUT_MIRA = $(BUILD_DIR)/$(OUTPUT_NAME).mira
OUTPUT_ASM = $(BUILD_DIR)/$(OUTPUT_NAME).asm
OUTPUT_OBJ = $(BUILD_DIR)/$(OUTPUT_NAME).o
OUTPUT_EXE_PATH = $(BUILD_DIR)/$(OUTPUT_NAME).out

# targets
all: setup build

setup:
	mkdir -p $(BUILD_DIR)
	echo $(shell pwd) > $(BUILD_DIR)/project_root.txt

build: $(OUTPUT_EXE_PATH)

$(OUTPUT_EXE_PATH): $(OUTPUT_OBJ)
	$(LD) $< -o $@

$(OUTPUT_OBJ): $(OUTPUT_ASM)
	$(NASM) -f elf64 $< -o $@

$(OUTPUT_ASM): $(OUTPUT_MIRA)
	$(MIRAC) $(MIRAC_FLAGS) $(OUTPUT_MIRA) $(OUTPUT_ASM)

$(OUTPUT_MIRA): $(SOURCE_DIR)/$(SOURCE_FILE)
	$(CPP) $(CPP_FLAGS) -o $@ $<

run: $(OUTPUT_EXE_PATH)
	$<

clean:
	rm -rf $(BUILD_DIR)

.PHONY:
	all setup build run clean

# -------------------------------------------------- #
