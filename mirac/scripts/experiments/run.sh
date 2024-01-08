
# !/bin/sh

tabs 4
clear


echo "--- debug : building   ---"
./../build.sh debug
time ./../../build/mirac_debug -e _start -a nasm_x86_64_linux ./hello_world.mira ./output.asm

echo "--- debug : assembling ---"
nasm -f elf64 ./output.asm -o ./output.o
ld ./output.o -o ./output.out

echo "--- debug : running    ---"
./output.out
echo $?


echo "--- release : building   ---"
./../build.sh release
time ./../../build/mirac_release -e _start -a nasm_x86_64_linux ./hello_world.mira ./output.asm

echo "--- release : assembling ---"
nasm -f elf64 ./output.asm -o ./output.o
ld ./output.o -o ./output.out

echo "--- release : running    ---"
./output.out
echo $?
