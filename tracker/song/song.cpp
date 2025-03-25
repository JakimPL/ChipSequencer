#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <zlib.h>

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

void Song::export_channels(const std::string &filename) const {
    std::ofstream file(filename, std::ios::binary);
    for (const Channel *channel : channels) {
        channel->serialize(file);
    }
    file.close();
}

void Song::export_dsps(const std::string &filename) const {
    std::ofstream file(filename, std::ios::binary);
    for (void *dsp : dsps) {
        const uint8_t *bytes = static_cast<const uint8_t *>(dsp);
        uint8_t dsp_type = bytes[1];
        switch (dsp_type) {
        case EFFECT_DELAY:
            reinterpret_cast<DSPDelay *>(dsp)->serialize(file);
            break;
        case EFFECT_GAINER:
            reinterpret_cast<DSPGainer *>(dsp)->serialize(file);
            break;
        case EFFECT_FILTER:
            reinterpret_cast<DSPFilter *>(dsp)->serialize(file);
            break;
        default:
            throw std::runtime_error("Unknown DSP type: " + std::to_string(dsp_type));
        }
    }
    file.close();
}

void Song::export_links(const std::string &filename) const {
    std::ofstream file(filename, std::ios::binary);
    for (const auto &link : links[0]) {
        link.serialize(file);
    }
    for (const auto &link : links[1]) {
        link.serialize(file);
    }
    file.close();
}

void Song::save_to_file(const std::string &filename) {
    std::string temp_dir = "temp";
    std::filesystem::create_directory(temp_dir);

    try {
        export_asm_file(temp_dir);
        export_header(temp_dir);
        export_vector(temp_dir + "/envelopes.bin", envelopes, {sizeof(Envelope)});
        export_channels(temp_dir + "/channels.bin");
        export_vector(temp_dir + "/oscillators.bin", oscillators, get_struct_sizes(oscillators));
        export_dsps(temp_dir + "/dsps.bin");
        export_structure(temp_dir + "/sequences.bin", sequences);
        export_structure(temp_dir + "/orders.bin", orders);
        export_structure(temp_dir + "/wavetables.bin", wavetables);
        export_links(temp_dir + "/links.bin");
        compress_directory(temp_dir, filename);
        // std::filesystem::remove_all(temp_dir);
    } catch (const std::exception &e) {
        // std::filesystem::remove_all(temp_dir);
        throw;
    }
}