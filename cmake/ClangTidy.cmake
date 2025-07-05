find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")
    option(ENABLE_CLANG_TIDY "Enable clang-tidy during compilation" OFF)
    if(ENABLE_CLANG_TIDY)
        set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXE})
        message(STATUS "clang-tidy enabled during compilation")
    endif()
else()
    message(WARNING "clang-tidy not found")
endif()

if(CLANG_TIDY_EXE)
    file(GLOB_RECURSE PROJECT_SOURCES
        "tracker/*.cpp"
        "tracker/**/*.cpp"
    )

    set(CLANG_TIDY_ARGS
        -p ${CMAKE_BINARY_DIR}
        --system-headers=false
        --header-filter='^${CMAKE_SOURCE_DIR}/tracker/.*'
        --extra-arg=-std=c++17
        --extra-arg=-stdlib=libstdc++
    )

    add_custom_target(clang-tidy
        COMMAND ${CLANG_TIDY_EXE}
        ${CLANG_TIDY_ARGS}
        ${PROJECT_SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy"
        VERBATIM
    )

    add_custom_target(clang-tidy-fix
        COMMAND ${CLANG_TIDY_EXE}
        ${CLANG_TIDY_ARGS}
        --fix
        ${PROJECT_SOURCES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-tidy --fix"
        VERBATIM
    )
endif()
