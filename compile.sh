#!/bin/bash

nasm -f bin -g core/main.asm -o bin/main
chmod +x bin/main
