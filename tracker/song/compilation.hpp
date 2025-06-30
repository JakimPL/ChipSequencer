#pragma once

#include <cstdint>

enum class CompilationScheme : uint8_t {
    Compressed,
    Uncompressed,
    Debug
};

enum class CompilationTarget : uint8_t {
    Linux,
    Windows,
};

#ifdef _WIN32
#define DefaultCompilationTarget CompilationTarget::Windows
#else
#define DefaultCompilationTarget CompilationTarget::Linux
#endif
