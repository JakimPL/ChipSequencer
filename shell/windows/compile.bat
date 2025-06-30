@echo off
setlocal enabledelayedexpansion

set "COMPILATION_MODE=0"
set "NASM_FLAGS="
set "OUTPUT_FILE=bin\main.exe"

:parse_args
if "%~1"=="" goto :end_parse
if /i "%~1"=="STANDARD" (
    set "COMPILATION_MODE=0"
    shift
    goto :parse_args
)
if /i "%~1"=="UNCOMPRESSED" (
    set "COMPILATION_MODE=1"
    shift
    goto :parse_args
)
if /i "%~1"=="DEBUG" (
    set "COMPILATION_MODE=2"
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
echo Usage: %~nx0 [STANDARD^|UNCOMPRESSED^|DEBUG] [--define=KEY=VALUE] [-d KEY=VALUE]
exit /b 1

:end_parse

if not exist "build" mkdir build
if not exist "bin" mkdir bin

if "%COMPILATION_MODE%"=="2" (
    echo Compiling in DEBUG mode...
    nasm -f win32 -d DEBUG -d WIN32 -g -F cv8 core\main.asm -o build\main.obj %NASM_FLAGS%
    ld -m i386pe --subsystem console -e _start -o %OUTPUT_FILE% build\main.obj -lwinmm -lkernel32
) else if "%COMPILATION_MODE%"=="1" (
    echo Compiling in UNCOMPRESSED mode...
    nasm -f bin core\main.asm -d WIN32 -d UNCOMPRESSED -o %OUTPUT_FILE% %NASM_FLAGS%
) else (
    echo Compiling in STANDARD mode...
    nasm -f win32 core\main.asm -d WIN32 -o %OUTPUT_FILE% %NASM_FLAGS%
)

if errorlevel 1 (
    echo Compilation failed.
    exit /b 1
)

echo Compilation complete.
