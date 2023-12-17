
# !/bin/sh

# Check if lalal.txt exists and remove it if it does
if [ -e "./string_view_suite.out" ]; then
	echo "[info]: removing previous test executable './string_view_suite.out'."
	rm "./string_view_suite.out"
fi

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
	-I./../../include \
	-I./../../../tools/utester/include \
	-g -O0 \
	./../../source/mirac/c_common.c \
	./../../source/mirac/debug.c \
	./../../source/mirac/logger.c \
	./../../source/mirac/string_view.c \
	./string_view_suite.c \
	-o ./string_view_suite.out

# Check if compilation was successful
if [ $? -eq 0 ]; then
	echo "info : compilation successful - executable: ./string_view_suite.out"
	./string_view_suite.out
	exit 0
else
	echo "error: compilation failed."
	exit 1
fi
