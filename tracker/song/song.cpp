#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <zlib.h>
#include <vector>
#include <memory>
#include <stdexcept>

#include "song.hpp"
#include "../utils.hpp"

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

nlohmann::json Song::create_header_json() const {
    nlohmann::json json;
    json["title"] = header.title;
    json["author"] = header.author;
    json["version"] = header.version;
    json["bpm"] = bpm;
    json["normalizer"] = normalizer;
    json["output_channels"] = output_channels;
    json["length"] = song_length;

    return json;
}

void Song::compress_directory(const std::string &directory, const std::string &output_file) const {
    std::string tar_command = "tar -cvf " + output_file + " -C " +
                              std::filesystem::path(directory).string() + " .";

    int result = system(tar_command.c_str());
    if (result != 0) {
        throw std::runtime_error("Failed to create tar.gz archive. Error code: " + std::to_string(result));
    }
}

void Song::export_asm_file(const std::string &directory) const {
    std::string asm_content = generate_asm_file();
    std::ofstream asm_file(directory + "/song.asm");
    asm_file << asm_content;
    asm_file.close();
}

void Song::export_header(const std::string &directory) const {
    nlohmann::json header = create_header_json();
    std::ofstream header_file(directory + "/header.json");
    header_file << header.dump(4);
    header_file.close();
}

void Song::save_to_file(const std::string &filename) {
    std::string temp_dir = "temp";
    std::filesystem::create_directory(temp_dir);

    try {
        export_asm_file(temp_dir);
        export_header(temp_dir);
        export_vector(temp_dir + "/envelopes.bin", envelopes, {sizeof(Envelope)});
        export_vector(temp_dir + "/channels.bin", channels, {sizeof(Channel)});
        export_vector(temp_dir + "/oscillators.bin", oscillators, get_struct_sizes(oscillators));
        export_vector(temp_dir + "/dsps.bin", dsps, get_struct_sizes(dsps));
        export_structure(temp_dir + "/sequences.bin", sequences);
        export_structure(temp_dir + "/orders.bin", orders);
        export_structure(temp_dir + "/wavetables.bin", wavetables);
        compress_directory(temp_dir, filename);
        // std::filesystem::remove_all(temp_dir);
    } catch (const std::exception &e) {
        // std::filesystem::remove_all(temp_dir);
        throw;
    }
}