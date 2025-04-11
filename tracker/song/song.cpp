#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <zlib.h>

#include "../general.hpp"
#include "../utils/file.hpp"
#include "../utils/temp.hpp"
#include "data.hpp"
#include "functions.hpp"
#include "song.hpp"

Song::Song() {
    set_buffer_offsets();
    new_song();
}

Song::~Song() {
}

void Song::new_song() {
    clear_data();
    bpm = DEFAULT_BPM;
    normalizer = DEFAULT_NORMALIZER;
    header = {
        "Unknown",
        "Untitled",
        TRACKER_VERSION
    };
    tuning = {
        DEFAULT_EDO,
        DEFAULT_A4_FREQUENCY
    };
    change_tuning(tuning.edo, tuning.a4_frequency);
    set_links();
}

void Song::save_to_file(const std::string &filename) {
    const auto [temp_base, song_path] = prepare_temp_directory();
    const std::string song_dir = song_path.string();

    try {
        calculate_song_length();
        set_links();
        export_all(song_dir);
        compress_directory(song_dir, filename);
        std::filesystem::remove_all(temp_base);
    } catch (const std::exception &exception) {
        std::filesystem::remove_all(temp_base);
        throw;
    }
}

void Song::load_from_file(const std::string &filename) {
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error("File does not exist: " + filename);
    }

    const auto [temp_base, song_path] = prepare_temp_directory();
    const std::string song_dir = song_path.string();

    try {
        decompress_archive(filename, song_dir);
        nlohmann::json json = import_header(song_dir + "/header.json");
        clear_data();
        import_all(song_dir, json);
        update_sizes();
        change_tuning(tuning.edo, tuning.a4_frequency);

        std::filesystem::remove_all(temp_base);
    } catch (const std::exception &exception) {
        std::filesystem::remove_all(temp_base);
        throw;
    }
}

void Song::compile(const std::string &filename, bool compress) const {
    const auto [temp_base, song_path] = prepare_temp_directory();
    const std::string song_dir = song_path.string();

    try {
        export_all(song_dir);
        compile_sources(temp_base.string(), filename, compress);
        std::filesystem::remove_all(temp_base);
    } catch (const std::exception &exception) {
        std::filesystem::remove_all(temp_base);
        throw;
    }
}

void Song::set_title(const std::string &title) {
    if (title.length() >= MAX_STRING_LENGTH) {
        throw std::runtime_error("Song title is too long");
    }

    header.title = title;
}

void Song::set_author(const std::string &author) {
    if (author.length() >= MAX_STRING_LENGTH) {
        throw std::runtime_error("Song author is too long");
    }

    header.author = author;
}

void Song::change_tuning(const uint8_t new_edo, const double base_frequency) {
    scale_composer.compose(new_edo);
    frequency_table.calculate(base_frequency);
    double frequency = frequency_table.get_last_frequency();
    std::cout << "New tuning: " << static_cast<int>(new_edo) << "-edo" << std::endl;
    std::cout << "Reference frequency: " << static_cast<float>(frequency) << " Hz" << std::endl;
    reference_frequency = static_cast<uint64_t>(std::round(frequency * 65536.0));
    note_divisor = pow(2.0f, 1.0f / new_edo);

    tuning = {
        new_edo,
        base_frequency
    };
}

uint16_t Song::get_max_rows() {
    calculate_song_length();
    return max_rows;
}

uint64_t Song::get_song_length() {
    calculate_song_length();
    return song_length;
}

uint8_t Song::get_output_channels() const {
    return output_channels;
}

void Song::set_output_channels(const uint8_t channels) {
    output_channels = std::clamp(static_cast<int>(channels), 1, MAX_OUTPUT_CHANNELS);
}

void Song::export_all(const std::string &directory) const {
    export_header_asm_file(directory);
    export_data_asm_file(directory);
    export_header(directory);
    export_series(directory, "envel", envelopes, {sizeof(Envelope)});
    export_channels(directory);
    export_series(directory, "osc", oscillators, get_struct_sizes(oscillators));
    export_dsps(directory);
    export_arrays(directory, "seq", sequences);
    export_arrays(directory, "order", orders);
    export_arrays(directory, "wave", wavetables);
    export_offsets(directory + "/offsets.bin");
    export_links(directory + "/links.bin");
}

