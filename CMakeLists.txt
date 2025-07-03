cmake_minimum_required(VERSION 3.10)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(ChipSequencer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Utils.cmake)
include(cmake/PlatformConfig.cmake)
include(cmake/Dependencies.cmake)
include(cmake/Assembly.cmake)
include(cmake/ThirdParty.cmake)
include(cmake/Application.cmake)
include(cmake/ClangTidy.cmake)
