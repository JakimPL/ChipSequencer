rm -f bin/player.o bin/main.o bin/main
nasm -d EXE=1 -d ELF -f elf32 -Fdwarf -g src/player.asm -o bin/player.o

g++ -m32 -g -c tracker/main.cpp -o bin/main.o --std=c++17
g++ -m32 -g bin/main.o bin/player.o -o bin/main --std=c++17 -lportaudio
