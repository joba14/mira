
# !/bin/sh

clear
./build.sh debug
time ./../build/mirac -a fasm_x86_64_linux $1 ./experiments/devrun_output.asm
