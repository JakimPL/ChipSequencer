#include "temp.hpp"

std::pair<std::filesystem::path, std::filesystem::path> prepare_temp_directory(const bool clear_temp) {
    std::filesystem::path temp_base = std::filesystem::temp_directory_path() / "chipsequencer";
    std::filesystem::path song_path = temp_base / "song";

    remove_temp_directory(temp_base, clear_temp);
    std::filesystem::create_directories(song_path);

    return {temp_base, song_path};
}

void remove_temp_directory(const std::filesystem::path &directory, const bool clear_temp) {
    if (clear_temp && std::filesystem::exists(directory)) {
        std::filesystem::remove_all(directory);
    }
}
