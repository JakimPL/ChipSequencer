set(ASM_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(ASM_SOURCE_FILE "${CMAKE_SOURCE_DIR}/core/player.asm")

if (WIN32)
    set(ASM_FORMAT "win32")
    set(ASM_DEBUG_FORMAT "cv8")
    set(ASM_OBJECT_FILE "${CMAKE_BINARY_DIR}/player.obj")
    set(ASM_FLAGS "WIN32")
else()
    set(ASM_FORMAT "elf32")
    set(ASM_DEBUG_FORMAT "dwarf")
    set(ASM_OBJECT_FILE "${CMAKE_BINARY_DIR}/player.o")
    set(ASM_FLAGS "ELF32")
endif()

add_custom_target(asm_player ALL
    COMMAND ${CMAKE_COMMAND} -E remove -f ${ASM_OBJECT_FILE}
    COMMAND ${NASM_EXECUTABLE} -d TRACKER -d ${ASM_FLAGS} -f ${ASM_FORMAT} -F${ASM_DEBUG_FORMAT} -g ${ASM_SOURCE_FILE} -o ${ASM_OBJECT_FILE}
    WORKING_DIRECTORY ${ASM_SOURCE_DIR}
)
