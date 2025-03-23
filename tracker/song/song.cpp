#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <zlib.h>
#include <vector>
#include <memory>
#include <stdexcept>

#include "song.hpp"

std::string Song::generate_asm_file() const {
    std::stringstream asm_content;
    asm_content << "SEGMENT_DATA\n";
    asm_content << "bpm:\n";
    asm_content << "dw " << bpm << "\n";
    asm_content << "normalizer:\n";
    asm_content << "dd " << normalizer << "\n\n";

    asm_content << "envelopes:\n";
    asm_content << "incbin envelopes.bin\n\n";

    asm_content << "sequences:\n";
    asm_content << "incbin sequences.bin\n\n";

    asm_content << "orders:\n";
    asm_content << "incbin orders.bin\n\n";

    asm_content << "oscillators:\n";
    asm_content << "incbin oscillators.bin\n\n";

    asm_content << "wavetables:\n";
    asm_content << "incbin wavetables.bin\n\n";

    asm_content << "dsps:\n";
    asm_content << "incbin dsps.bin\n\n";

    asm_content << "channels:\n";
    asm_content << "incbin channels.bin\n\n";

    asm_content << "buffer_offsets:\n";
    asm_content << "incbin buffer_offsets.bin\n";

    return asm_content.str();
}

// Create header JSON file
nlohmann::json Song::create_header_json() const {
    nlohmann::json header;
    header["title"] = "Untitled Song";
    header["author"] = "Unknown";
    header["version"] = "1.0";
    header["bpm"] = bpm;
    header["normalizer"] = normalizer;
    header["output_channels"] = output_channels;

    return header;
}

template <typename T>
void Song::export_binary(const std::string &filename, const T *data, size_t size) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to create file: " + filename);
    }
    file.write(reinterpret_cast<const char *>(data), size);
    file.close();
}

void Song::export_envelopes(const std::string &filename) const {
    size_t count = 0;
    for (size_t i = 0; envelopes[i] != nullptr; i++) {
        count++;
    }

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to create file: " + filename);
    }

    file.write(reinterpret_cast<const char *>(&count), sizeof(count));
    for (size_t i = 0; i < count; i++) {
        file.write(reinterpret_cast<const char *>(envelopes[i]), sizeof(Envelope));
    }

    file.close();
}

void Song::compress_directory(const std::string &directory, const std::string &output_file) const {
    std::string tar_command = "tar -czf \"" + output_file + "\" -C \"" +
                              std::filesystem::path(directory).parent_path().string() +
                              "\" \"" + std::filesystem::path(directory).filename().string() + "\"";

    int result = system(tar_command.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to create tar.gz archive. Error code: " + std::to_string(result));
    }
}

void Song::save_to_file(const std::string &filename) {
    std::string temp_dir = "temp";
    std::filesystem::create_directory(temp_dir);

    try {
        std::string asm_content = generate_asm_file();
        std::ofstream asm_file(temp_dir + "/song.asm");
        asm_file << asm_content;
        asm_file.close();

        nlohmann::json header = create_header_json();
        std::ofstream header_file(temp_dir + "/header.json");
        header_file << header.dump(4);
        header_file.close();

        export_envelopes(temp_dir + "/envelopes.bin");
        compress_directory(temp_dir, filename);
        std::filesystem::remove_all(temp_dir);
    } catch (const std::exception &e) {
        std::filesystem::remove_all(temp_dir);
        throw;
    }
}