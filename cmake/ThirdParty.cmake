# ImGui
file(GLOB IMGUI_SOURCES
    "imgui/*.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
    "imgui/backends/imgui_impl_sdl2.cpp"
)

if(HAVE_SDL_RENDERER)
    add_definitions(-DHAVE_SDL_RENDERER=1)
    list(APPEND IMGUI_SOURCES "imgui/backends/imgui_impl_sdlrenderer2.cpp")
    message(STATUS "Including SDL renderer backend")
else()
    add_definitions(-DHAVE_SDL_RENDERER=0)
    message(STATUS "Excluding SDL renderer backend")
endif()


add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/imgui
    ${SDL2_INCLUDE_DIRS}
)

# ImGui Knobs
add_library(imgui_knobs STATIC imgui-knobs/imgui-knobs.cpp)
target_include_directories(imgui_knobs PUBLIC
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui-knobs
)
target_link_libraries(imgui_knobs PUBLIC imgui)

# Native File Dialog
if(WIN32)
    add_library(nfd STATIC
        nfd/src/nfd_common.c
        nfd/src/nfd_win.cpp
    )
else()
    add_library(nfd STATIC
        nfd/src/nfd_common.c
        nfd/src/nfd_gtk.c
    )
endif()

target_include_directories(nfd PUBLIC
    ${CMAKE_SOURCE_DIR}/nfd/src/include
    ${GTK_INCLUDE_DIRS}
)
target_link_libraries(nfd PUBLIC ${GTK_LIBRARIES})

# JSON library
add_subdirectory(json)
