#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <zlib.h>

#include "../utils.hpp"
#include "data.hpp"
#include "song.hpp"

std::string Song::generate_asm_file() const {
    std::stringstream asm_content;
    asm_content << "SEGMENT_DATA\n";
    asm_content << "bpm:\n";
    asm_content << "dw " << bpm << "\n";
    asm_content << "normalizer:\n";
    asm_content << "dd " << normalizer << "\n\n";

    asm_content << "envelopes:\n";
    asm_content << "incbin \"song\\envels.bin\"\n\n";

    asm_content << "sequences:\n";
    asm_content << "incbin \"song\\seqs.bin\"\n\n";

    asm_content << "orders:\n";
    asm_content << "incbin \"song\\orders.bin\"\n\n";

    asm_content << "oscillators:\n";
    asm_content << "incbin \"song\\oscs.bin\"\n\n";

    asm_content << "wavetables:\n";
    asm_content << "incbin \"song\\waves.bin\"\n\n";

    asm_content << "dsps:\n";
    asm_content << "incbin \"song\\dsps.bin\"\n\n";

    asm_content << "channels:\n";
    asm_content << "incbin \"song\\channels.bin\"\n\n";

    asm_content << "buffer_offsets:\n";
    asm_content << "incbin \"song\\offsets.bin\"\n";

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

void Song::set_link(Link &link, void *item) const {
    switch (link.target) {
    case Target::OUTPUT_CHANNEL:
        link.base = &output;
        break;
    case Target::DSP_CHANNEL:
        link.base = &dsp_input;
        break;
    case Target::ENVELOPE:
        link.base = envelopes[link.index];
        break;
    case Target::SEQUENCE:
        link.base = sequences[link.index];
        break;
    case Target::ORDER:
        link.base = orders[link.index];
        break;
    case Target::OSCILLATOR:
        link.base = oscillators[link.index];
        break;
    case Target::WAVETABLE:
        link.base = wavetables[link.index];
        break;
    case Target::DSP:
        link.base = dsps[link.index];
        break;
    case Target::CHANNEL:
        link.base = channels[link.index];
        break;
    }
    link.item = item;
    link.pointer = link.base + link.offset;
    link.assign_output();
}

void Song::set_links() {
    for (size_t i = 0; i < channels.size(); i++) {
        Link &link = links[0][i];
        void *item = channels[i];
        set_link(link, item);
    }
    for (size_t i = 0; i < dsps.size(); i++) {
        Link &link = links[1][i];
        void *item = dsps[i];
        set_link(link, item);
    }
}

int Song::run_command(const std::string &command) const {
    return system(command.c_str());
}

void Song::compress_directory(const std::string &directory, const std::string &output_file) const {
    std::string tar_command = "tar -cvf " + output_file + " -C \"" +
                              std::filesystem::path(directory).string() + "\" .";
    run_command(tar_command);
}

void Song::compile_sources(const std::string &directory) const {
    run_command("python scripts/compile.py \"" + directory + "\"");
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

void Song::export_offsets(const std::string &filename) const {
    std::ofstream file(filename, std::ios::binary);
    for (size_t i = 0; i < dsps.size(); i++) {
        uint16_t offset = buffer_offsets[i];
        write_data(file, &offset, sizeof(offset));
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
    std::filesystem::path temp_base = std::filesystem::temp_directory_path() / "chipsequencer_temp";
    std::filesystem::path song_path = temp_base / "song";
    if (std::filesystem::exists(temp_base)) {
        std::filesystem::remove_all(temp_base);
    }
    std::filesystem::create_directories(song_path);
    std::string song_dir = song_path.string();

    try {
        export_asm_file(song_dir);
        export_header(song_dir);
        export_vector(song_dir + "/envels.bin", envelopes, {sizeof(Envelope)});
        export_channels(song_dir + "/channels.bin");
        export_vector(song_dir + "/oscs.bin", oscillators, get_struct_sizes(oscillators));
        export_dsps(song_dir + "/dsps.bin");
        export_structure(song_dir + "/seqs.bin", sequences);
        export_structure(song_dir + "/orders.bin", orders);
        export_structure(song_dir + "/waves.bin", wavetables);
        export_offsets(song_dir + "/offsets.bin");
        export_links(song_dir + "/links.bin");
        compile_sources(temp_base.string());
        compress_directory(song_dir, filename);
        // std::filesystem::remove_all(temp_dir);
    } catch (const std::exception &e) {
        // std::filesystem::remove_all(temp_dir);
        throw;
    }
}