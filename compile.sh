#!/bin/bash

nasm -f bin -g core/main.asm -o bin/player
chmod +x bin/player
