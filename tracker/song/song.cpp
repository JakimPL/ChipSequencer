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
    const std::string extract_command = "tar -xf \"" + output_file + "\" -C \"" + directory + "\"";
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
    const uint8_t dsp_type = bytes[1];
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

Channel *Song::deserialize_channel(std::ifstream &file) const {
    Channel *channel = new Channel();
    read_data(file, &channel->envelope_index, sizeof(channel->envelope_index));
    read_data(file, &channel->order_index, sizeof(channel->order_index));
    read_data(file, &channel->oscillator_index, sizeof(channel->oscillator_index));
    read_data(file, &channel->pitch, sizeof(channel->pitch));
    file.seekg(sizeof(uint16_t), std::ios::cur);
    read_data(file, &channel->output_flag, sizeof(channel->output_flag));
    channel->output = &output;
    return channel;
}

void *Song::deserialize_dsp(std::ifstream &file) const {
    uint8_t size, effect_type;
    read_data(file, &size, sizeof(size));
    read_data(file, &effect_type, sizeof(effect_type));

    if (effect_type == EFFECT_DELAY) {
        DSPDelay *delay = new DSPDelay();
        delay->dsp_size = DSP_DELAY_SIZE;
        delay->output = &output;
        file.seekg(sizeof(uint16_t), std::ios::cur);
        read_data(file, &delay->output_flag, sizeof(delay->output_flag));
        read_data(file, &delay->dry, sizeof(delay->dry));
        read_data(file, &delay->wet, sizeof(delay->wet));
        read_data(file, &delay->feedback, sizeof(delay->feedback));
        read_data(file, &delay->delay_time, sizeof(delay->delay_time));
        return reinterpret_cast<void *>(delay);
    } else if (effect_type == EFFECT_GAINER) {
        DSPGainer *gainer = new DSPGainer();
        gainer->dsp_size = DSP_GAINER_SIZE;
        gainer->output = &output;
        file.seekg(sizeof(uint16_t), std::ios::cur);
        read_data(file, &gainer->output_flag, sizeof(gainer->output_flag));
        read_data(file, &gainer->volume, sizeof(gainer->volume));
        return reinterpret_cast<void *>(gainer);
    } else if (effect_type == EFFECT_FILTER) {
        DSPFilter *filter = new DSPFilter();
        filter->dsp_size = DSP_FILTER_SIZE;
        filter->output = &output;
        file.seekg(sizeof(uint16_t), std::ios::cur);
        read_data(file, &filter->output_flag, sizeof(filter->output_flag));
        read_data(file, &filter->frequency, sizeof(filter->frequency));
        return reinterpret_cast<void *>(filter);
    } else {
        throw std::runtime_error("Unknown DSP type: " + std::to_string(effect_type));
    }
}

void *Song::deserialize_oscillator(std::ifstream &file) const {
    uint8_t size, oscillator_type;
    read_data(file, &size, sizeof(size));
    read_data(file, &oscillator_type, sizeof(oscillator_type));

    if (oscillator_type == OSCILLATOR_SQUARE) {
        OscillatorSquare *oscillator = new OscillatorSquare();
        read_data(file, &oscillator->duty_cycle, sizeof(oscillator->duty_cycle));
        return reinterpret_cast<void *>(oscillator);
    } else if (oscillator_type == OSCILLATOR_SAW) {
        OscillatorSaw *oscillator = new OscillatorSaw();
        return reinterpret_cast<void *>(oscillator);
    } else if (oscillator_type == OSCILLATOR_SINE) {
        OscillatorSine *oscillator = new OscillatorSine();
        return reinterpret_cast<void *>(oscillator);
    } else if (oscillator_type == OSCILLATOR_WAVETABLE) {
        OscillatorWavetable *oscillator = new OscillatorWavetable();
        read_data(file, &oscillator->wavetable_index, sizeof(oscillator->wavetable_index));
        return reinterpret_cast<void *>(oscillator);
    } else {
        throw std::runtime_error("Unknown oscillator type: " + std::to_string(oscillator_type));
    }
}

