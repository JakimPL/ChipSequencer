find_program(NASM_EXECUTABLE NAMES nasm)
if(NOT NASM_EXECUTABLE)
    message(FATAL_ERROR "NASM assembler not found")
endif()

if(WIN32)
    set(MINGW_PREFIX "C:/msys64/mingw32")
    set(OPENGL_LIBRARIES "opengl32")
    set(OPENGL_INCLUDE_DIRS "")

    add_definitions(-DSDL_VIDEO_OPENGL_ES2=0)
    set(SDL2_INCLUDE_DIRS "${MINGW_PREFIX}/include/SDL2")
    set(SDL2_LIBRARIES "-lSDL2main -lSDL2")

    set(FFTW_ROOT "${CMAKE_SOURCE_DIR}/fftw")
    set(FFTW_INCLUDE_DIRS "${FFTW_ROOT}")
    set(FFTW_LIBRARIES "${FFTW_ROOT}/libfftw3f-3.dll")
    if(NOT EXISTS "${FFTW_INCLUDE_DIRS}/fftw3.h")
        message(FATAL_ERROR "FFTW3 header not found at ${FFTW_INCLUDE_DIRS}/fftw3.h. Please extract the FFTW archive to ${FFTW_ROOT}/")
    endif()

    if(NOT EXISTS "${FFTW_LIBRARIES}")
        message(FATAL_ERROR "FFTW3 library not found at ${FFTW_LIBRARIES}. Please extract the FFTW archive to ${FFTW_ROOT}/")
    endif()

    find_library(SNDFILE_LIBRARIES
        NAMES sndfile libsndfile-1
        PATHS ${MINGW_PREFIX}/lib
        NO_DEFAULT_PATH
    )
    find_path(SNDFILE_INCLUDE_DIRS
        NAMES sndfile.h
        PATHS ${MINGW_PREFIX}/include
        NO_DEFAULT_PATH
    )
    if(NOT SNDFILE_LIBRARIES)
        message(FATAL_ERROR "libsndfile not found. Please install the correct mingw64/mingw-w64-x86_64-libsndfile package")
    endif()
else()
    set(OPENGL_INCLUDE_DIRS "/usr/include")
    set(OPENGL_LIBRARIES "/usr/lib/i386-linux-gnu/libGL.so.1")
    set(SDL2_INCLUDE_DIRS "/usr/include/SDL2")
    set(SDL2_LIBRARIES "-lSDL2")

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

    find_package(SDL2 REQUIRED)
    if(SDL2_VERSION VERSION_LESS "2.0.17")
        message(FATAL_ERROR "SDL2 version 2.0.17 or higher is required")
    endif()
endif()
