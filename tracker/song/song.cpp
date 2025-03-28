#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <zlib.h>

#include "../utils/file.hpp"
#include "../utils/temp.hpp"
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

nlohmann::json Song::import_header(const std::string &filename) {
    std::ifstream file(filename);
    nlohmann::json json;
    file >> json;
    header.title = json["title"];
    header.author = json["author"];
    header.version = json["version"];
    bpm = json["bpm"];
    normalizer = json["normalizer"];
    output_channels = json["output_channels"];
    song_length = json["length"];
    file.close();
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

void Song::decompress_archive(const std::string &output_file, const std::string &directory) {
    std::string extract_command = "tar -xf \"" + output_file + "\" -C \"" + directory + "\"";
    if (run_command(extract_command) != 0) {
        throw std::runtime_error("Failed to extract song file: " + output_file);
    }
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

void Song::import_envelopes(const std::string &song_dir, const nlohmann::json &json) {
    const size_t envelope_count = json["envelopes"];
    for (size_t i = 0; i < envelope_count; i++) {
        const std::string filename = get_element_path(song_dir, "envel", i);
        auto envelope = new Envelope();
        std::ifstream file(filename, std::ios::binary);
        file.read(reinterpret_cast<char *>(envelope), sizeof(Envelope));
        envelopes.push_back(envelope);
    }
}

void Song::import_sequences(const std::string &song_dir, const nlohmann::json &json) {
    const size_t sequence_count = json["sequences"];
    sequences.clear();
    for (size_t i = 0; i < sequence_count; i++) {
        const std::string filename = get_element_path(song_dir + "/seqs", "seq", i);
        std::ifstream file(filename, std::ios::binary);
        Sequence *sequence = Sequence::deserialize(file);
        sequences.push_back(sequence);
        file.close();
    }
}

void Song::import_orders(const std::string &song_dir, const nlohmann::json &json) {
    const size_t order_count = json["orders"];
    orders.clear();
    for (size_t i = 0; i < order_count; i++) {
        const std::string filename = get_element_path(song_dir + "/orders", "order", i);
        std::ifstream file(filename, std::ios::binary);
        Order *order = Order::deserialize(file);
        orders.push_back(order);
        file.close();
    }
}

void Song::import_wavetables(const std::string &song_dir, const nlohmann::json &json) {
    const size_t wavetable_count = json["wavetables"];
    wavetables.clear();
    for (size_t i = 0; i < wavetable_count; i++) {
        const std::string filename = get_element_path(song_dir, "wave", i);
        std::ifstream file(filename, std::ios::binary);
        Wavetable *wavetable = Wavetable::deserialize(file);
        wavetables.push_back(wavetable);
        file.close();
    }
}

void Song::import_oscillators(const std::string &song_dir, const nlohmann::json &json) {
    const size_t oscillator_count = json["oscillators"];
    return;
    for (size_t i = 0; i < oscillator_count; i++) {
        const std::string filename = get_element_path(song_dir, "osc", i);
        std::ifstream file(filename, std::ios::binary);

        uint8_t type;
        file.read(reinterpret_cast<char *>(&type), sizeof(type));
        file.seekg(0);

        void *oscillator = nullptr;
        switch (type) {
        case OSCILLATOR_SQUARE:
            oscillator = new OscillatorSquare();
            break;
        case OSCILLATOR_SAW:
            oscillator = new OscillatorSaw();
            break;
        case OSCILLATOR_SINE:
            oscillator = new OscillatorSine();
            break;
        case OSCILLATOR_WAVETABLE:
            oscillator = new OscillatorWavetable();
            break;
        default:
            throw std::runtime_error("Unknown oscillator type: " + std::to_string(type));
        }

        // const size_t size = get_struct_sizes({oscillator})[0];
        // file.read(reinterpret_cast<char *>(oscillator), size);
        // oscillators.push_back(oscillator);
    }
}

void Song::import_dsps(const std::string &song_dir, const nlohmann::json &json) {
    const size_t dsp_count = json["dsps"];
    return;
    for (size_t i = 0; i < dsp_count; i++) {
        const std::string filename = get_element_path(song_dir + "/dsps", "dsp", i);
        std::ifstream file(filename, std::ios::binary);

        uint8_t size, effect_index;
        file.read(reinterpret_cast<char *>(&size), sizeof(size));
        file.read(reinterpret_cast<char *>(&effect_index), sizeof(effect_index));
        file.seekg(0);

        void *dsp = nullptr;
        if (effect_index == EFFECT_DELAY) {
            dsp = new DSPDelay();
            DSPDelay *delay = reinterpret_cast<DSPDelay *>(dsp);

            file.seekg(sizeof(uint8_t));
            file.read(reinterpret_cast<char *>(&delay->effect_index), sizeof(delay->effect_index));
            file.seekg(sizeof(uint16_t), std::ios::cur); // Skip null field
            file.read(reinterpret_cast<char *>(&delay->output_flag), sizeof(delay->output_flag));
            file.read(reinterpret_cast<char *>(&delay->dry), sizeof(delay->dry));
            file.read(reinterpret_cast<char *>(&delay->wet), sizeof(delay->wet));
            file.read(reinterpret_cast<char *>(&delay->feedback), sizeof(delay->feedback));
            file.read(reinterpret_cast<char *>(&delay->delay_time), sizeof(delay->delay_time));
        } else if (effect_index == EFFECT_GAINER) {
            dsp = new DSPGainer();
            DSPGainer *gainer = reinterpret_cast<DSPGainer *>(dsp);

            file.seekg(sizeof(uint8_t));
            file.read(reinterpret_cast<char *>(&gainer->effect_index), sizeof(gainer->effect_index));
            file.seekg(sizeof(uint16_t), std::ios::cur); // Skip null field
            file.read(reinterpret_cast<char *>(&gainer->output_flag), sizeof(gainer->output_flag));
            file.read(reinterpret_cast<char *>(&gainer->volume), sizeof(gainer->volume));
        } else if (effect_index == EFFECT_FILTER) {
            dsp = new DSPFilter();
            DSPFilter *filter = reinterpret_cast<DSPFilter *>(dsp);

            file.seekg(sizeof(uint8_t));
            file.read(reinterpret_cast<char *>(&filter->effect_index), sizeof(filter->effect_index));
            file.seekg(sizeof(uint16_t), std::ios::cur); // Skip null field
            file.read(reinterpret_cast<char *>(&filter->output_flag), sizeof(filter->output_flag));
            file.read(reinterpret_cast<char *>(&filter->frequency), sizeof(filter->frequency));
        } else {
            throw std::runtime_error("Unknown DSP type: " + std::to_string(effect_index));
        }

        dsps.push_back(dsp);
    }
}

void Song::import_channels(const std::string &song_dir, const nlohmann::json &json) {
    const size_t channel_count = json["channels"];
    return;
    for (size_t i = 0; i < channel_count; i++) {
        const std::string filename = get_element_path(song_dir + "/chans", "chan", i);
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open channel file: " + filename);
        }

        auto channel = new Channel();
        file.read(reinterpret_cast<char *>(&channel->envelope_index), sizeof(channel->envelope_index));
        file.read(reinterpret_cast<char *>(&channel->order_index), sizeof(channel->order_index));
        file.read(reinterpret_cast<char *>(&channel->oscillator_index), sizeof(channel->oscillator_index));
        file.read(reinterpret_cast<char *>(&channel->pitch), sizeof(channel->pitch));
        file.seekg(sizeof(uint16_t), std::ios::cur);
        file.read(reinterpret_cast<char *>(&channel->output_flag), sizeof(channel->output_flag));
        channels.push_back(channel);
    }
}

void Song::import_offsets(const std::string &song_dir, const nlohmann::json &json) {
    const size_t dsp_count = json["dsps"];
    const std::string offsets_filename = song_dir + "/offsets.bin";
    std::ifstream offsets_file(offsets_filename, std::ios::binary);

    // buffer_offsets.clear();
    // for (size_t i = 0; i < dsp_count; i++) {
    //     uint16_t offset;
    //     offsets_file.read(reinterpret_cast<char *>(&offset), sizeof(offset));
    //     buffer_offsets.push_back(offset);
    // }
}

void Song::import_links(const std::string &song_dir, const nlohmann::json &json) {
    const size_t channel_count = json["channels"];
    const size_t dsp_count = json["dsps"];
    const std::string links_filename = song_dir + "/links.bin";
    std::ifstream links_file(links_filename, std::ios::binary);
    return;
    // for (size_t i = 0; i < channel_count; i++) {
    //     Link link;
    //     link.deserialize(links_file);
    //     links[0].push_back(link);
    // }

    // for (size_t i = 0; i < dsp_count; i++) {
    //     Link link;
    //     link.deserialize(links_file);
    //     links[1].push_back(link);
    // }
}

void Song::clear_data() {
    envelopes.clear();
    sequences.clear();
    orders.clear();
    oscillators.clear();
    wavetables.clear();
    dsps.clear();
    channels.clear();
    links[0].clear();
    links[1].clear();
}

void Song::save_to_file(const std::string &filename, const bool compile) const {
    const auto [temp_base, song_path] = prepare_temp_directory();
    const std::string song_dir = song_path.string();

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

void Song::load_from_file(const std::string &filename) {
    const auto [temp_base, song_path] = prepare_temp_directory();
    const std::string song_dir = song_path.string();

    try {
        decompress_archive(filename, song_dir);
        nlohmann::json json = import_header(song_dir + "/header.json");
        import_orders(song_dir, json);

        return;
        // clear_data();
        import_envelopes(song_dir, json);
        import_sequences(song_dir, json);
        import_orders(song_dir, json);
        import_wavetables(song_dir, json);
        import_oscillators(song_dir, json);
        import_dsps(song_dir, json);
        import_channels(song_dir, json);
        import_offsets(song_dir, json);
        import_links(song_dir, json);
        set_links();

        std::filesystem::remove_all(temp_base);
    } catch (const std::exception &e) {
        // std::filesystem::remove_all(temp_base);
        throw;
    }
}