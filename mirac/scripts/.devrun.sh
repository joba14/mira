
# !/bin/sh

clear
./build.sh debug
time ./../build/mirac -e _start -a fasm_x86_64_linux $1 ./experiments/devrun_output.asm
# gdb --args ./../build/mirac -e _start -a fasm_x86_64_linux $1 ./experiments/devrun_output.asm