void Song::export_asm_file(const std::string &directory) const {
    std::string asm_content = generate_asm_file();
    std::ofstream asm_file(directory + "/data.asm");
    asm_file << asm_content;
    asm_file.close();
}

void Song::export_header(const std::string &directory) const {
    const nlohmann::json header = create_header_json();
    std::ofstream header_file(directory + "/header.json");
    header_file << header.dump(4);
    header_file.close();
}

template <typename T>
void Song::export_series(const std::string &song_dir, const std::string &prefix, const std::vector<T> &series, const std::vector<size_t> &sizes) const {
    const std::filesystem::path series_dir = song_dir + "/" + prefix + "s";
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < series.size(); i++) {
        const std::string filename = get_element_path(series_dir.string(), prefix, i);
        export_data(filename, series[i], sizes[i % sizes.size()]);
    }
}

void Song::export_channels(const std::string &directory) const {
    const std::filesystem::path series_dir = directory + "/chans";
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < channels.size(); i++) {
        const std::string filename = get_element_path(series_dir, "chan", i);
        std::ofstream file(filename, std::ios::binary);
        Channel *channel = channels[i];
        serialize_channel(file, channel);
        file.close();
    }
}

void Song::export_dsps(const std::string &directory) const {
    const std::filesystem::path dsps_dir = directory + "/dsps";
    std::filesystem::create_directories(dsps_dir);

    for (size_t i = 0; i < dsps.size(); i++) {
        const std::string filename = get_element_path(dsps_dir, "dsp", i);
        std::ofstream file(filename, std::ios::binary);

        void *dsp = dsps[i];
        serialize_dsp(file, dsp);
        file.close();
    }
}

template <typename T>
void Song::export_arrays(const std::string &directory, const std::string &prefix, const std::vector<T> &arrays) const {
    const std::filesystem::path series_dir = directory + "/" + prefix + "s";
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < arrays.size(); i++) {
        const std::string filename = get_element_path(series_dir, prefix, i);
        std::ofstream file(filename, std::ios::binary);
        const T element = arrays[i];
        element->serialize(file);
        file.close();
    }
}

