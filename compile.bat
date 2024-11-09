if exist bin\main.com del bin\main.com
if exist bin\main.obj del bin\main.obj
if exist bin\player.obj del bin\player.obj
if exist bin\main.exe del bin\main.exe

nasm -Ox -f obj -l bin\player.lst -o bin\player.obj src\player.asm
nasm -Ox -f obj -l bin\main.lst -o bin\main.obj src\main.asm
jwlinkd @linker.lnk name bin\main.exe file bin\main.obj file bin\player.obj
