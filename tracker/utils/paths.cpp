#include <algorithm>

#include "paths.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

std::pair<std::filesystem::path, std::filesystem::path> prepare_temp_directory(const bool clean_temp) {
    const std::filesystem::path temp_base = std::filesystem::temp_directory_path() / "chipsequencer";
    const std::filesystem::path song_path = temp_base / "song";

    remove_temp_directory(temp_base, clean_temp);
    remove_temp_directory(song_path, true);
    std::filesystem::create_directories(song_path);

    return {temp_base, song_path};
}

void remove_temp_directory(const std::filesystem::path &directory, const bool clean_temp) {
    if (clean_temp && std::filesystem::exists(directory)) {
        std::filesystem::remove_all(directory);
    }
}

std::filesystem::path get_executable_directory() {
#ifdef _WIN32
    char path[MAX_PATH];
    DWORD result = GetModuleFileNameA(NULL, path, MAX_PATH);
    if (result > 0 && result < MAX_PATH) {
        return std::filesystem::path(path).parent_path();
    }

    return std::filesystem::current_path();
#else
    char path[PATH_MAX];
    const ssize_t count = readlink("/proc/self/exe", path, PATH_MAX - 1);
    if (count != -1) {
        path[count] = '\0';
        return std::filesystem::path(path).parent_path();
    }
    return std::filesystem::current_path();
#endif
}

std::filesystem::path get_base_path() {
    const std::filesystem::path exe_dir = get_executable_directory();
    return exe_dir.parent_path();
}

std::filesystem::path get_resource_path(const std::string &resource) {
    const std::filesystem::path base_path = get_base_path();
    std::filesystem::path resource_path = base_path / resource;

    if (std::filesystem::exists(resource_path)) {
        return resource_path;
    }

    std::filesystem::path fallback_path = std::filesystem::current_path() / resource;
    if (std::filesystem::exists(fallback_path)) {
        return fallback_path;
    }

    return resource_path;
}

std::filesystem::path get_core_path() {
    return get_resource_path("core");
}

std::filesystem::path get_scripts_path() {
    return get_resource_path("scripts");
}

std::string get_python_path() {
    const std::filesystem::path base_path = get_base_path();

#ifdef _WIN32
    const std::filesystem::path venv_python = base_path / "venv" / "Scripts" / "python.exe";
#else
    const std::filesystem::path venv_python = base_path / "venv" / "bin" / "python";
#endif

    std::string executable;
    if (std::filesystem::exists(venv_python)) {
        executable = venv_python.string();
    } else {
#ifdef _WIN32
        executable = "python";
#else
        executable = "python3";
#endif
    }

    return executable;
}

std::filesystem::path check_and_correct_path_by_extension(const std::filesystem::path &path, const std::string &extension) {
    std::filesystem::path new_path = path;
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != extension) {
        new_path.replace_extension(extension);
    }

    return new_path;
}
