#!/bin/bash

FORMAT="bin"
DEBUG_MODE=0
NASM_FLAGS=""
OUTPUT_FILE="bin/main"

while [[ $# -gt 0 ]]; do
    case $1 in
        DEBUG|--debug)
            DEBUG_MODE=1
            FORMAT="elf32"
            shift
            ;;
        -d=*|--define=*)
            NASM_FLAGS="$NASM_FLAGS -d ${1#*=}"
            shift
            ;;
        -d|--define)
            if [[ $2 == -* || -z $2 ]]; then
                echo "Error: Missing value for $1"
                exit 1
            fi
            NASM_FLAGS="$NASM_FLAGS -d $2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [DEBUG] [--define=KEY=VALUE] [--define KEY=VALUE]"
            exit 1
            ;;
    esac
done

if [ $DEBUG_MODE -eq 1 ]; then
    echo "Compiling in DEBUG mode (ELF32)..."
    nasm -f elf32 -D DEBUG -g -F dwarf core/main.asm -o build/main.o $NASM_FLAGS
    ld -m elf_i386 -o $OUTPUT_FILE build/main.o
else
    echo "Compiling in BIN mode..."
    nasm -f bin core/main.asm -o $OUTPUT_FILE $NASM_FLAGS
    chmod +x $OUTPUT_FILE
fi

echo "Compilation complete."
