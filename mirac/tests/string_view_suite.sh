
# !/bin/sh

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
MIRAC_DIR="$SCRIPT_DIR/.."

# --------------------------------------------------------------------------- #

PROJECT_NAME="string_view_suite"

INCLUDES="
	-I$MIRAC_DIR/include
"

SOURCES="
	$MIRAC_DIR/source/mirac/debug.c
	$MIRAC_DIR/source/mirac/logger.c
	$MIRAC_DIR/source/mirac/c_common.c
	$MIRAC_DIR/source/mirac/string_view.c
	./$PROJECT_NAME.c
"

LIBRARIES="
"

# --------------------------------------------------------------------------- #

# Compilation command
gcc -Wall \
	-Wextra \
	-Wpedantic \
	-Werror \
	-Wshadow \
	-Wimplicit \
	-Wreturn-type \
	-Wunknown-pragmas \
	-Wunused-variable \
	-Wunused-function \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wconversion \
	-Wsign-conversion \
	-Wunreachable-code \
	-g -O0 \
	$INCLUDES \
	$SOURCES \
	-o "./$PROJECT_NAME.out" \
	$LIBRARIES

# Check if compilation was successful
if [ $? -eq 0 ]; then
	echo "[info]: compilation successful - executable: ./$PROJECT_NAME.out"
	./$PROJECT_NAME.out
	exit 0
else
	echo "[error]: compilation failed."
	exit 1
fi
