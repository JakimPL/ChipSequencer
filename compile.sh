rm -f bin/player.o bin/main.o bin/main
nasm -d EXE=1 -d ELF -f elf32 -Fdwarf -g src/player.asm -o bin/player.o

g++ -m32 -c tracker/main.cpp -o bin/main.o
g++ -m32 bin/main.o bin/player.o -o bin/main