void Song::export_offsets(const std::string &filename) const {
    std::ofstream file(filename, std::ios::binary);
    for (size_t i = 0; i < dsps.size(); i++) {
        const uint16_t offset = buffer_offsets[i];
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
    envelopes.clear();
    for (size_t i = 0; i < envelope_count; i++) {
        const std::string filename = get_element_path(song_dir + "/envels", "envel", i);
        Envelope *envelope = new Envelope();
        std::ifstream file(filename, std::ios::binary);
        read_data(file, envelope, sizeof(Envelope));
        envelopes.push_back(envelope);
        file.close();
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
        const std::string filename = get_element_path(song_dir + "/waves", "wave", i);
        std::ifstream file(filename, std::ios::binary);
        Wavetable *wavetable = Wavetable::deserialize(file);
        wavetables.push_back(wavetable);
        file.close();
    }
}

void Song::import_oscillators(const std::string &song_dir, const nlohmann::json &json) {
    const size_t oscillator_count = json["oscillators"];
    oscillators.clear();
    for (size_t i = 0; i < oscillator_count; i++) {
        const std::string filename = get_element_path(song_dir + "/oscs", "osc", i);
        std::ifstream file(filename, std::ios::binary);
        void *oscillator = deserialize_oscillator(file);
        oscillators.push_back(oscillator);
        file.close();
    }
}

void Song::import_dsps(const std::string &song_dir, const nlohmann::json &json) {
    const size_t dsp_count = json["dsps"];
    dsps.clear();
    for (size_t i = 0; i < dsp_count; i++) {
        const std::string filename = get_element_path(song_dir + "/dsps", "dsp", i);
        std::ifstream file(filename, std::ios::binary);
        void *dsp = deserialize_dsp(file);
        dsps.push_back(dsp);
        file.close();
    }
}

void Song::import_channels(const std::string &song_dir, const nlohmann::json &json) {
    const size_t channel_count = json["channels"];
    channels.clear();
    for (size_t i = 0; i < channel_count; i++) {
        const std::string filename = get_element_path(song_dir + "/chans", "chan", i);
        std::ifstream file(filename, std::ios::binary);
        Channel *channel = deserialize_channel(file);
        channels.push_back(channel);
        file.close();
    }
}

void Song::import_offsets(const std::string &song_dir, const nlohmann::json &json) {
    const size_t dsp_count = json["dsps"];
    const std::string offsets_filename = song_dir + "/offsets.bin";
    std::ifstream file(offsets_filename, std::ios::binary);

    delete[] current_offsets;
    current_offsets = new uint16_t[dsp_count];
    buffer_offsets = current_offsets;
    for (size_t i = 0; i < dsp_count; i++) {
        read_data(file, &buffer_offsets[i], sizeof(uint16_t));
    }

    file.close();
}

void Song::import_links(const std::string &song_dir, const nlohmann::json &json) {
    const size_t channel_count = json["channels"];
    const size_t dsp_count = json["dsps"];
    const std::string links_filename = song_dir + "/links.bin";
    std::ifstream file(links_filename, std::ios::binary);

    links.clear();
    links.resize(2);

    links[0].clear();
    for (size_t i = 0; i < channel_count; i++) {
        Link link;
        link.deserialize(file);
        links[0].push_back(link);
    }

    links[1].clear();
    for (size_t i = 0; i < dsp_count; i++) {
        Link link;
        link.deserialize(file);
        links[1].push_back(link);
    }

    set_links();
    file.close();
}

void Song::clear_data() {
    for (auto *envelope : envelopes) {
        delete envelope;
    }
    for (auto *sequence : sequences) {
        delete sequence;
    }
    for (auto *sequence : sequences) {
        delete sequence;
    }
    for (auto *order : orders) {
        delete order;
    }
    for (auto *wavetable : wavetables) {
        delete wavetable;
    }
    for (auto *oscillator : oscillators) {
        const uint8_t *bytes = static_cast<const uint8_t *>(oscillator);
        const uint8_t dsp_type = bytes[1];
        if (dsp_type == OSCILLATOR_SQUARE) {
            delete static_cast<OscillatorSquare *>(oscillator);
        } else if (dsp_type == OSCILLATOR_SAW) {
            delete static_cast<OscillatorSaw *>(oscillator);
        } else if (dsp_type == OSCILLATOR_SINE) {
            delete static_cast<OscillatorSine *>(oscillator);
        } else if (dsp_type == OSCILLATOR_WAVETABLE) {
            delete static_cast<OscillatorWavetable *>(oscillator);
        }
    }
    for (auto *dsp : dsps) {
        const uint8_t *bytes = static_cast<const uint8_t *>(dsp);
        const uint8_t dsp_type = bytes[1];
        if (dsp_type == EFFECT_DELAY) {
            delete static_cast<DSPDelay *>(dsp);
        } else if (dsp_type == EFFECT_GAINER) {
            delete static_cast<DSPGainer *>(dsp);
        } else if (dsp_type == EFFECT_FILTER) {
            delete static_cast<DSPFilter *>(dsp);
        }
    }
    for (auto *channel : channels) {
        delete channel;
    }

    envelopes.clear();
    sequences.clear();
    orders.clear();
    oscillators.clear();
    wavetables.clear();
    dsps.clear();
    channels.clear();
    links.clear();
    links.resize(2);
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
        import_envelopes(song_dir, json);
        import_sequences(song_dir, json);
        import_orders(song_dir, json);
        import_wavetables(song_dir, json);
        import_channels(song_dir, json);
        import_oscillators(song_dir, json);
        import_dsps(song_dir, json);
        import_links(song_dir, json);
        import_offsets(song_dir, json);

        std::filesystem::remove_all(temp_base);
    } catch (const std::exception &e) {
        // std::filesystem::remove_all(temp_base);
        throw;
    }
}