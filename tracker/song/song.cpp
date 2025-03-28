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

void Song::generate_header_vector(std::stringstream &asm_content, const std::string &name, const std::string &short_name, const size_t size) const {
    asm_content << name << "s:\n";
    for (size_t i = 0; i < size; i++) {
        asm_content << "global " << name << "s." << name << "_" << i << ":\n";
        asm_content << "." << name << "_" << i << ":\n";
        asm_content << "incbin \"song\\" << get_element_path(short_name + "s", short_name, i, '\\') << "\"\n";
    }
}

std::string Song::generate_asm_file() const {
    std::stringstream asm_content;
    asm_content << "SEGMENT_DATA\n";
    asm_content << "bpm:\n";
    asm_content << "dw " << bpm << "\n";
    asm_content << "normalizer:\n";
    asm_content << "dd " << normalizer << "\n\n";

    generate_header_vector(asm_content, "envelope", "envel", envelopes.size());
    generate_header_vector(asm_content, "sequence", "seq", sequences.size());
    generate_header_vector(asm_content, "order", "order", orders.size());
    generate_header_vector(asm_content, "oscillator", "osc", oscillators.size());
    generate_header_vector(asm_content, "wavetable", "wave", wavetables.size());
    generate_header_vector(asm_content, "dsp", "dsp", dsps.size());
    generate_header_vector(asm_content, "channel", "chan", channels.size());

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
    json["envelopes"] = envelopes.size();
    json["sequences"] = sequences.size();
    json["orders"] = orders.size();
    json["wavetables"] = wavetables.size();
    json["oscillators"] = oscillators.size();
    json["dsps"] = dsps.size();
    json["channels"] = channels.size();

    return json;
}

void Song::set_link(Link &link, void *item, const u_int8_t i) const {
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
    link.id = i;
    link.item = item;
    link.pointer = link.base + link.offset;
    link.assign_output();
}

void Song::set_links() {
    for (size_t i = 0; i < channels.size(); i++) {
        Link &link = links[0][i];
        void *item = channels[i];
        set_link(link, item, i);
    }
    for (size_t i = 0; i < dsps.size(); i++) {
        Link &link = links[1][i];
        void *item = dsps[i];
        set_link(link, item, i);
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

std::string Song::get_element_path(const std::string &directory, const std::string prefix, const size_t i, const char separator) const {
    return directory + separator + prefix + "_" + std::to_string(i) + ".bin";
}

void Song::serialize_channel(std::ofstream &file, Channel *channel) const {
    uint16_t null = 0;
    write_data(file, &channel->envelope_index, sizeof(channel->envelope_index));
    write_data(file, &channel->order_index, sizeof(channel->order_index));
    write_data(file, &channel->oscillator_index, sizeof(channel->oscillator_index));
    write_data(file, &channel->pitch, sizeof(channel->pitch));
    write_data(file, &null, sizeof(null));
    write_data(file, &channel->output_flag, sizeof(channel->output_flag));
}

void Song::serialize_dsp(std::ofstream &file, void *dsp) const {
    const uint8_t *bytes = static_cast<const uint8_t *>(dsp);
    uint8_t dsp_type = bytes[1];
    uint16_t null = 0;
    if (dsp_type == EFFECT_DELAY) {
        DSPDelay *delay = reinterpret_cast<DSPDelay *>(dsp);
        uint8_t size = 12;
        write_data(file, &size, sizeof(size));
        write_data(file, &delay->effect_index, sizeof(delay->effect_index));
        write_data(file, &null, sizeof(null));
        write_data(file, &delay->output_flag, sizeof(delay->output_flag));
        write_data(file, &delay->dry, sizeof(delay->dry));
        write_data(file, &delay->wet, sizeof(delay->wet));
        write_data(file, &delay->feedback, sizeof(delay->feedback));
        write_data(file, &delay->delay_time, sizeof(delay->delay_time));
    } else if (dsp_type == EFFECT_GAINER) {
        DSPGainer *gainer = reinterpret_cast<DSPGainer *>(dsp);
        uint16_t null = 0;
        uint8_t size = 6;
        write_data(file, &size, sizeof(size));
        write_data(file, &gainer->effect_index, sizeof(gainer->effect_index));
        write_data(file, &null, sizeof(null));
        write_data(file, &gainer->output_flag, sizeof(gainer->output_flag));
        write_data(file, &gainer->volume, sizeof(gainer->volume));
    } else if (dsp_type == EFFECT_FILTER) {
        DSPFilter *filter = reinterpret_cast<DSPFilter *>(dsp);
        uint16_t null = 0;
        uint8_t size = 6;
        write_data(file, &size, sizeof(size));
        write_data(file, &filter->effect_index, sizeof(filter->effect_index));
        write_data(file, &null, sizeof(null));
        write_data(file, &filter->output_flag, sizeof(filter->output_flag));
        write_data(file, &filter->frequency, sizeof(filter->frequency));
    } else {
        throw std::runtime_error("Unknown DSP type: " + std::to_string(dsp_type));
    }
}

void Song::export_asm_file(const std::string &directory) const {
    std::string asm_content = generate_asm_file();
    std::ofstream asm_file(directory + "/data.asm");
    asm_file << asm_content;
    asm_file.close();
}

void Song::export_header(const std::string &directory) const {
    nlohmann::json header = create_header_json();
    std::ofstream header_file(directory + "/header.json");
    header_file << header.dump(4);
    header_file.close();
}

template <typename T>
void Song::export_series(const std::string &song_dir, const std::string &prefix, const std::vector<T> &series, const std::vector<size_t> &sizes) const {
    std::filesystem::path series_dir = song_dir + "/" + prefix + "s";
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < series.size(); i++) {
        std::string filename = get_element_path(series_dir.string(), prefix, i);
        export_data(filename, series[i], sizes[i % sizes.size()]);
    }
}

void Song::export_channels(const std::string &directory) const {
    std::filesystem::path series_dir = directory + "/chans";
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < channels.size(); i++) {
        std::string filename = get_element_path(series_dir, "chan", i);
        std::ofstream file(filename, std::ios::binary);

        Channel *channel = channels[i];
        uint16_t null = 0;
        serialize_channel(file, channel);
        file.close();
    }
}

