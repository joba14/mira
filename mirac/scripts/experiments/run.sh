
# !/bin/sh

tabs 4
clear

rm -fr ./*.asm
rm -fr ./*.o
rm -fr ./*.out

echo "--- debug : building ---"
./../build.sh debug
cp -f ./../../build/mirac_debug ./mirac_debug.out
time ./mirac_debug.out -e _start -a nasm_x86_64_linux ./hello_world.mira ./output_debug.asm
nasm -f elf64 ./output_debug.asm -o ./output_debug.o
ld ./output_debug.o -o ./output_debug.out

echo "--- release : building ---"
./../build.sh release
cp -f ./../../build/mirac_release ./mirac_release.out
time ./mirac_release.out -e _start -a nasm_x86_64_linux ./hello_world.mira ./output_release.asm
nasm -f elf64 ./output_release.asm -o ./output_release.o
ld ./output_release.o -o ./output_release.out

echo "--- debug : running ---"
time ./output_debug.out
echo $?

echo "--- release : running ---"
time ./output_release.out
echo $?