void Song::import_all(const std::string &directory, const nlohmann::json &json) {
    import_envelopes(directory, json);
    import_sequences(directory, json);
    import_orders(directory, json);
    import_wavetables(directory, json);
    import_channels(directory, json);
    import_oscillators(directory, json);
    import_dsps(directory, json);
    import_links(directory, json);
}

Envelope *Song::add_envelope() {
    if (envelopes.size() >= UINT8_MAX) {
        return nullptr;
    }

    Envelope *envelope = new Envelope();
    envelope->base_volume = DEFAULT_ENVELOPE_BASE_VOLUME;
    envelope->sustain_level = DEFAULT_ENVELOPE_SUSTAIN_LEVEL;
    envelope->attack = DEFAULT_ENVELOPE_ATTACK;
    envelope->decay = DEFAULT_ENVELOPE_DECAY;
    envelope->hold = DEFAULT_ENVELOPE_HOLD;
    envelope->release = DEFAULT_ENVELOPE_RELEASE;
    envelopes.push_back(envelope);
    return envelope;
}

Sequence *Song::add_sequence() {
    if (sequences.size() >= MAX_SEQUENCES) {
        return nullptr;
    }

    Sequence *sequence = new Sequence();
    sequence->data_size = 0;
    sequences.push_back(sequence);
    return sequence;
}

Order *Song::add_order() {
    if (orders.size() >= MAX_ORDERS) {
        return nullptr;
    }

    Order *order = new Order();
    order->order_length = 0;
    orders.push_back(order);
    return order;
}

Wavetable *Song::add_wavetable() {
    if (wavetables.size() >= MAX_WAVETABLES) {
        return nullptr;
    }

    Wavetable *wavetable = new Wavetable();
    wavetable->wavetable_size = 0;
    wavetables.push_back(wavetable);
    return wavetable;
}

void *Song::add_oscillator() {
    if (oscillators.size() >= MAX_OSCILLATORS) {
        return nullptr;
    }

    OscillatorSquare *oscillator = new OscillatorSquare();
    oscillator->duty_cycle = DEFAULT_OSCILLATOR_DUTY_CYCLE;
    oscillators.push_back(oscillator);
    return oscillator;
}

Channel *Song::add_channel() {
    if (envelopes.size() >= MAX_CHANNELS) {
        return nullptr;
    }

    Channel *channel = new Channel();
    channel->envelope_index = 0;
    channel->order_index = 0;
    channel->oscillator_index = 0;
    channel->output_flag = 0;
    channel->pitch = DEFAULT_CHANNEL_PITCH;
    channel->output = &output;
    channels.push_back(channel);
    num_channels = channels.size();
    links[static_cast<size_t>(ItemType::CHANNEL)].push_back(Link());
    set_links();
    return channel;
}

void *Song::add_dsp() {
    if (dsps.size() >= MAX_DSPS) {
        return nullptr;
    }

    DSPGainer *dsp = new DSPGainer();
    dsp->volume = DEFAULT_GAINER_VOLUME;

    dsps.push_back(dsp);
    num_dsps = dsps.size();
    links[static_cast<size_t>(ItemType::DSP)].push_back(Link());
    set_links();
    return dsp;
}

void Song::remove_envelope(const size_t index) {
    if (index < envelopes.size()) {
        delete envelopes[index];
        envelopes.erase(envelopes.begin() + index);
        realign_links(index, Target::ENVELOPE);
        for (auto &channel : channels) {
            if (channel->envelope_index >= index) {
                channel->envelope_index = std::max(0, channel->envelope_index - 1);
            }
        }
    }
}

void Song::remove_sequence(const size_t index) {
    if (index < sequences.size()) {
        delete sequences[index];
        sequences.erase(sequences.begin() + index);
        realign_links(index, Target::SEQUENCE);
        for (auto &order : orders) {
            for (size_t i = 0; i < order->order_length; i++) {
                if (order->sequences[i] >= index) {
                    order->sequences[i] = std::max(0, order->sequences[i] - 1);
                }
            }
        }
    }
}

