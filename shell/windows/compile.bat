@echo off
setlocal enabledelayedexpansion

set "FORMAT=bin"
set "DEBUG_MODE=0"
set "NASM_FLAGS="
set "OUTPUT_FILE=bin\main.exe"

:parse_args
if "%~1"=="" goto :end_parse
if /i "%~1"=="DEBUG" (
    set "DEBUG_MODE=1"
    set "FORMAT=win32"
    shift
    goto :parse_args
)
if /i "%~1"=="--debug" (
    set "DEBUG_MODE=1"
    set "FORMAT=win32"
    shift
    goto :parse_args
)
if "%~1:~0,3%"=="-d=" (
    set "NASM_FLAGS=%NASM_FLAGS% -d %~1:~3%"
    shift
    goto :parse_args
)
if "%~1:~0,9%"=="--define=" (
    set "NASM_FLAGS=%NASM_FLAGS% -d %~1:~9%"
    shift
    goto :parse_args
)
if /i "%~1"=="-d" (
    if "%~2"=="" (
        echo Error: Missing value for %~1
        exit /b 1
    )
    if "%~2:~0,1%"=="-" (
        echo Error: Missing value for %~1
        exit /b 1
    )
    set "NASM_FLAGS=%NASM_FLAGS% -d %~2"
    shift
    shift
    goto :parse_args
)
if /i "%~1"=="--define" (
    if "%~2"=="" (
        echo Error: Missing value for %~1
        exit /b 1
    )
    if "%~2:~0,1%"=="-" (
        echo Error: Missing value for %~1
        exit /b 1
    )
    set "NASM_FLAGS=%NASM_FLAGS% -d %~2"
    shift
    shift
    goto :parse_args
)
echo Unknown option: %~1
echo Usage: %~nx0 [DEBUG] [--define=KEY=VALUE] [--define KEY=VALUE]
exit /b 1

:end_parse

if not exist "build" mkdir build
if not exist "bin" mkdir bin

if "%DEBUG_MODE%"=="1" (
    echo Compiling in DEBUG mode ^(WIN32^)...
    nasm -f win32 -D DEBUG -d WIN32 -g -F cv8 core\main.asm -o build\main.obj %NASM_FLAGS%
    if errorlevel 1 (
        echo NASM compilation failed
        exit /b 1
    )
    ld -m i386pe --subsystem console -e _start -o %OUTPUT_FILE% build\main.obj -lwinmm -lkernel32
    if errorlevel 1 (
        echo Linking failed
        exit /b 1
    )
) else (
    echo Compiling in BIN mode...
    nasm -f bin core\main.asm -d WIN32 -o %OUTPUT_FILE% %NASM_FLAGS%
    if errorlevel 1 (
        echo NASM compilation failed
        exit /b 1
    )
)

echo Compilation complete.