void Song::export_dsps(const std::string &directory) const {
    std::filesystem::path dsps_dir = directory + "/dsps";
    std::filesystem::create_directories(dsps_dir);

    for (size_t i = 0; i < dsps.size(); i++) {
        std::string filename = get_element_path(dsps_dir, "dsp", i);
        std::ofstream file(filename, std::ios::binary);

        void *dsp = dsps[i];
        serialize_dsp(file, dsp);
        file.close();
    }
}

template <typename T>
void Song::export_arrays(const std::string &directory, const std::string &prefix, const std::vector<T> &arrays) const {
    std::filesystem::path series_dir = directory + "/" + prefix + "s";
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < arrays.size(); i++) {
        std::string filename = get_element_path(series_dir, prefix, i);
        std::ofstream file(filename, std::ios::binary);
        const T element = arrays[i];
        element->serialize(file);
        file.close();
    }
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

void Song::save_to_file(const std::string &filename, const bool compile) const {
    std::filesystem::path temp_base = std::filesystem::temp_directory_path() / "chipsequencer";
    std::filesystem::path song_path = temp_base / "song";
    if (std::filesystem::exists(temp_base)) {
        std::filesystem::remove_all(temp_base);
    }
    std::filesystem::create_directories(song_path);
    std::string song_dir = song_path.string();

    try {
        export_asm_file(song_dir);
        export_header(song_dir);
        export_series(song_dir, "envel", envelopes, {sizeof(Envelope)});
        export_channels(song_dir);
        export_series(song_dir, "osc", oscillators, get_struct_sizes(oscillators));
        export_dsps(song_dir);
        export_arrays(song_dir, "seq", sequences);
        export_arrays(song_dir, "order", orders);
        export_arrays(song_dir, "wave", wavetables);
        export_offsets(song_dir + "/offsets.bin");
        export_links(song_dir + "/links.bin");
        if (compile) {
            compile_sources(temp_base.string());
        }

        compress_directory(song_dir, filename);
        std::filesystem::remove_all(temp_base);
    } catch (const std::exception &e) {
        std::filesystem::remove_all(temp_base);
        throw;
    }
}