void Song::remove_order(const size_t index) {
    if (index < orders.size()) {
        delete orders[index];
        orders.erase(orders.begin() + index);
        realign_links(index, Target::ORDER);
        for (auto &channel : channels) {
            if (channel->order_index >= index) {
                channel->order_index = std::max(0, channel->order_index - 1);
            }
        }
    }
}

void Song::remove_wavetable(const size_t index) {
    if (index < wavetables.size()) {
        delete wavetables[index];
        wavetables.erase(wavetables.begin() + index);
        realign_links(index, Target::WAVETABLE);
        for (auto &oscillator : oscillators) {
            Oscillator *generic = static_cast<Oscillator *>(oscillator);
            if (generic->generator_index == GENERATOR_WAVETABLE) {
                OscillatorWavetable *wavetable = static_cast<OscillatorWavetable *>(oscillator);
                if (wavetable->wavetable_index >= index) {
                    wavetable->wavetable_index = std::max(0, wavetable->wavetable_index - 1);
                }
            }
        }
    }
}

void Song::remove_oscillator(const size_t index) {
    if (index < oscillators.size()) {
        delete_oscillator(oscillators[index]);
        oscillators.erase(oscillators.begin() + index);
        realign_links(index, Target::OSCILLATOR);
        for (auto &channel : channels) {
            if (channel->oscillator_index >= index) {
                channel->oscillator_index = std::max(0, channel->oscillator_index - 1);
            }
        }
    }
}

void Song::remove_channel(const size_t index) {
    if (index < channels.size()) {
        size_t link_type = static_cast<size_t>(ItemType::CHANNEL);
        delete channels[index];
        channels.erase(channels.begin() + index);
        links[link_type].erase(links[link_type].begin() + index);
        num_channels = channels.size();

        realign_links(index, Target::CHANNEL);
        set_links();
    }
}

void Song::remove_dsp(const size_t index) {
    if (index < dsps.size()) {
        size_t link_type = static_cast<size_t>(ItemType::DSP);
        delete_dsp(dsps[index]);
        dsps.erase(dsps.begin() + index);
        links[link_type].erase(links[link_type].begin() + index);
        num_dsps = dsps.size();

        realign_links(index, Target::DSP_CHANNEL);
        realign_links(index, Target::DSP);
        set_links();
    }
}

std::pair<ValidationResult, int> Song::validate() {
    calculate_song_length();
    for (size_t index = 0; index < channels.size(); index++) {
        const Channel *channel = channels[index];
        if (channel->order_index >= orders.size() && channel->order_index != CONSTANT_PITCH) {
            return {ValidationResult::ChannelMissingOrder, index};
        }
        if (channel->oscillator_index >= oscillators.size()) {
            return {ValidationResult::ChannelMissingOscillator, index};
        }
        if (channel->envelope_index >= envelopes.size()) {
            return {ValidationResult::ChannelMissingEnvelope, index};
        }
    }

    for (size_t index = 0; index < orders.size(); index++) {
        const Order *order = orders[index];
        for (size_t i = 0; i < order->order_length; i++) {
            const uint8_t sequence_index = order->sequences[i];
            if (sequence_index >= sequences.size()) {
                return {ValidationResult::OrderMissingSequence, index};
            }
        }
    }

    for (size_t index = 0; index < oscillators.size(); index++) {
        const void *oscillator = oscillators[index];
        const Oscillator *generic = static_cast<const Oscillator *>(oscillator);
        if (generic->generator_index == GENERATOR_WAVETABLE) {
            const OscillatorWavetable *wavetable = static_cast<const OscillatorWavetable *>(oscillator);
            if (wavetable->wavetable_index >= wavetables.size()) {
                return {ValidationResult::OscillatorMissingWavetable, index};
            }
        }
    }

    if (song_length == 0 || max_rows == 0) {
        return {ValidationResult::InvalidSongLength, -1};
    }

    return {ValidationResult::OK, -1};
}

