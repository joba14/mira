
# commands
CPP = cpp
MIRAC = ./../../../mirac/build/mirac
NASM = nasm
LD = ld

# directories
SOURCE_DIR = ./
BUILD_DIR = ./build

# source file and output name
SOURCE_FILE = main.mira
OUTPUT_NAME = figures

# output files
OUTPUT_MIRA = $(BUILD_DIR)/$(OUTPUT_NAME).mira
OUTPUT_ASM = $(BUILD_DIR)/$(OUTPUT_NAME).asm
OUTPUT_OBJ = $(BUILD_DIR)/$(OUTPUT_NAME).o
OUTPUT_EXE_PATH = $(BUILD_DIR)/$(OUTPUT_NAME).out

# flags
CPP_FLAGS = -I./../../
MIRAC_FLAGS = -u -e _start -a nasm_x86_64_linux

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
	$(CPP) $(CPP_FLAGS) -P -o $@ $<

run: $(OUTPUT_EXE_PATH)
	$<

clean:
	rm -rf $(BUILD_DIR)

.PHONY:
	all setup build run clean
