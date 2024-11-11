rm bin/player.o bin/libplayer.a
nasm -d EXE=1 -d ELF -f elf32 -g src/player.asm -o bin/player.o
ar rcs bin/libplayer.a bin/player.o