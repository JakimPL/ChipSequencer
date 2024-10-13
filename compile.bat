nasm -f bin -l bin/player.lst -o bin/player.com src/player.asm
nasm -d EXE -f obj -o bin/player.obj src/player.asm
jwlinkd format dos option start=start name bin/player.exe file bin/player.obj