#include "temp.hpp"

std::pair<std::filesystem::path, std::filesystem::path> prepare_temp_directory() {
    std::filesystem::path temp_base = std::filesystem::temp_directory_path() / "chipsequencer";
    std::filesystem::path song_path = temp_base / "song";

    if (std::filesystem::exists(temp_base)) {
        std::filesystem::remove_all(temp_base);
    }
    std::filesystem::create_directories(song_path);

    return {temp_base, song_path};
}
