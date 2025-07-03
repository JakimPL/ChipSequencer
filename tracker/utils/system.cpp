#include "system.hpp"

std::pair<int, std::string> run_command(const std::string &command) {
    const std::string full_command = command + " 2>&1";

#ifdef _WIN32
    FILE *pipe = _popen(full_command.c_str(), "r");
#else
    FILE *pipe = popen(full_command.c_str(), "r");
#endif

    if (pipe == nullptr) {
        return {-1, "Failed to execute command"};
    }

    std::string output;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

#ifdef _WIN32
    int exit_code = _pclose(pipe);
#else
    int exit_code = pclose(pipe);
#endif

    return {exit_code, output};
}
