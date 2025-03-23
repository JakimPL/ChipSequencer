rm -f bin/player.o bin/main.o bin/main
nasm -d EXE=1 -d ELF -f elf32 -Fdwarf -g src/player.asm -o bin/player.o

g++ -m32 -g -c --std=c++17 \
    -Iimgui -I/usr/include/SDL2 \
    imgui/imgui.cpp \
    imgui/imgui_draw.cpp \
    imgui/imgui_widgets.cpp \
    imgui/imgui_tables.cpp \
    imgui/backends/imgui_impl_opengl3.cpp \
    imgui/backends/imgui_impl_sdl2.cpp

g++ -m32 -g -c --std=c++17 \
    -Iimgui -Iimgui-knobs -I/usr/include/SDL2 \
    imgui-knobs/imgui-knobs.cpp

mv *.o bin/

g++ -m32 -g -c --std=c++17 \
    -Iimgui -Iimgui-knobs -I/usr/include/SDL2 \
    tracker/song/song.cpp \
    -o bin/song.o

g++ -m32 -g -c --std=c++17 \
    -Iimgui -Iimgui-knobs -I/usr/include/SDL2 \
    tracker/main.cpp \
    -o bin/main.o

g++ -m32 -g --std=c++17 \
    -Iimgui -Iimgui-knobs -I/usr/include/SDL2 \
    bin/main.o \
    bin/song.o \
    bin/player.o \
    bin/imgui.o \
    bin/imgui-knobs.o \
    bin/imgui_draw.o \
    bin/imgui_widgets.o \
    bin/imgui_tables.o \
    bin/imgui_impl_opengl3.o \
    bin/imgui_impl_sdl2.o \
    -o bin/main \
    -lportaudio -lSDL2 -lGL -ldl -lpthread -lz
