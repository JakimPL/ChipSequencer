#pragma once

#include <cstdint>

enum class CompilationScheme : uint8_t {
    Compressed,
    Uncompressed,
    Debug
};

enum class CompilationTarget : uint8_t {
    Linux,
};
