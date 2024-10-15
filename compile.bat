nasm -f bin -l bin\player.lst -o bin\main.com src\main.asm
nasm -d EXE -f obj -o bin\main.obj src\main.asm
jwlinkd format dos option map option start=start name bin\main.exe file bin\main.obj
