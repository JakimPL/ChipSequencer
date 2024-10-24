if exist bin\main.com del bin\main.com
if exist bin\main.obj del bin\main.obj
if exist bin\main.exe del bin\main.exe

nasm -d EXE=0 -Ox -f bin -l bin\player.lst -o bin\main.com src\main.asm
nasm -d EXE=1 -Ox -f obj -o bin\main.obj src\main.asm
jwlinkd @linker.lnk name bin\main.exe file bin\main.obj
