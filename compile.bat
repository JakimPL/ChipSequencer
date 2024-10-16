nasm -d EXE=0 -f bin -l bin\player.lst -o bin\main.com src\main.asm
nasm -d EXE=1 -f obj -o bin\main.obj src\main.asm
jwlinkd format dos option dosseg option map option start=start name bin\main.exe file bin\main.obj
