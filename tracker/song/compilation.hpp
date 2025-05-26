#ifndef SONG_COMPILATION_HPP
#define SONG_COMPILATION_HPP

#include <cstdint>

enum class CompilationScheme : uint8_t {
    Compressed,
    Uncompressed,
    Debug
};

enum class CompilationTarget : uint8_t {
    Linux,
};

#endif // SONG_COMPILATION_HPP
