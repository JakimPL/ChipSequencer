#!/bin/bash

nasm -f elf32 -Fdwarf -g core/player.asm -o build/player.o
nasm -f elf32 -Fdwarf -g core/song.asm -o build/song.o

g++ -m32 -g -Wno-pointer-arith \
    main.cpp \
    build/player.o \
    build/song.o \
    -o bin/player \
    -L/usr/lib/i386-linux-gnu -lasound \
    -Wl,-Map=bin/player.map
