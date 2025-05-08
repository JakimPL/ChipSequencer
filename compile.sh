#!/bin/bash

nasm -f bin core/main.asm -o bin/main
chmod +x bin/main
