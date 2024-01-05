
# !/bin/sh

tabs 4
clear
echo "-------------------"
./../build.sh debug
time ./../../build/mirac -e _start -a fasm_x86_64_linux -s ./hello_world.mira ./output.asm
echo "-------------------"
nasm -f elf64 ./output.asm -o ./output.o
ld ./output.o -o ./output.out
echo "-------------------"
./output.out
echo $?