void Song::generate_header_vector(std::stringstream &asm_content, const std::string &name, const std::string &short_name, const size_t size) const {
    asm_content << name << "s:\n";
    for (size_t i = 0; i < size; i++) {
        asm_content << "global " << name << "s." << name << "_" << i << ":\n";
        asm_content << "." << name << "_" << i << ":\n";
        asm_content << "incbin \"song\\" << get_element_path(short_name + "s", short_name, i, '\\') << "\"\n";
    }
}

std::string Song::generate_header_asm_file() const {
    std::stringstream asm_content;
    asm_content << "    \%define CHANNELS " << channels.size() << "\n";
    asm_content << "    \%define DSPS " << dsps.size() << "\n";
    asm_content << "    \%define WAVETABLES " << wavetables.size() << "\n";
    asm_content << "\n";
    asm_content << "    \%define OUTPUT_CHANNELS " << static_cast<int>(output_channels) << "\n";
    asm_content << "    \%define SONG_LENGTH " << song_length << "\n";
    asm_content << "\n";
    asm_content << "    \%define TUNING_FREQUENCY " << static_cast<uint32_t>(std::round(frequency_table.get_last_frequency() * 65536.0)) << "\n";
    asm_content << "    \%define TUNING_NOTE_DIVISOR " << static_cast<_Float32>(frequency_table.get_note_divisor()) << "\n";
    asm_content << "\n";
    asm_content << "    \%define DSP_BUFFER_SIZE MAX_DSP_BUFFER_SIZE * DSPS" << "\n";
    asm_content << "\n";
    asm_content << "SEGMENT_DATA" << "\n";
    asm_content << "num_channels:" << "\n";
    asm_content << "    db CHANNELS" << "\n";
    asm_content << "num_dsps:" << "\n";
    asm_content << "    db DSPS" << "\n";
    return asm_content.str();
}

std::string Song::generate_data_asm_file() const {
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
    json["general"] = {
        {"bpm", bpm},
        {"normalizer", normalizer},
        {"output_channels", output_channels},
        {"rows", max_rows},
        {"song_length", song_length}
    };

    json["header"] = {
        {"title", header.title},
        {"author", header.author},
        {"version", header.version}
    };

    json["tuning"] = {
        {"edo", tuning.edo},
        {"a4_frequency", tuning.a4_frequency}
    };

    json["data"] = {
        {"envelopes", envelopes.size()},
        {"sequences", sequences.size()},
        {"orders", orders.size()},
        {"wavetables", wavetables.size()},
        {"oscillators", oscillators.size()},
        {"dsps", dsps.size()},
        {"channels", channels.size()}
    };

    return json;
}

nlohmann::json Song::import_header(const std::string &filename) {
    nlohmann::json json = read_json(filename);
    header.title = json["header"]["title"];
    header.author = json["header"]["author"];
    header.version = json["header"]["version"];
    bpm = json["general"]["bpm"];
    normalizer = json["general"]["normalizer"];
    output_channels = json["general"]["output_channels"];
    song_length = json["general"]["song_length"];
    tuning.edo = json["tuning"]["edo"];
    tuning.a4_frequency = json["tuning"]["a4_frequency"];
    return json;
}

