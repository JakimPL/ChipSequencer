if exist build\driver.obj del build\driver.obj
if exist build\player.obj del build\player.obj
if exist build\song.obj del build\song.obj

if exist build\main.com del build\main.com
if exist build\main.obj del build\main.obj
if exist build\main.exe del build\main.exe

tools\nasm -Ox -f obj -o build\song.obj core\song.asm
tools\nasm -Ox -f obj -o build\driver.obj core\driver.asm
tools\nasm -Ox -f obj -l build\player.lst -o build\player.obj core\player.asm
tools\nasm -Ox -f obj -l build\main.lst -o build\main.obj core\main.asm
tools\jwlinkd @linker.lnk
