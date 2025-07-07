set(TRACKER_SOURCES
    tracker/audio/engine.cpp
    tracker/audio/fft.cpp
    tracker/audio/wave.cpp
    tracker/driver/file.cpp
    tracker/driver/port.cpp
    tracker/general.cpp
    tracker/gui/clipboard/clipboard.cpp
    tracker/gui/clipboard/items/commands.cpp
    tracker/gui/clipboard/items/item.cpp
    tracker/gui/clipboard/items/notes.cpp
    tracker/gui/gui.cpp
    tracker/gui/history/actions/action.cpp
    tracker/gui/history/actions/add.cpp
    tracker/gui/history/actions/command.cpp
    tracker/gui/history/actions/lock.cpp
    tracker/gui/history/actions/note.cpp
    tracker/gui/history/actions/remove.cpp
    tracker/gui/history/actions/routing.cpp
    tracker/gui/history/actions/selection.cpp
    tracker/gui/history/actions/sequence.cpp
    tracker/gui/history/actions/text.cpp
    tracker/gui/history/actions/value.cpp
    tracker/gui/history/actions/wavetable.cpp
    tracker/gui/history/manager.cpp
    tracker/gui/inputs/input.cpp
    tracker/gui/inputs/numeric.cpp
    tracker/gui/inputs/string.cpp
    tracker/gui/keys.cpp
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
    tracker/gui/shortcuts/manager.cpp
    tracker/gui/themes/color.cpp
    tracker/gui/themes/theme.cpp
    tracker/gui/undo.cpp
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
    tracker/structures/resources/manager.cpp
    tracker/structures/sequence.cpp
    tracker/structures/wavetable.cpp
    tracker/tuning/frequencies.cpp
    tracker/tuning/scale.cpp
    tracker/utils/file.cpp
    tracker/utils/math.cpp
    tracker/utils/paths.cpp
    tracker/utils/string.cpp
    tracker/utils/system.cpp
)

add_executable(ChipSequencer ${TRACKER_SOURCES})

target_compile_options(ChipSequencer PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Werror=switch-enum>
    $<$<COMPILE_LANGUAGE:CXX>:-Werror=switch-enum ${SANITIZER_FLAGS}>
)

add_dependencies(ChipSequencer asm_player)
target_include_directories(ChipSequencer PRIVATE
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui-knobs
    ${CMAKE_SOURCE_DIR}/miniz-cpp
    ${SDL2_INCLUDE_DIRS}
    ${FFTW_INCLUDE_DIRS}
)

if (WIN32)
    target_link_libraries(ChipSequencer
        imgui
        imgui_knobs
        nlohmann_json::nlohmann_json
        ${ASM_OBJECT_FILE}
        ${SDL2_LIBRARIES}
        ${OPENGL_LIBRARIES}
        portaudio
        nfd
        ${SNDFILE_LIBRARIES}
        ${FFTW_LIBRARIES}
    )

    set(REQUIRED_DLLS
        "${FFTW_ROOT}/libfftw3f-3.dll"
        "${MINGW_PREFIX}/bin/SDL2.dll"
        "${MINGW_PREFIX}/bin/libFLAC.dll"
        "${MINGW_PREFIX}/bin/libgcc_s_dw2-1.dll"
        "${MINGW_PREFIX}/bin/libmp3lame-0.dll"
        "${MINGW_PREFIX}/bin/libmpg123-0.dll"
        "${MINGW_PREFIX}/bin/libogg-0.dll"
        "${MINGW_PREFIX}/bin/libopus-0.dll"
        "${MINGW_PREFIX}/bin/libportaudio.dll"
        "${MINGW_PREFIX}/bin/libsndfile-1.dll"
        "${MINGW_PREFIX}/bin/libstdc++-6.dll"
        "${MINGW_PREFIX}/bin/libvorbis-0.dll"
        "${MINGW_PREFIX}/bin/libvorbisenc-2.dll"
        "${MINGW_PREFIX}/bin/libwinpthread-1.dll"
    )

    foreach(DLL_FILE ${REQUIRED_DLLS})
        if(EXISTS ${DLL_FILE})
            add_custom_command(TARGET ChipSequencer POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${DLL_FILE}
                $<TARGET_FILE_DIR:ChipSequencer>
            )
        endif()
    endforeach()
else()
    target_link_libraries(ChipSequencer
        imgui
        imgui_knobs
        nlohmann_json::nlohmann_json
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
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(ChipSequencer PRIVATE ${SANITIZER_FLAGS})
endif()

set_target_properties(ChipSequencer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)

if(WIN32)
    add_custom_command(TARGET ChipSequencer POST_BUILD
        COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/shell/shortcut.ps1"
                -ShortcutPath "${CMAKE_SOURCE_DIR}/ChipSequencer.lnk"
                -TargetPath "${CMAKE_SOURCE_DIR}/bin/ChipSequencer.exe"
                -WorkingDirectory "${CMAKE_SOURCE_DIR}"
        COMMENT "Creating shortcut to executable"
    )
else()
    add_custom_command(TARGET ChipSequencer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove -f "${CMAKE_SOURCE_DIR}/ChipSequencer"
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        "bin/ChipSequencer"
        "${CMAKE_SOURCE_DIR}/ChipSequencer"
        COMMENT "Creating symlink to executable in main directory"
    )
endif()
