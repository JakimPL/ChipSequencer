cmake_minimum_required(VERSION 3.10)
project(ChipSequencer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(COMMON_C_FLAGS "-m32 -Wno-pointer-arith -Wno-address-of-packed-member")
set(COMMON_CXX_FLAGS "-m32 -Wno-pointer-arith -Wno-address-of-packed-member")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${COMMON_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${COMMON_CXX_FLAGS} -fsanitize=address -g")
    set(SANITIZER_FLAGS "-fsanitize=address")
else()
    set(CMAKE_C_FLAGS "${COMMON_C_FLAGS} -O2 -DNDEBUG")
    set(CMAKE_CXX_FLAGS "${COMMON_CXX_FLAGS} -O2 -DNDEBUG")
    set(SANITIZER_FLAGS "")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_LIBRARY_PATH /usr/lib/i386-linux-gnu ${CMAKE_LIBRARY_PATH})
set(OPENGL_INCLUDE_DIRS "/usr/include")
set(OPENGL_LIBRARIES "/usr/lib/i386-linux-gnu/libGL.so.1")
set(ENV{PKG_CONFIG_PATH} "/usr/lib/i386-linux-gnu/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig")

set(SDL2_INCLUDE_DIRS "/usr/include/SDL2")
set(SDL2_LIBRARIES "-lSDL2")

find_program(NASM_EXECUTABLE NAMES nasm)
if(NOT NASM_EXECUTABLE)
    message(FATAL_ERROR "NASM assembler not found")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW REQUIRED fftw3f)
if (NOT FFTW_FOUND)
    message(FATAL_ERROR "FFTW3 not found. Please install libfftw3-dev:i386.")
endif()

pkg_check_modules(GTK REQUIRED gtk+-3.0)
if (NOT GTK_FOUND)
    message(FATAL_ERROR "GTK not found")
endif()

pkg_check_modules(SNDFILE REQUIRED sndfile)
if (NOT SNDFILE_FOUND)
    message(FATAL_ERROR "libsndfile not found. Please install libsndfile-dev:i386.")
endif()

set(ASM_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(ASM_SOURCE_FILE "${CMAKE_SOURCE_DIR}/core/player.asm")
set(ASM_OBJECT_FILE "${CMAKE_BINARY_DIR}/player.o")

add_custom_target(asm_objects ALL
    COMMAND ${CMAKE_COMMAND} -E remove -f ${ASM_OBJECT_FILE}
    COMMAND ${NASM_EXECUTABLE} -d TRACKER -f elf32 -Fdwarf -g ${ASM_SOURCE_FILE} -o ${ASM_OBJECT_FILE}
    WORKING_DIRECTORY ${ASM_SOURCE_DIR}
)

file(GLOB IMGUI_SOURCES
    "imgui/*.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
    "imgui/backends/imgui_impl_sdl2.cpp"
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/imgui
    ${SDL2_INCLUDE_DIRS}
)

add_library(imgui_knobs STATIC imgui-knobs/imgui-knobs.cpp)
target_include_directories(imgui_knobs PUBLIC
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui-knobs
)
target_link_libraries(imgui_knobs PUBLIC imgui)

add_library(song STATIC tracker/song/song.cpp)
target_include_directories(song PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui-knobs
    ${SNDFILE_INCLUDE_DIRS}
)
target_link_libraries(song PUBLIC imgui imgui_knobs)

add_library(nfd STATIC
    nfd/src/nfd_common.c
    nfd/src/nfd_gtk.c
)
target_include_directories(nfd PUBLIC
    ${CMAKE_SOURCE_DIR}/nfd/src/include
    ${GTK_INCLUDE_DIRS}
)
target_link_libraries(nfd PUBLIC ${GTK_LIBRARIES})

add_executable(ChipSequencer
    tracker/audio/engine.cpp
    tracker/audio/fft.cpp
    tracker/audio/wave.cpp
    tracker/driver/file.cpp
    tracker/driver/port.cpp
    tracker/general.cpp
    tracker/gui/gui.cpp
    tracker/gui/history/actions/action.cpp
    tracker/gui/history/actions/add.cpp
    tracker/gui/history/actions/note.cpp
    tracker/gui/history/actions/remove.cpp
    tracker/gui/history/actions/routing.cpp
    tracker/gui/history/actions/sequence.cpp
    tracker/gui/history/actions/text.cpp
    tracker/gui/history/actions/value.cpp
    tracker/gui/history/actions/wavetable.cpp
    tracker/gui/history/manager.cpp
    tracker/gui/history/shortcuts.cpp
    tracker/gui/inputs/input.cpp
    tracker/gui/inputs/numeric.cpp
    tracker/gui/inputs/string.cpp
    tracker/gui/names.cpp
    tracker/gui/node.cpp
    tracker/gui/panels/channels.cpp
    tracker/gui/panels/commands/channels.cpp
    tracker/gui/panels/commands/sequences.cpp
    tracker/gui/panels/dsps.cpp
    tracker/gui/panels/editor.cpp
    tracker/gui/panels/envelopes.cpp
    tracker/gui/panels/general.cpp
    tracker/gui/panels/menu.cpp
    tracker/gui/panels/orders.cpp
    tracker/gui/panels/oscillators.cpp
    tracker/gui/panels/panel.cpp
    tracker/gui/panels/patterns.cpp
    tracker/gui/panels/routings.cpp
    tracker/gui/panels/sequences.cpp
    tracker/gui/panels/summary.cpp
    tracker/gui/panels/waveform.cpp
    tracker/gui/panels/wavetables.cpp
    tracker/gui/patterns/commands.cpp
    tracker/gui/patterns/pattern.cpp
    tracker/gui/patterns/selection.cpp
    tracker/gui/utils.cpp
    tracker/main.cpp
    tracker/maps/routing.cpp
    tracker/song/buffers.cpp
    tracker/song/core.cpp
    tracker/song/links/link.cpp
    tracker/song/links/manager.cpp
    tracker/song/links/target.cpp
    tracker/song/lock/registry.cpp
    tracker/song/output.cpp
    tracker/song/song.cpp
    tracker/structures/channel.cpp
    tracker/structures/commands/command.cpp
    tracker/structures/commands/sequence.cpp
    tracker/structures/order.cpp
    tracker/structures/sequence.cpp
    tracker/structures/wavetable.cpp
    tracker/tuning/frequencies.cpp
    tracker/tuning/scale.cpp
    tracker/utils/math.cpp
    tracker/utils/string.cpp
    tracker/utils/temp.cpp
)

target_compile_options(ChipSequencer PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Werror=switch-enum>
    $<$<COMPILE_LANGUAGE:CXX>:-Werror=switch-enum ${SANITIZER_FLAGS}>
)

add_dependencies(ChipSequencer asm_objects)
target_include_directories(ChipSequencer PRIVATE
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui-knobs
    ${SDL2_INCLUDE_DIRS}
    ${FFTW_INCLUDE_DIRS}
)
target_link_libraries(ChipSequencer
    song
    imgui
    imgui_knobs
    ${ASM_OBJECT_FILE}
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    portaudio
    dl
    pthread
    z
    nfd
    ${SNDFILE_LIBRARIES}
    ${FFTW_LIBRARIES}
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(ChipSequencer PRIVATE ${SANITIZER_FLAGS})
endif()

set_target_properties(ChipSequencer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)
