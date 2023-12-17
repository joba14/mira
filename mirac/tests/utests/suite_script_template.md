
Here is a template for shell scripts, used to build and run the tests suite:
```sh
# !/bin/sh

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
UTESTER_DIR="$SCRIPT_DIR/../../../tools/utester"
MIRAC_DIR="$SCRIPT_DIR/../.."

# --------------------------------------------------------------------------- #

PROJECT_NAME="<ENTER_TESTS_SUITE_FILE_NAME>"

INCLUDES="
	-I$UTESTER_DIR/include
	-I$MIRAC_DIR/include
"

SOURCES="
	# <ENTER_MIRAC_SOURCE_FILES_THAT_ARE_REQUIRED_FOR_TESTS> # Like below:
	# $MIRAC_DIR/source/mirac/example.c
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
```