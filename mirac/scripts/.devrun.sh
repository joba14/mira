
# !/bin/sh

clear
./build.sh
time ./../build/mirac -a fasm_x86_64_linux $1