void Song::calculate_song_length() {
    max_rows = 0;
    for (size_t channel_index = 0; channel_index < channels.size(); channel_index++) {
        Channel *channel = channels[channel_index];
        const bool constant_pitch = channel->order_index == CONSTANT_PITCH;
        if (constant_pitch || channel->order_index >= orders.size()) {
            continue;
        }

        const Order *order = orders[channel->order_index];
        const OrderSequences &order_sequences = order->sequences;
        const size_t order_length = order->order_length;
        uint16_t rows = 0;
        for (size_t i = 0; i < order_length; i++) {
            const uint8_t sequence_index = order_sequences[i];
            if (sequence_index >= sequences.size()) {
                break;
            }

            const Sequence *sequence = sequences[sequence_index];
            const size_t sequence_length = sequence->data_size / 2;
            for (size_t j = 0; j < sequence_length; j++) {
                const uint8_t duration = sequence->notes[j].duration;
                rows += duration;
            }
        }

        max_rows = std::max(max_rows, rows);
    }

    calculate_ticks_per_beat();
    if (ticks_per_beat == 0) {
        throw std::runtime_error("Ticks per beat is zero. Cannot calculate song length.");
    }

    song_length = max_rows * ticks_per_beat;
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
        if (link.index >= envelopes.size()) {
            link.base = &output;
        } else {
            link.base = envelopes[link.index];
        }
        break;
    case Target::SEQUENCE:
        if (link.index >= sequences.size()) {
            link.base = &output;
        } else {
            link.base = sequences[link.index];
        }
        break;
    case Target::ORDER:
        if (link.index >= orders.size()) {
            link.base = &output;
        } else {
            link.base = orders[link.index];
        }
        break;
    case Target::OSCILLATOR:
        if (link.index >= oscillators.size()) {
            link.base = &output;
        } else {
            link.base = oscillators[link.index];
        }
        break;
    case Target::WAVETABLE:
        if (link.index >= wavetables.size()) {
            link.base = &output;
        } else {
            link.base = wavetables[link.index];
        }
        break;
    case Target::DSP:
        if (link.index >= dsps.size()) {
            link.base = &output;
        } else {
            link.base = dsps[link.index];
        }
        break;
    case Target::CHANNEL:
        if (link.index >= channels.size()) {
            link.base = &output;
        } else {
            link.base = channels[link.index];
        }
        break;
    }

    link.id = i;
    link.item = item;
    link.pointer = link.base + link.offset;
    link.assign_output();
}

void Song::set_links() const {
    size_t link_type = static_cast<size_t>(ItemType::CHANNEL);
    for (size_t i = 0; i < channels.size(); i++) {
        Link &link = links[link_type][i];
        void *item = channels[i];
        try {
            set_link(link, item, i);
        } catch (const std::out_of_range &exception) {
            std::cerr << "Error setting link for a channel " << i << ": " << exception.what() << std::endl;
        }
    }

    link_type = static_cast<size_t>(ItemType::DSP);
    for (size_t i = 0; i < dsps.size(); i++) {
        Link &link = links[link_type][i];
        void *item = dsps[i];
        try {
            set_link(link, item, i);
        } catch (const std::out_of_range &exception) {
            std::cerr << "Error setting link for a DSP " << i << ": " << exception.what() << std::endl;
        }
    }
}

void Song::realign_links(const size_t index, const Target target, const ItemType type) const {
    size_t link_type = static_cast<size_t>(type);
    for (Link &link : links[link_type]) {
        if (link.target == target && link.index >= index) {
            link.index = std::max(0, link.index - 1);
        }
    }
}

void Song::realign_links(const size_t index, const Target target) const {
    realign_links(index, target, ItemType::CHANNEL);
    realign_links(index, target, ItemType::DSP);
}

