#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <zlib.h>

#include "../general.hpp"
#include "../sizes.hpp"
#include "../utils/file.hpp"
#include "../utils/temp.hpp"
#include "core.hpp"
#include "functions.hpp"
#include "headers.hpp"
#include "song.hpp"

#define JSON_INDENTATION 4

Song::Song() {
    set_buffer_offsets();
    new_song();
}

Song::~Song() {
}

void Song::new_song() {
    clear_data();
    bpm = DEFAULT_BPM;
    sample_rate = DEFAULT_SAMPLE_RATE;
    note_divisor = DEFAULT_NOTE_DIVISOR;
    unit = DEFAULT_UNIT;
    normalizer = DEFAULT_NORMALIZER;
    header = {
        DEFAULT_AUTHOR,
        DEFAULT_TITLE,
        DEFAULT_MESSAGE,
        TRACKER_VERSION
    };
    tuning = {
        DEFAULT_EDO,
        DEFAULT_A4_FREQUENCY
    };

    output_channels = DEFAULT_OUTPUT_CHANNELS;
    song_length = 0;
    max_rows = 0;

    change_tuning(tuning.edo, tuning.a4_frequency);
    link_manager.set_links();
}

void Song::save_to_file(const std::string &filename) {
    const auto [temp_base, song_path] = prepare_temp_directory();
    const std::string directory = song_path.string();

    try {
        calculate_song_length();
        link_manager.set_links();
        export_all(directory, CompilationTarget::Linux);
        compress_directory(directory, filename);
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
    const std::string directory = song_path.string();

    try {
        decompress_archive(filename, directory);
        nlohmann::json json = import_header(directory + "/header.json");
        clear_data();
        import_all(directory, json);
        update_sizes();
        change_tuning(tuning.edo, tuning.a4_frequency);
        link_manager.set_links();

        std::filesystem::remove_all(temp_base);
    } catch (const std::exception &exception) {
        std::filesystem::remove_all(temp_base);
        throw;
    }
}

void Song::compile(const std::string &filename, const CompilationScheme scheme, const CompilationTarget compilation_target) const {
    if (compilation_target != CompilationTarget::Linux) {
        std::cerr << "Unsupported compilation target";
        return;
    }

    const std::string platform = "linux";
    const auto [temp_base, song_path] = prepare_temp_directory();
    const std::string directory = song_path.string();

    try {
        export_all(directory, compilation_target);
        compile_sources(temp_base.string(), filename, scheme, platform);
        std::filesystem::remove_all(temp_base);
    } catch (const std::exception &exception) {
        std::filesystem::remove_all(temp_base);
        throw;
    }
}

void Song::render(const std::string &filename) {
    calculate_song_length();
    file_driver.set_length(song_length);
    file_driver.set_sample_rate(sample_rate);
    file_driver.set_output_channels(output_channels);
    file_driver.set_output_filename(filename);
    file_driver.play();
}

std::string Song::get_title() const {
    return header.title;
}

std::string Song::get_author() const {
    return header.author;
}

std::string Song::get_message() const {
    return header.message;
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

void Song::set_message(const std::string &message) {
    if (message.length() >= MAX_STRING_LENGTH) {
        throw std::runtime_error("Message is too long");
    }

    header.message = message;
}

void Song::change_tuning(const uint8_t new_edo, const double base_frequency) {
    scale_composer.compose(new_edo);
    frequency_table.calculate(base_frequency);
    const double frequency = frequency_table.get_last_frequency();
    std::cout << "New tuning: " << static_cast<int>(new_edo) << "-edo" << std::endl;
    std::cout << "Reference frequency: " << static_cast<float>(frequency) << " Hz" << std::endl;
    reference_frequency = static_cast<uint64_t>(std::round(frequency * 65536.0));
    note_divisor = pow(2.0f, 1.0f / new_edo);
    tuning = {new_edo, base_frequency};
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

void Song::export_all(const std::string &directory, const CompilationTarget compilation_target) const {
    export_header_asm_file(directory);
    export_data_asm_file(directory, compilation_target);
    export_header(directory);
    export_series(directory, "envel", envelopes, {sizeof(Envelope)});
    export_channels(directory);
    export_series(directory, "osc", oscillators, get_struct_sizes(oscillators));
    export_dsps(directory);
    export_arrays(directory, "seq", sequences);
    export_arrays(directory, "order", orders);
    export_arrays(directory, "wave", wavetables);
    export_commands_sequences(directory);
    export_series(directory, "c_chan", commands_channels, {sizeof(CommandsChannel)});
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
    import_commands_sequences(directory, json);
    import_commands_channels(directory, json);
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

Envelope *Song::duplicate_envelope(const size_t index) {
    if (index >= envelopes.size()) {
        return nullptr;
    }

    Envelope *envelope = add_envelope();
    if (envelope == nullptr) {
        return nullptr;
    }

    *envelope = *envelopes[index];
    return envelope;
}

Sequence *Song::add_sequence() {
    if (sequences.size() >= MAX_SEQUENCES) {
        return nullptr;
    }

    Sequence *sequence = new Sequence();
    sequence->size = 0;
    sequences.push_back(sequence);
    return sequence;
}

Sequence *Song::duplicate_sequence(const size_t index) {
    if (index >= sequences.size()) {
        return nullptr;
    }

    Sequence *sequence = add_sequence();
    if (sequence == nullptr) {
        return nullptr;
    }

    *sequence = *sequences[index];
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

Order *Song::duplicate_order(const size_t index) {
    if (index >= orders.size()) {
        return nullptr;
    }

    Order *order = add_order();
    if (order == nullptr) {
        return nullptr;
    }

    *order = *orders[index];
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

Wavetable *Song::duplicate_wavetable(const size_t index) {
    if (index >= wavetables.size()) {
        return nullptr;
    }

    Wavetable *wavetable = add_wavetable();
    if (wavetable == nullptr) {
        return nullptr;
    }

    *wavetable = *wavetables[index];
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

void *Song::duplicate_oscillator(const size_t index) {
    if (index >= oscillators.size()) {
        return nullptr;
    }

    void *oscillator = add_oscillator();
    if (oscillator == nullptr) {
        return nullptr;
    }

    Oscillator *generic = static_cast<Oscillator *>(oscillator);
    *generic = *static_cast<Oscillator *>(oscillators[index]);
    return oscillator;
}

Channel *Song::add_channel() {
    if (envelopes.size() >= MAX_CHANNELS) {
        return nullptr;
    }

    Channel *channel = new Channel();
    channels.push_back(channel);
    num_channels = channels.size();
    links[static_cast<size_t>(ItemType::CHANNEL)].push_back(Link());
    link_manager.set_links();

    channel->envelope_index = 0;
    channel->order_index = 0;
    channel->oscillator_index = 0;
    channel->output_flag = DEFAULT_OUTPUT_FLAG;
    channel->pitch = DEFAULT_CHANNEL_PITCH;
    channel->target = 0;
    channel->splitter[0] = 0x80;
    channel->splitter[1] = 0x80;
    channel->splitter[2] = 0x00;
    channel->splitter[3] = 0x00;
    return channel;
}

Channel *Song::duplicate_channel(const size_t index) {
    if (index >= channels.size()) {
        return nullptr;
    }

    Channel *channel = add_channel();
    if (channel == nullptr) {
        return nullptr;
    }

    *channel = *channels[index];
    const Link &link = links[static_cast<size_t>(ItemType::CHANNEL)][index];
    links[static_cast<size_t>(ItemType::CHANNEL)][channels.size() - 1] = link;
    link_manager.set_links();
    return channel;
}

void *Song::add_dsp() {
    if (dsps.size() >= MAX_DSPS) {
        return nullptr;
    }

    DSPGainer *dsp = new DSPGainer();
    dsps.push_back(dsp);
    num_dsps = dsps.size();
    links[static_cast<size_t>(ItemType::DSP)].push_back(Link());
    link_manager.set_links();

    dsp->dsp_size = SIZE_DSP_GAINER;
    dsp->output_flag = DEFAULT_OUTPUT_FLAG;
    dsp->splitter[0] = 0x80;
    dsp->splitter[1] = 0x80;
    dsp->splitter[2] = 0x00;
    dsp->splitter[3] = 0x00;
    dsp->effect_index = EFFECT_GAINER;
    dsp->volume = DEFAULT_GAINER_VOLUME;

    return dsp;
}

void *Song::duplicate_dsp(const size_t index) {
    if (index >= dsps.size()) {
        return nullptr;
    }

    void *dsp = add_dsp();
    if (dsp == nullptr) {
        return nullptr;
    }

    DSPGainer *generic = static_cast<DSPGainer *>(dsp);
    *generic = *static_cast<DSPGainer *>(dsps[index]);

    Link link = links[static_cast<size_t>(ItemType::DSP)][index];
    if (link.type == ItemType::DSP) {
        if (link.target == Target::DIRECT_DSP || link.target == Target::SPLITTER_DSP) {
            link.target = Target::DIRECT_OUTPUT;
        }
    }

    links[static_cast<size_t>(ItemType::DSP)][dsps.size() - 1] = link;
    link_manager.set_links();
    return dsp;
}

CommandsSequence *Song::add_commands_sequence() {
    if (commands_sequences.size() >= MAX_COMMANDS_SEQUENCES) {
        return nullptr;
    }

    CommandsSequence *sequence = new CommandsSequence();
    sequence->size = 0;
    sequence->length = 1;
    commands_sequences.push_back(sequence);
    return sequence;
}

CommandsSequence *Song::duplicate_commands_sequence(const size_t index) {
    if (index >= commands_sequences.size()) {
        return nullptr;
    }

    CommandsSequence *sequence = add_commands_sequence();
    if (sequence == nullptr) {
        return nullptr;
    }

    *sequence = *commands_sequences[index];
    return sequence;
}

CommandsChannel *Song::add_commands_channel() {
    if (commands_channels.size() >= MAX_COMMANDS_CHANNELS) {
        return nullptr;
    }

    CommandsChannel *channel = new CommandsChannel();
    commands_channels.push_back(channel);
    num_commands_channels = commands_channels.size();
    return channel;
}

CommandsChannel *Song::duplicate_commands_channel(const size_t index) {
    if (index >= commands_channels.size()) {
        return nullptr;
    }

    CommandsChannel *channel = add_commands_channel();
    if (channel == nullptr) {
        return nullptr;
    }

    *channel = *commands_channels[index];
    return channel;
}

void Song::remove_envelope(const size_t index) {
    if (index < envelopes.size()) {
        delete envelopes[index];
        envelopes.erase(envelopes.begin() + index);
        link_manager.realign_links(index, Target::ENVELOPE);
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
        link_manager.realign_links(index, Target::SEQUENCE);
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
        link_manager.realign_links(index, Target::ORDER);
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
        link_manager.realign_links(index, Target::WAVETABLE);
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
        link_manager.realign_links(index, Target::OSCILLATOR);
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

        link_manager.realign_links(index, Target::CHANNEL);
        link_manager.set_links();
    }
}

void Song::remove_dsp(const size_t index) {
    if (index < dsps.size()) {
        size_t link_type = static_cast<size_t>(ItemType::DSP);
        delete_dsp(dsps[index]);
        dsps.erase(dsps.begin() + index);
        links[link_type].erase(links[link_type].begin() + index);
        num_dsps = dsps.size();

        link_manager.realign_links(index, Target::DIRECT_DSP);
        link_manager.realign_links(index, Target::DSP);
        link_manager.set_links();
    }
}

void Song::remove_commands_sequence(const size_t index) {
    if (index < commands_sequences.size()) {
        delete commands_sequences[index];
        commands_sequences.erase(commands_sequences.begin() + index);
        link_manager.realign_links(index, Target::COMMANDS_SEQUENCE);
        for (auto &order : orders) {
            for (size_t i = 0; i < order->order_length; i++) {
                if (order->sequences[i] >= index) {
                    order->sequences[i] = std::max(0, order->sequences[i] - 1);
                }
            }
        }
    }
}

void Song::remove_commands_channel(const size_t index) {
    if (index < commands_channels.size()) {
        delete commands_channels[index];
        commands_channels.erase(commands_channels.begin() + index);
        num_commands_channels = commands_channels.size();

        link_manager.realign_links(index, Target::COMMANDS_CHANNEL);
        link_manager.set_links();
    }
}

std::pair<ValidationResult, int> Song::validate() {
    calculate_song_length();
    for (size_t index = 0; index < channels.size(); index++) {
        const Channel *channel = channels[index];
        if (channel->order_index >= orders.size()) {
            return {ValidationResult::ChannelMissingOrder, index};
        }
        if (channel->oscillator_index >= oscillators.size()) {
            return {ValidationResult::ChannelMissingOscillator, index};
        }
        if (channel->envelope_index >= envelopes.size()) {
            return {ValidationResult::ChannelMissingEnvelope, index};
        }
    }

    for (size_t index = 0; index < commands_channels.size(); index++) {
        const CommandsChannel *channel = commands_channels[index];
        if (channel->order_index >= orders.size()) {
            return {ValidationResult::ChannelMissingOrder, index};
        }
    }

    std::set<size_t> channel_orders = get_channel_orders();
    for (const size_t order_index : channel_orders) {
        const Order *order = orders[order_index];
        for (size_t i = 0; i < order->order_length; i++) {
            if (order->sequences[i] >= sequences.size()) {
                return {ValidationResult::OrderMissingSequence, order_index};
            }
        }
    }

    std::set<size_t> commands_channel_orders = get_commands_channel_orders();
    for (const size_t order_index : commands_channel_orders) {
        const Order *order = orders[order_index];
        for (size_t i = 0; i < order->order_length; i++) {
            if (order->sequences[i] >= commands_sequences.size()) {
                return {ValidationResult::OrderMissingSequence, order_index};
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

std::vector<size_t> Song::find_envelope_dependencies(const size_t envelope_index) const {
    std::set<size_t> dependencies;
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->envelope_index == envelope_index) {
            dependencies.insert(i);
        }
    }

    return std::vector<size_t>(dependencies.begin(), dependencies.end());
}

std::vector<size_t> Song::find_sequence_dependencies(const size_t sequence_index) const {
    std::set<size_t> dependencies;
    std::set<size_t> channel_orders = get_channel_orders();
    for (const size_t order_index : channel_orders) {
        const Order *order = orders[order_index];
        for (size_t i = 0; i < order->order_length; i++) {
            if (order->sequences[i] == sequence_index) {
                dependencies.insert(order_index);
            }
        }
    }

    return std::vector<size_t>(dependencies.begin(), dependencies.end());
}

std::vector<size_t> Song::find_order_dependencies(const size_t order_index) const {
    std::set<size_t> dependencies;
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->order_index == order_index) {
            dependencies.insert(i);
        }
    }

    return std::vector<size_t>(dependencies.begin(), dependencies.end());
}

std::vector<size_t> Song::find_wavetable_dependencies(const size_t wavetable_index) const {
    std::set<size_t> dependencies;
    for (size_t i = 0; i < oscillators.size(); i++) {
        const Oscillator *oscillator = static_cast<const Oscillator *>(oscillators[i]);
        if (oscillator->generator_index == GENERATOR_WAVETABLE) {
            const OscillatorWavetable *wavetable = static_cast<const OscillatorWavetable *>(oscillators[i]);
            if (wavetable->wavetable_index == wavetable_index) {
                dependencies.insert(i);
            }
        }
    }

    return std::vector<size_t>(dependencies.begin(), dependencies.end());
}

std::vector<size_t> Song::find_oscillator_dependencies(const size_t oscillator_index) const {
    std::set<size_t> dependencies;
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->oscillator_index == oscillator_index) {
            dependencies.insert(i);
        }
    }

    return std::vector<size_t>(dependencies.begin(), dependencies.end());
}

std::vector<size_t> Song::find_commands_sequence_dependencies(const size_t sequence_index) const {
    std::set<size_t> dependencies;
    std::set<size_t> channel_orders = get_commands_channel_orders();
    for (const size_t order_index : channel_orders) {
        const Order *order = orders[order_index];
        for (size_t i = 0; i < order->order_length; i++) {
            if (order->sequences[i] == sequence_index) {
                dependencies.insert(order_index);
            }
        }
    }

    return std::vector<size_t>(dependencies.begin(), dependencies.end());
}

float Song::calculate_real_bpm() const {
    return unit * static_cast<float>(sample_rate) / static_cast<float>(ticks_per_beat);
}

void Song::generate_header_vector(
    std::stringstream &asm_content,
    const std::string &name,
    const std::string &short_name,
    const size_t size,
    const char separator
) const {
    asm_content << "\n\n"
                << name << "s:\n";
    for (size_t i = 0; i < size; i++) {
        asm_content << "\nglobal " << name << "s." << name << "_" << i << ":\n";
        asm_content << "." << name << "_" << i << ":\n";
        asm_content << "incbin \"song" << separator << get_element_path(short_name + "s", short_name, i, separator) << "\"\n";
    }
}

void Song::set_used_flags(std::stringstream &asm_content) const {
    if (!dsps.empty()) {
        asm_content << "    \%define USED_DSP\n";
        for (size_t i = 0; i < static_cast<size_t>(Effect::Count); i++) {
            const Effect effect = static_cast<Effect>(i);
            const std::string &effect_name = header_effect_names[i];
            if (calculate_dsps(effect) > 0) {
                asm_content << "    \%define USED_" << effect_name << "\n";
            }
        }
    }

    for (size_t i = 0; i < static_cast<size_t>(Generator::Count); i++) {
        const Generator generator = static_cast<Generator>(i);
        const std::string &generator_name = header_generator_names[i];
        if (calculate_oscillators(generator) > 0) {
            asm_content << "    \%define USED_" << generator_name << "\n";
        }
    }

    if (!commands_channels.empty()) {
        asm_content << "    \%define USED_COMMANDS\n";
        for (size_t i = 0; i < static_cast<size_t>(Instruction::Count); i++) {
            const Instruction instruction = static_cast<Instruction>(i);
            const std::string &command_name = header_instruction_names[i];
            if (calculate_commands(instruction) > 0) {
                asm_content << "    \%define USED_" << command_name << "\n";
            }
        }
    }

    asm_content << "\n";
}

std::string Song::generate_header_asm_file() const {
    std::stringstream asm_content;
    set_used_flags(asm_content);
    asm_content << "    \%define CHANNELS " << channels.size() << "\n";
    asm_content << "    \%define DSPS " << dsps.size() << "\n";
    asm_content << "    \%define COMMANDS_CHANNELS " << commands_channels.size() << "\n";
    asm_content << "    \%define WAVETABLES " << wavetables.size() << "\n";
    asm_content << "\n";
    asm_content << "    \%define OUTPUT_CHANNELS " << static_cast<int>(output_channels) << "\n";
    asm_content << "    \%define SONG_LENGTH " << song_length << "\n";
    asm_content << "    \%define SAMPLE_RATE " << sample_rate << "\n";
    asm_content << "    \%define MESSAGE " << "\"" << header.message << "\"\n";
    asm_content << "\n";
    asm_content << "    \%define TUNING_FREQUENCY " << static_cast<uint64_t>(std::round(frequency_table.get_last_frequency() * 65536.0)) << "\n";
    asm_content << "    \%define TUNING_NOTE_DIVISOR " << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << static_cast<_Float64>(frequency_table.get_note_divisor()) << "\n";
    asm_content << "\n";
    asm_content << "\n";
    asm_content << "    extern num_channels" << "\n";
    asm_content << "    extern num_dsps" << "\n";
    asm_content << "    extern num_commands_channels" << "\n";
    asm_content << "    extern unit" << "\n";
    asm_content << "\n";
    return asm_content.str();
}

void Song::generate_targets_asm(
    std::stringstream &asm_content,
    const CompilationTarget compilation_target,
    const char separator
) const {
    asm_content << "\n\n"
                << "align 4\n"
                << "targets:\n";
    const auto pointers = link_manager.get_pointers_map();
    for (const auto &pair : pointers) {
        const LinkKey key = pair.second;
        asm_content << "    dd ";
        asm_content << link_manager.get_link_reference(key) << "\n";
    }
}

void Song::generate_offsets_asm(std::stringstream &asm_content, const char separator) const {
    asm_content << "\n\nbuffer_offsets:\n";
    asm_content << "incbin \"song" << separator << "offsets.bin\"\n";
}

std::string Song::generate_data_asm_file(const CompilationTarget compilation_target, const char separator) const {
    std::stringstream asm_content;
    asm_content << "SEGMENT_DATA\n";
    asm_content << "bpm:\n";
    asm_content << "dw " << bpm << "\n";
    asm_content << "unit:\n";
    asm_content << "dd "
                << std::fixed
                << std::setprecision(std::numeric_limits<float>::max_digits10)
                << static_cast<float>(unit) << "\n";
    asm_content << "normalizer:\n";
    asm_content << "dd "
                << std::fixed
                << std::setprecision(std::numeric_limits<float>::max_digits10)
                << static_cast<float>(normalizer) << "\n\n";
    asm_content << "num_channels:\n";
    asm_content << "db " << static_cast<int>(num_channels) << "\n";
    asm_content << "num_dsps:\n";
    asm_content << "db " << static_cast<int>(num_dsps) << "\n";
    asm_content << "num_commands_channels:\n";
    asm_content << "db " << static_cast<int>(num_commands_channels) << "\n\n";

    generate_header_vector(asm_content, "envelope", "envel", envelopes.size(), separator);
    generate_header_vector(asm_content, "sequence", "seq", sequences.size(), separator);
    generate_header_vector(asm_content, "order", "order", orders.size(), separator);
    generate_header_vector(asm_content, "oscillator", "osc", oscillators.size(), separator);
    generate_header_vector(asm_content, "wavetable", "wave", wavetables.size(), separator);
    generate_header_vector(asm_content, "channel", "chan", channels.size(), separator);
    generate_header_vector(asm_content, "dsp", "dsp", dsps.size(), separator);
    generate_header_vector(asm_content, "commands_sequence", "c_seq", commands_sequences.size(), separator);
    generate_header_vector(asm_content, "commands_channel", "c_chan", commands_channels.size(), separator);
    generate_targets_asm(asm_content, compilation_target, separator);
    generate_offsets_asm(asm_content, separator);

    return asm_content.str();
}

nlohmann::json Song::create_header_json() const {
    nlohmann::json json;
    json["general"] = {
        {"bpm", bpm},
        {"unit", unit},
        {"sample_rate", sample_rate},
        {"normalizer", normalizer},
        {"output_channels", output_channels},
        {"rows", max_rows},
        {"song_length", song_length}
    };

    json["header"] = {
        {"title", header.title},
        {"author", header.author},
        {"message", header.message},
        {"version", header.version},
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
        {"channels", channels.size()},
        {"commands", links[static_cast<size_t>(ItemType::COMMANDS)].size()},
        {"commands_sequences", commands_sequences.size()},
        {"commands_channels", commands_channels.size()},
    };

    return json;
}

nlohmann::json Song::import_header(const std::string &filename) {
    nlohmann::json json = read_json(filename);
    const auto &json_header = json["header"];
    header.title = json_header.value("title", DEFAULT_TITLE);
    header.author = json_header.value("author", DEFAULT_AUTHOR);
    header.message = json_header.value("message", DEFAULT_MESSAGE);
    header.version = json_header["version"];

    const auto &json_general = json["general"];
    bpm = json_general["bpm"];
    unit = json_general.value("unit", DEFAULT_UNIT);
    sample_rate = json_general.value("sample_rate", DEFAULT_SAMPLE_RATE);
    normalizer = json_general.value("normalizer", DEFAULT_NORMALIZER);
    output_channels = json_general.value("output_channels", DEFAULT_OUTPUT_CHANNELS);
    song_length = json_general.value("song_length", 0);
    max_rows = json_general.value("rows", 0);

    const auto &json_tuning = json["tuning"];
    tuning.edo = json_tuning.value("edo", DEFAULT_EDO);
    tuning.a4_frequency = json_tuning.value("a4_frequency", DEFAULT_A4_FREQUENCY);
    return json;
}

void Song::calculate_song_length() {
    max_rows = 0;
    for (size_t channel_index = 0; channel_index < channels.size(); channel_index++) {
        Channel *channel = channels[channel_index];
        if (channel->order_index >= orders.size()) {
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
            const size_t sequence_length = sequence->size / 2;
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

size_t Song::calculate_dsps(const Effect effect) const {
    size_t count = 0;
    const uint8_t effect_value = static_cast<uint8_t>(effect);
    for (const auto &dsp : dsps) {
        if (static_cast<DSP *>(dsp)->effect_index == effect_value) {
            count++;
        }
    }
    return count;
}

size_t Song::calculate_oscillators(const Generator generator) const {
    size_t count = 0;
    const uint8_t generator_value = static_cast<uint8_t>(generator);
    for (const auto &oscillator : oscillators) {
        if (static_cast<Oscillator *>(oscillator)->generator_index == generator_value) {
            count++;
        }
    }
    return count;
}

size_t Song::calculate_commands(const Instruction instruction) const {
    size_t count = 0;
    const uint8_t instruction_value = static_cast<uint8_t>(instruction);
    for (const auto &commands_sequence : commands_sequences) {
        const size_t sequence_length = commands_sequence->length;
        for (size_t i = 0; i < sequence_length; ++i) {
            const Command &command = commands_sequence->commands[i];
            if (command.instruction == instruction_value) {
                count++;
            }
        }
    }
    return count;
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

void Song::compile_sources(const std::string &directory, const std::string &filename, const CompilationScheme scheme, const std::string platform) const {
    std::string compile_command = "python scripts/compile.py " + platform + " \"" + directory + "\" \"" + filename + "\"";
    switch (scheme) {
    case CompilationScheme::Uncompressed: {
        compile_command += " --uncompressed";
        break;
    }
    case CompilationScheme::Debug: {
        compile_command += " --debug";
        break;
    }
    case CompilationScheme::Compressed:
    default:
        break;
    }

    const int exit_code = run_command(compile_command);
    if (exit_code != 0) {
        throw std::runtime_error("Failed to compile sources: " + compile_command);
    }
}

std::string Song::get_element_path(const std::string &directory, const std::string prefix, const size_t i, const char separator) const {
    return directory + separator + prefix + "_" + std::to_string(i) + ".bin";
}

void Song::serialize_dsp(std::ofstream &file, void *dsp) const {
    const DSP *generic = static_cast<DSP *>(dsp);
    write_data(file, &generic->dsp_size, sizeof(generic->dsp_size));
    write_data(file, &generic->effect_index, sizeof(generic->effect_index));
    write_data(file, &generic->output_flag, sizeof(generic->output_flag));
    write_data(file, &generic->flag, sizeof(generic->flag));
    write_data(file, &generic->splitter, sizeof(generic->splitter));
    write_data(file, &generic->target, sizeof(generic->target));

    const uint8_t effect_index = static_cast<DSP *>(dsp)->effect_index;
    switch (static_cast<Effect>(effect_index)) {
    case Effect::Gainer: {
        DSPGainer *gainer = reinterpret_cast<DSPGainer *>(dsp);
        write_data(file, &gainer->volume, sizeof(gainer->volume));
        write_data(file, &gainer->pad, sizeof(gainer->pad));
        return;
    }
    case Effect::Distortion: {
        DSPDistortion *distortion = reinterpret_cast<DSPDistortion *>(dsp);
        write_data(file, &distortion->pad, sizeof(distortion->pad));
        write_data(file, &distortion->level, sizeof(distortion->level));
        return;
    }
    case Effect::Filter: {
        DSPFilter *filter = reinterpret_cast<DSPFilter *>(dsp);
        write_data(file, &filter->frequency, sizeof(filter->frequency));
        write_data(file, &filter->mode, sizeof(filter->mode));
        write_data(file, &filter->pad, sizeof(filter->pad));
        return;
    }
    case Effect::Delay: {
        DSPDelay *delay = reinterpret_cast<DSPDelay *>(dsp);
        write_data(file, &delay->dry, sizeof(delay->dry));
        write_data(file, &delay->wet, sizeof(delay->wet));
        write_data(file, &delay->feedback, sizeof(delay->feedback));
        write_data(file, &delay->delay_time, sizeof(delay->delay_time));
        write_data(file, &delay->pad, sizeof(delay->pad));
        return;
    }
    case Effect::Count:
    default: {
        throw std::runtime_error("Unknown DSP type: " + std::to_string(effect_index));
    }
    }
}

void *Song::deserialize_dsp(std::ifstream &file) const {
    DSP *generic = new DSP();
    read_data(file, &generic->dsp_size, sizeof(generic->dsp_size));
    read_data(file, &generic->effect_index, sizeof(generic->effect_index));
    read_data(file, &generic->output_flag, sizeof(generic->output_flag));
    read_data(file, &generic->flag, sizeof(generic->flag));
    read_data(file, &generic->splitter, sizeof(generic->splitter));
    read_data(file, &generic->target, sizeof(generic->target));

    switch (static_cast<Effect>(generic->effect_index)) {
    case Effect::Gainer: {
        DSPGainer *gainer = reinterpret_cast<DSPGainer *>(generic);
        read_data(file, &gainer->volume, sizeof(gainer->volume));
        return reinterpret_cast<void *>(gainer);
    }
    case Effect::Distortion: {
        DSPDistortion *distortion = reinterpret_cast<DSPDistortion *>(generic);
        read_data(file, &distortion->level, sizeof(distortion->level));
        return reinterpret_cast<void *>(distortion);
    }
    case Effect::Filter: {
        DSPFilter *filter = reinterpret_cast<DSPFilter *>(generic);
        read_data(file, &filter->frequency, sizeof(filter->frequency));
        read_data(file, &filter->mode, sizeof(filter->mode));
        return reinterpret_cast<void *>(filter);
    }
    case Effect::Delay: {
        DSPDelay *delay = reinterpret_cast<DSPDelay *>(generic);
        read_data(file, &delay->dry, sizeof(delay->dry));
        read_data(file, &delay->wet, sizeof(delay->wet));
        read_data(file, &delay->feedback, sizeof(delay->feedback));
        read_data(file, &delay->delay_time, sizeof(delay->delay_time));
        return reinterpret_cast<void *>(delay);
    }
    case Effect::Count:
    default: {
        throw std::runtime_error("Unknown DSP type: " + std::to_string(generic->effect_index));
    }
    }

    return nullptr;
}

void *Song::deserialize_oscillator(std::ifstream &file) const {
    uint8_t size, oscillator_type;
    read_data(file, &size, sizeof(size));
    read_data(file, &oscillator_type, sizeof(oscillator_type));

    switch (static_cast<Generator>(oscillator_type)) {
    case Generator::Square: {
        OscillatorSquare *oscillator = new OscillatorSquare();
        read_data(file, &oscillator->duty_cycle, sizeof(oscillator->duty_cycle));
        return reinterpret_cast<void *>(oscillator);
    }
    case Generator::Saw: {
        OscillatorSaw *oscillator = new OscillatorSaw();
        read_data(file, &oscillator->reverse, sizeof(oscillator->reverse));
        return reinterpret_cast<void *>(oscillator);
    }
    case Generator::Sine: {
        OscillatorSine *oscillator = new OscillatorSine();
        return reinterpret_cast<void *>(oscillator);
    }
    case Generator::Wavetable: {
        OscillatorWavetable *oscillator = new OscillatorWavetable();
        read_data(file, &oscillator->wavetable_index, sizeof(oscillator->wavetable_index));
        read_data(file, &oscillator->interpolation, sizeof(oscillator->interpolation));
        return reinterpret_cast<void *>(oscillator);
    }
    case Generator::Noise: {
        OscillatorNoise *oscillator = new OscillatorNoise();
        return reinterpret_cast<void *>(oscillator);
    }
    case Generator::Count:
    default: {
        throw std::runtime_error("Unknown oscillator type: " + std::to_string(oscillator_type));
    }
    }
}

void Song::export_header_asm_file(const std::string &directory) const {
    std::string asm_content = generate_header_asm_file();
    std::ofstream asm_file(directory + "/header.asm");
    asm_file << asm_content;
    asm_file.close();
}

void Song::export_data_asm_file(const std::string &directory, const CompilationTarget compilation_target) const {
    std::string asm_content = generate_data_asm_file(compilation_target);
    std::ofstream asm_file(directory + "/data.asm");
    asm_file << asm_content;
    asm_file.close();
}

void Song::export_header(const std::string &directory) const {
    const nlohmann::json header = create_header_json();
    std::ofstream header_file(directory + "/header.json");
    header_file << header.dump(JSON_INDENTATION);
    header_file.close();
}

template <typename T>
void Song::export_series(const std::string &directory, const std::string &prefix, const std::vector<T> &series, const std::vector<size_t> &sizes) const {
    const std::filesystem::path series_dir = directory + "/" + prefix + "s";
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
        const Channel *channel = channels[i];
        const std::string filename = get_element_path(series_dir, "chan", i);
        std::ofstream file(filename, std::ios::binary);
        channel->serialize(file);
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

void Song::export_commands_sequences(const std::string &directory) const {
    const std::filesystem::path series_dir = directory + "/c_seqs";
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < commands_sequences.size(); i++) {
        const CommandsSequence *sequence = commands_sequences[i];
        const std::string filename = get_element_path(series_dir, "c_seq", i);
        std::ofstream file(filename, std::ios::binary);
        sequence->serialize(file);
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
        const uint32_t offset = buffer_offsets[i];
        write_data(file, &offset, sizeof(offset));
    }
    file.close();
}

void Song::export_links(const std::string &filename) const {
    std::ofstream file(filename, std::ios::binary);
    for (const ItemType &type : {ItemType::CHANNEL, ItemType::DSP, ItemType::COMMANDS}) {
        const size_t link_type = static_cast<size_t>(type);
        for (const auto &link : links[link_type]) {
            link.serialize(file);
        }
    }
    file.close();
}

void Song::import_envelopes(const std::string &directory, const nlohmann::json &json) {
    const size_t envelope_count = json["data"]["envelopes"];
    for (size_t i = 0; i < envelope_count; i++) {
        const std::string filename = get_element_path(directory + "/envels", "envel", i);
        Envelope *envelope = new Envelope();
        std::ifstream file(filename, std::ios::binary);
        read_data(file, envelope, sizeof(Envelope));
        envelopes.push_back(envelope);
        file.close();
    }
}

void Song::import_sequences(const std::string &directory, const nlohmann::json &json) {
    const size_t sequence_count = json["data"]["sequences"];
    for (size_t i = 0; i < sequence_count; i++) {
        const std::string filename = get_element_path(directory + "/seqs", "seq", i);
        std::ifstream file(filename, std::ios::binary);
        Sequence *sequence = Sequence::deserialize(file);
        sequences.push_back(sequence);
        file.close();
    }
}

void Song::import_orders(const std::string &directory, const nlohmann::json &json) {
    const size_t order_count = json["data"]["orders"];
    for (size_t i = 0; i < order_count; i++) {
        const std::string filename = get_element_path(directory + "/orders", "order", i);
        std::ifstream file(filename, std::ios::binary);
        Order *order = Order::deserialize(file);
        orders.push_back(order);
        file.close();
    }
}

void Song::import_wavetables(const std::string &directory, const nlohmann::json &json) {
    const size_t wavetable_count = json["data"]["wavetables"];
    for (size_t i = 0; i < wavetable_count; i++) {
        const std::string filename = get_element_path(directory + "/waves", "wave", i);
        std::ifstream file(filename, std::ios::binary);
        Wavetable *wavetable = Wavetable::deserialize(file);
        wavetables.push_back(wavetable);
        file.close();
    }
}

void Song::import_oscillators(const std::string &directory, const nlohmann::json &json) {
    const size_t oscillator_count = json["data"]["oscillators"];
    for (size_t i = 0; i < oscillator_count; i++) {
        const std::string filename = get_element_path(directory + "/oscs", "osc", i);
        std::ifstream file(filename, std::ios::binary);
        void *oscillator = deserialize_oscillator(file);
        oscillators.push_back(oscillator);
        file.close();
    }
}

void Song::import_dsps(const std::string &directory, const nlohmann::json &json) {
    const size_t dsp_count = json["data"]["dsps"];
    for (size_t i = 0; i < dsp_count; i++) {
        const std::string filename = get_element_path(directory + "/dsps", "dsp", i);
        std::ifstream file(filename, std::ios::binary);
        void *dsp = deserialize_dsp(file);
        dsps.push_back(dsp);
        file.close();
    }
}

void Song::import_channels(const std::string &directory, const nlohmann::json &json) {
    const size_t channel_count = json["data"]["channels"];
    for (size_t i = 0; i < channel_count; i++) {
        const std::string filename = get_element_path(directory + "/chans", "chan", i);
        std::ifstream file(filename, std::ios::binary);
        Channel *channel = Channel::deserialize(file);
        channels.push_back(channel);
        file.close();
    }
}

void Song::import_commands_sequences(const std::string &directory, const nlohmann::json &json) {
    const size_t sequence_count = json["data"]["commands_sequences"];
    for (size_t i = 0; i < sequence_count; i++) {
        const std::string filename = get_element_path(directory + "/c_seqs", "c_seq", i);
        std::ifstream file(filename, std::ios::binary);
        CommandsSequence *sequence = CommandsSequence::deserialize(file);
        commands_sequences.push_back(sequence);
        file.close();
    }
}

void Song::import_commands_channels(const std::string &directory, const nlohmann::json &json) {
    const size_t channel_count = json["data"]["commands_channels"];
    for (size_t i = 0; i < channel_count; i++) {
        const std::string filename = get_element_path(directory + "/c_chans", "c_chan", i);
        std::ifstream file(filename, std::ios::binary);
        CommandsChannel *channel = new CommandsChannel();
        read_data(file, channel, sizeof(CommandsChannel));
        commands_channels.push_back(channel);
        file.close();
    }
}

void Song::import_links(const std::string &directory, const nlohmann::json &json) {
    const size_t channel_count = json["data"]["channels"];
    const size_t dsp_count = json["data"]["dsps"];
    const size_t commands_count = json["data"]["commands"];
    const std::string links_filename = directory + "/links.bin";
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

    link_type = static_cast<size_t>(ItemType::COMMANDS);
    for (size_t i = 0; i < commands_count; i++) {
        Link link;
        link.deserialize(file);
        links[link_type].push_back(link);
        const size_t sequence_index = link.id >> 8;
        const size_t channel_index = link.id & 0xFF;
        commands_links[sequence_index][channel_index] = link;
    }

    link_manager.set_links();
    file.close();
}

void Song::update_sizes() {
    num_channels = channels.size();
    num_dsps = dsps.size();
    num_commands_channels = commands_channels.size();
}

void Song::clear_data() {
    envelopes.clear();
    sequences.clear();
    orders.clear();
    oscillators.clear();
    wavetables.clear();
    dsps.clear();
    channels.clear();
    commands_channels.clear();
    commands_sequences.clear();
    link_manager.reset();
    update_sizes();
    buffers.clear();
}

void Song::delete_oscillator(void *oscillator) {
    if (oscillator == nullptr) {
        return;
    }

    const Oscillator *generic = reinterpret_cast<const Oscillator *>(oscillator);
    const uint8_t generator = generic->generator_index;
    switch (static_cast<Generator>(generator)) {
    case Generator::Square: {
        delete static_cast<OscillatorSquare *>(oscillator);
        break;
    }
    case Generator::Saw: {
        delete static_cast<OscillatorSaw *>(oscillator);
        break;
    }
    case Generator::Sine: {
        delete static_cast<OscillatorSine *>(oscillator);
        break;
    }
    case Generator::Wavetable: {
        delete static_cast<OscillatorWavetable *>(oscillator);
        break;
    }
    case Generator::Noise: {
        delete static_cast<OscillatorNoise *>(oscillator);
        break;
    }
    case Generator::Count:
    default:
        throw std::runtime_error("Unknown oscillator type: " + std::to_string(generator));
    }
}

void Song::delete_dsp(void *dsp) {
    if (dsp == nullptr) {
        return;
    }

    const DSP *generic = reinterpret_cast<const DSP *>(dsp);
    const uint8_t effect = generic->effect_index;
    switch (static_cast<Effect>(effect)) {
    case Effect::Gainer: {
        delete static_cast<DSPGainer *>(dsp);
        return;
    }
    case Effect::Distortion: {
        delete static_cast<DSPDistortion *>(dsp);
        return;
    }
    case Effect::Filter: {
        delete static_cast<DSPFilter *>(dsp);
        return;
    }
    case Effect::Delay: {
        delete static_cast<DSPFilter *>(dsp);
        return;
    }
    case Effect::Count:
    default:
        throw std::runtime_error("Unknown DSP type: " + std::to_string(effect));
    }
}

std::set<size_t> Song::get_channel_orders() const {
    std::set<size_t> channel_orders;
    for (size_t i = 0; i < channels.size(); i++) {
        const Channel *channel = channels[i];
        if (channel->order_index >= orders.size()) {
            continue;
        }

        channel_orders.insert(channel->order_index);
    }

    return channel_orders;
}

std::set<size_t> Song::get_commands_channel_orders() const {
    std::set<size_t> commands_channel_orders;
    for (size_t i = 0; i < commands_channels.size(); i++) {
        const CommandsChannel *channel = commands_channels[i];
        if (channel->order_index >= orders.size()) {
            continue;
        }

        commands_channel_orders.insert(channel->order_index);
    }

    return commands_channel_orders;
}
