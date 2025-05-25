#!/bin/bash

if [ "$1" == "DEBUG" ]; then
    echo "Compiling in DEBUG mode (ELF32)..."
    nasm -f elf32 -D DEBUG -g -F dwarf core/main.asm -o build/main.o
    ld -m elf_i386 -o bin/main build/main.o
else
    echo "Compiling in BIN mode..."
    nasm -f bin core/main.asm -o bin/main
    chmod +x bin/main
fi

echo "Compilation complete."