void Song::set_buffer_offsets() {
    for (size_t i = 0; i < MAX_DSPS; i++) {
        buffer_offsets[i] = i * MAX_DSP_BUFFER_SIZE;
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

void Song::compile_sources(const std::string &directory, const std::string &filename, const bool compress) const {
    std::string compile_command = "python scripts/compile.py \"" + directory + "\" \"" + filename + "\"";
    if (!compress) {
        compile_command += " --uncompressed";
    }

    const int exit_code = run_command(compile_command);
    if (exit_code != 0) {
        throw std::runtime_error("Failed to compile sources: " + compile_command);
    }
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
    write_data(file, &channel->output_flag, sizeof(channel->output_flag));
    write_data(file, &null, sizeof(null));
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
        write_data(file, &delay->output_flag, sizeof(delay->output_flag));
        write_data(file, &null, sizeof(null));
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
        write_data(file, &gainer->output_flag, sizeof(gainer->output_flag));
        write_data(file, &null, sizeof(null));
        write_data(file, &gainer->volume, sizeof(gainer->volume));
    } else if (dsp_type == EFFECT_FILTER) {
        DSPFilter *filter = reinterpret_cast<DSPFilter *>(dsp);
        uint16_t null = 0;
        uint8_t size = 6;
        write_data(file, &size, sizeof(size));
        write_data(file, &filter->effect_index, sizeof(filter->effect_index));
        write_data(file, &filter->output_flag, sizeof(filter->output_flag));
        write_data(file, &null, sizeof(null));
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
    read_data(file, &channel->output_flag, sizeof(channel->output_flag));
    file.seekg(sizeof(uint16_t), std::ios::cur);
    channel->output = &output;
    return channel;
}

void *Song::deserialize_dsp(std::ifstream &file) const {
    uint8_t size, effect_type;
    read_data(file, &size, sizeof(size));
    read_data(file, &effect_type, sizeof(effect_type));

    if (effect_type == EFFECT_DELAY) {
        DSPDelay *delay = new DSPDelay();
        delay->dsp_size = SIZE_DSP_DELAY;
        delay->output = &output;
        read_data(file, &delay->output_flag, sizeof(delay->output_flag));
        file.seekg(sizeof(uint16_t), std::ios::cur);
        read_data(file, &delay->dry, sizeof(delay->dry));
        read_data(file, &delay->wet, sizeof(delay->wet));
        read_data(file, &delay->feedback, sizeof(delay->feedback));
        read_data(file, &delay->delay_time, sizeof(delay->delay_time));
        return reinterpret_cast<void *>(delay);
    } else if (effect_type == EFFECT_GAINER) {
        DSPGainer *gainer = new DSPGainer();
        gainer->dsp_size = SIZE_DSP_GAINER;
        gainer->output = &output;
        read_data(file, &gainer->output_flag, sizeof(gainer->output_flag));
        file.seekg(sizeof(uint16_t), std::ios::cur);
        read_data(file, &gainer->volume, sizeof(gainer->volume));
        return reinterpret_cast<void *>(gainer);
    } else if (effect_type == EFFECT_FILTER) {
        DSPFilter *filter = new DSPFilter();
        filter->dsp_size = SIZE_DSP_FILTER;
        filter->output = &output;
        read_data(file, &filter->output_flag, sizeof(filter->output_flag));
        file.seekg(sizeof(uint16_t), std::ios::cur);
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

    if (oscillator_type == GENERATOR_SQUARE) {
        OscillatorSquare *oscillator = new OscillatorSquare();
        read_data(file, &oscillator->duty_cycle, sizeof(oscillator->duty_cycle));
        return reinterpret_cast<void *>(oscillator);
    } else if (oscillator_type == GENERATOR_SAW) {
        OscillatorSaw *oscillator = new OscillatorSaw();
        return reinterpret_cast<void *>(oscillator);
    } else if (oscillator_type == GENERATOR_SINE) {
        OscillatorSine *oscillator = new OscillatorSine();
        return reinterpret_cast<void *>(oscillator);
    } else if (oscillator_type == GENERATOR_WAVETABLE) {
        OscillatorWavetable *oscillator = new OscillatorWavetable();
        read_data(file, &oscillator->wavetable_index, sizeof(oscillator->wavetable_index));
        return reinterpret_cast<void *>(oscillator);
    } else if (oscillator_type == GENERATOR_NOISE) {
        OscillatorNoise *oscillator = new OscillatorNoise();
        return reinterpret_cast<void *>(oscillator);
    } else {
        throw std::runtime_error("Unknown oscillator type: " + std::to_string(oscillator_type));
    }
}

void Song::export_header_asm_file(const std::string &directory) const {
    std::string asm_content = generate_header_asm_file();
    std::ofstream asm_file(directory + "/header.asm");
    asm_file << asm_content;
    asm_file.close();
}

void Song::export_data_asm_file(const std::string &directory) const {
    std::string asm_content = generate_data_asm_file();
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
    const size_t envelope_count = json["data"]["envelopes"];
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
    const size_t sequence_count = json["data"]["sequences"];
    for (size_t i = 0; i < sequence_count; i++) {
        const std::string filename = get_element_path(song_dir + "/seqs", "seq", i);
        std::ifstream file(filename, std::ios::binary);
        Sequence *sequence = Sequence::deserialize(file);
        sequences.push_back(sequence);
        file.close();
    }
}

void Song::import_orders(const std::string &song_dir, const nlohmann::json &json) {
    const size_t order_count = json["data"]["orders"];
    for (size_t i = 0; i < order_count; i++) {
        const std::string filename = get_element_path(song_dir + "/orders", "order", i);
        std::ifstream file(filename, std::ios::binary);
        Order *order = Order::deserialize(file);
        orders.push_back(order);
        file.close();
    }
}

void Song::import_wavetables(const std::string &song_dir, const nlohmann::json &json) {
    const size_t wavetable_count = json["data"]["wavetables"];
    for (size_t i = 0; i < wavetable_count; i++) {
        const std::string filename = get_element_path(song_dir + "/waves", "wave", i);
        std::ifstream file(filename, std::ios::binary);
        Wavetable *wavetable = Wavetable::deserialize(file);
        wavetables.push_back(wavetable);
        file.close();
    }
}

void Song::import_oscillators(const std::string &song_dir, const nlohmann::json &json) {
    const size_t oscillator_count = json["data"]["oscillators"];
    for (size_t i = 0; i < oscillator_count; i++) {
        const std::string filename = get_element_path(song_dir + "/oscs", "osc", i);
        std::ifstream file(filename, std::ios::binary);
        void *oscillator = deserialize_oscillator(file);
        oscillators.push_back(oscillator);
        file.close();
    }
}

void Song::import_dsps(const std::string &song_dir, const nlohmann::json &json) {
    const size_t dsp_count = json["data"]["dsps"];
    for (size_t i = 0; i < dsp_count; i++) {
        const std::string filename = get_element_path(song_dir + "/dsps", "dsp", i);
        std::ifstream file(filename, std::ios::binary);
        void *dsp = deserialize_dsp(file);
        dsps.push_back(dsp);
        file.close();
    }
}

void Song::import_channels(const std::string &song_dir, const nlohmann::json &json) {
    const size_t channel_count = json["data"]["channels"];
    for (size_t i = 0; i < channel_count; i++) {
        const std::string filename = get_element_path(song_dir + "/chans", "chan", i);
        std::ifstream file(filename, std::ios::binary);
        Channel *channel = deserialize_channel(file);
        channels.push_back(channel);
        file.close();
    }
}

void Song::import_links(const std::string &song_dir, const nlohmann::json &json) {
    const size_t channel_count = json["data"]["channels"];
    const size_t dsp_count = json["data"]["dsps"];
    const std::string links_filename = song_dir + "/links.bin";
    std::ifstream file(links_filename, std::ios::binary);

    size_t link_type = static_cast<size_t>(ItemType::CHANNEL);
    for (size_t i = 0; i < channel_count; i++) {
        Link link;
        link.deserialize(file);
        links[link_type].push_back(link);
    }

    link_type = static_cast<size_t>(ItemType::DSP);
    for (size_t i = 0; i < dsp_count; i++) {
        Link link;
        link.deserialize(file);
        links[link_type].push_back(link);
    }

    set_links();
    file.close();
}

void Song::update_sizes() {
    num_channels = channels.size();
    num_dsps = dsps.size();
}

void Song::clear_data() {
    envelopes.clear();
    sequences.clear();
    orders.clear();
    oscillators.clear();
    wavetables.clear();
    dsps.clear();
    channels.clear();
    links[static_cast<size_t>(ItemType::CHANNEL)].clear();
    links[static_cast<size_t>(ItemType::DSP)].clear();
    num_channels = 0;
    num_dsps = 0;
}

void Song::delete_oscillator(void *oscillator) {
    if (oscillator == nullptr) {
        return;
    }

    const uint8_t *bytes = static_cast<const uint8_t *>(oscillator);
    const uint8_t dsp_type = bytes[1];
    if (dsp_type == GENERATOR_SQUARE) {
        delete static_cast<OscillatorSquare *>(oscillator);
    } else if (dsp_type == GENERATOR_SAW) {
        delete static_cast<OscillatorSaw *>(oscillator);
    } else if (dsp_type == GENERATOR_SINE) {
        delete static_cast<OscillatorSine *>(oscillator);
    } else if (dsp_type == GENERATOR_WAVETABLE) {
        delete static_cast<OscillatorWavetable *>(oscillator);
    } else if (dsp_type == GENERATOR_NOISE) {
        delete static_cast<OscillatorNoise *>(oscillator);
    }
}

void Song::delete_dsp(void *dsp) {
    if (dsp == nullptr) {
        return;
    }

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
