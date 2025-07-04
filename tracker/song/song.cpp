#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

#include "zip_file.hpp"

#include "../driver/file.hpp"
#include "../maps/commands.hpp"
#include "../general.hpp"
#include "../gui/gui.hpp"
#include "../gui/names.hpp"
#include "../structures/sizes.hpp"
#include "../structures/commands/command.hpp"
#include "../structures/resources/manager.hpp"
#include "../utils/file.hpp"
#include "../utils/math.hpp"
#include "../utils/paths.hpp"
#include "../utils/system.hpp"
#include "buffers.hpp"
#include "core.hpp"
#include "functions.hpp"
#include "headers.hpp"
#include "song.hpp"
#include "links/manager.hpp"
#include "lock/registry.hpp"

#define JSON_INDENTATION 4

Song::Song() {
    new_song();
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
    lock_registry.clear();
    link_manager.set_links();
    calculate_song_length();

    gui.set_current_octave();
    gui.set_jump_step();
    gui.set_page_size();
    gui.clear_routing_nodes();
}

void Song::save_to_file(const std::string &filename) {
    const auto [temp_base, song_path] = prepare_temp_directory(clear_temp);
    const std::string directory = song_path.string();

    try {
        calculate_song_length();
        link_manager.set_links();

        export_all(directory);
        compress_directory(directory, filename);
        remove_temp_directory(temp_base, clear_temp);
    } catch (const std::exception &exception) {
        remove_temp_directory(temp_base, clear_temp);
        throw;
    }
}

void Song::load_from_file(const std::string &filename) {
    if (!std::filesystem::exists(filename)) {
        throw std::runtime_error("File does not exist: " + filename);
    }

    const auto [temp_base, song_path] = prepare_temp_directory(clear_temp);
    const std::string directory = song_path.string();

    try {
        decompress_archive(filename, directory);
        nlohmann::json json = import_header((song_path / "header.json").string());
        clear_data();
        import_all(directory, json);
        update_sizes();
        change_tuning(tuning.edo, tuning.a4_frequency);
        link_manager.set_links();
        calculate_song_length();
        remove_temp_directory(temp_base, clear_temp);
    } catch (const std::exception &exception) {
        remove_temp_directory(temp_base, clear_temp);
        throw;
    }
}

void Song::compile(const std::string &filename, const CompilationScheme scheme, const CompilationTarget compilation_target) const {
    const std::string platform = compilation_target == CompilationTarget::Linux ? "linux" : "windows";
    const auto [temp_base, song_path] = prepare_temp_directory(clear_temp);
    const std::string directory = song_path.string();

    try {
        export_all(directory);
        compile_sources(temp_base.string(), filename, scheme, platform);
        remove_temp_directory(temp_base, clear_temp);
    } catch (const std::exception &exception) {
        remove_temp_directory(temp_base, clear_temp);
        throw;
    }
}

void Song::render(const std::string &filename) {
    calculate_song_length();
    file_driver.set(filename, sample_rate, output_channels, song_length);
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
    output_channels = clamp(static_cast<int>(channels), 1, MAX_OUTPUT_CHANNELS);
}

void Song::export_all(const std::filesystem::path &directory) const {
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
    export_commands_sequences(directory);
    export_series(directory, "c_chan", commands_channels, {sizeof(CommandsChannel)});
    export_links((directory / "links.bin").string());
    export_gui_state(directory);
    export_lock_registry(directory);
}

void Song::import_all(const std::filesystem::path &directory, const nlohmann::json &json) {
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
    import_gui_state(directory);
    import_lock_registry(directory);
}

Envelope *Song::add_envelope(const size_t index) {
    if (envelopes.size() >= UINT8_MAX) {
        return nullptr;
    }

    Envelope *envelope = resource_manager.allocate<Envelope>();
    envelope->base_volume = DEFAULT_ENVELOPE_BASE_VOLUME;
    envelope->sustain_level = DEFAULT_ENVELOPE_SUSTAIN_LEVEL;
    envelope->attack = DEFAULT_ENVELOPE_ATTACK;
    envelope->decay = DEFAULT_ENVELOPE_DECAY;
    envelope->hold = DEFAULT_ENVELOPE_HOLD;
    envelope->release = DEFAULT_ENVELOPE_RELEASE;

    return insert_envelope(envelope, index);
}

Envelope *Song::insert_envelope(Envelope *envelope, const size_t index) {
    if (envelope == nullptr) {
        return nullptr;
    }

    if (index >= 0 && index < static_cast<int>(envelopes.size())) {
        envelopes.insert(envelopes.begin() + index, envelope);
        link_manager.realign_links(Target::ENVELOPE, index, false);
        lock_registry.realign_locks(Target::ENVELOPE, index, false);
        for (auto &channel : channels) {
            if (channel->envelope_index >= index) {
                channel->envelope_index++;
            }
        }
    } else {
        envelopes.push_back(envelope);
    }

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

Sequence *Song::add_sequence(const size_t index) {
    if (sequences.size() >= MAX_SEQUENCES) {
        return nullptr;
    }

    Sequence *sequence = resource_manager.allocate<Sequence>();
    sequence->size = DEFAULT_SEQUENCE_LENGTH;
    sequence->notes[0] = {NOTE_REST, DEFAULT_SEQUENCE_LENGTH};

    return insert_sequence(sequence, index);
}

Sequence *Song::insert_sequence(Sequence *sequence, const size_t index) {
    if (sequence == nullptr) {
        return nullptr;
    }

    if (index >= 0 && index < static_cast<int>(sequences.size())) {
        sequences.insert(sequences.begin() + index, sequence);
        link_manager.realign_links(Target::SEQUENCE, index, false);
        lock_registry.realign_locks(Target::SEQUENCE, index, false);
        /* potential conflict with commands sequences
        for (auto &order : orders) {
            for (size_t i = 0; i < order->order_length; i++) {
                if (order->sequences[i] >= index) {
                    order->sequences[i]++;
                }
            }
        }
        */
    } else {
        sequences.push_back(sequence);
    }

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

Order *Song::add_order(const size_t index) {
    if (orders.size() >= MAX_ORDERS) {
        return nullptr;
    }

    Order *order = resource_manager.allocate<Order>();
    order->order_length = DEFAULT_ORDER_LENGTH;

    return insert_order(order, index);
}

Order *Song::insert_order(Order *order, const size_t index) {
    if (order == nullptr) {
        return nullptr;
    }

    if (index >= 0 && index < static_cast<int>(orders.size())) {
        orders.insert(orders.begin() + index, order);
        link_manager.realign_links(Target::ORDER, index, false);
        lock_registry.realign_locks(Target::ORDER, index, false);
        for (auto &channel : channels) {
            if (channel->order_index >= index) {
                channel->order_index++;
            }
        }
        for (auto &commands_channel : commands_channels) {
            if (commands_channel->order_index >= index) {
                commands_channel->order_index++;
            }
        }
    } else {
        orders.push_back(order);
    }

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

Wavetable *Song::add_wavetable(const size_t index) {
    if (wavetables.size() >= MAX_WAVETABLES) {
        return nullptr;
    }

    Wavetable *wavetable = resource_manager.allocate<Wavetable>();
    wavetable->wavetable_size = DEFAULT_WAVETABLE_SIZE;
    wavetable->data.fill(0x80);

    return insert_wavetable(wavetable, index);
}

Wavetable *Song::insert_wavetable(Wavetable *wavetable, const size_t index) {
    if (wavetable == nullptr) {
        return nullptr;
    }

    if (index >= 0 && index < static_cast<int>(wavetables.size())) {
        wavetables.insert(wavetables.begin() + index, wavetable);
        link_manager.realign_links(Target::WAVETABLE, index, false);
        lock_registry.realign_locks(Target::WAVETABLE, index, false);
        for (auto &oscillator : oscillators) {
            Oscillator *generic = static_cast<Oscillator *>(oscillator);
            if (generic->generator_index == GENERATOR_WAVETABLE) {
                OscillatorWavetable *wavetable = static_cast<OscillatorWavetable *>(oscillator);
                if (wavetable->wavetable_index >= index) {
                    wavetable->wavetable_index++;
                }
            }
        }
    } else {
        wavetables.push_back(wavetable);
    }

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

void *Song::add_oscillator(const size_t index) {
    if (oscillators.size() >= MAX_OSCILLATORS) {
        return nullptr;
    }

    OscillatorSquare *oscillator = resource_manager.allocate<OscillatorSquare>();
    oscillator->duty_cycle = DEFAULT_OSCILLATOR_DUTY_CYCLE;

    return insert_oscillator(oscillator, index);
}

void *Song::insert_oscillator(void *oscillator, const size_t index) {
    if (oscillator == nullptr) {
        return nullptr;
    }

    if (index >= 0 && index < static_cast<int>(oscillators.size())) {
        oscillators.insert(oscillators.begin() + index, oscillator);
        link_manager.realign_links(Target::OSCILLATOR, index, false);
        lock_registry.realign_locks(Target::OSCILLATOR, index, false);
        for (auto &channel : channels) {
            if (channel->oscillator_index >= index) {
                channel->oscillator_index++;
            }
        }
    } else {
        oscillators.push_back(oscillator);
    }

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

Channel *Song::add_channel(const size_t index) {
    if (envelopes.size() >= MAX_CHANNELS) {
        return nullptr;
    }

    Channel *channel = resource_manager.allocate<Channel>();
    channel->envelope_index = 0;
    channel->order_index = 0;
    channel->oscillator_index = 0;
    channel->output_flag = DEFAULT_OUTPUT_FLAG;
    channel->pitch = DEFAULT_CHANNEL_PITCH;
    channel->target = 0;
    channel->splitter[0] = DEFAULT_SPLITTER_0;
    channel->splitter[1] = DEFAULT_SPLITTER_1;
    channel->splitter[2] = DEFAULT_SPLITTER_2;
    channel->splitter[3] = DEFAULT_SPLITTER_3;

    return insert_channel(channel, index);
}

Channel *Song::insert_channel(Channel *channel, const size_t index) {
    if (channel == nullptr) {
        return nullptr;
    }

    if (index >= 0 && index < static_cast<int>(channels.size())) {
        channels.insert(channels.begin() + index, channel);
        links[static_cast<size_t>(ItemType::CHANNEL)].insert(
            links[static_cast<size_t>(ItemType::CHANNEL)].begin() + index, Link()
        );
        link_manager.realign_links(Target::CHANNEL, index, false);
        lock_registry.realign_locks(Target::CHANNEL, index, false);
    } else {
        channels.push_back(channel);
        links[static_cast<size_t>(ItemType::CHANNEL)].push_back(Link());
    }

    num_channels = channels.size();
    link_manager.set_links();

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

void *Song::add_dsp(const size_t index) {
    if (dsps.size() >= MAX_DSPS) {
        return nullptr;
    }

    DSPGainer *dsp = resource_manager.allocate<DSPGainer>();
    dsp->dsp_size = SIZE_DSP_GAINER;
    dsp->output_flag = DEFAULT_OUTPUT_FLAG;
    dsp->splitter[0] = DEFAULT_SPLITTER_0;
    dsp->splitter[1] = DEFAULT_SPLITTER_1;
    dsp->splitter[2] = DEFAULT_SPLITTER_2;
    dsp->splitter[3] = DEFAULT_SPLITTER_3;
    dsp->effect_index = EFFECT_GAINER;
    dsp->volume = DEFAULT_GAINER_VOLUME;

    return insert_dsp(dsp, index);
}

void *Song::insert_dsp(void *dsp, const size_t index) {
    if (dsp == nullptr) {
        return nullptr;
    }

    if (index >= 0 && index < static_cast<int>(dsps.size())) {
        dsps.insert(dsps.begin() + index, dsp);
        links[static_cast<size_t>(ItemType::DSP)].insert(
            links[static_cast<size_t>(ItemType::DSP)].begin() + index, Link()
        );
        link_manager.realign_links(Target::DSP, index, false);
        link_manager.realign_links(Target::DIRECT_DSP, index, false);
        lock_registry.realign_locks(Target::DSP, index, false);
    } else {
        dsps.push_back(dsp);
        links[static_cast<size_t>(ItemType::DSP)].push_back(Link());
    }

    num_dsps = dsps.size();
    link_manager.set_links();

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

CommandsSequence *Song::add_commands_sequence(const size_t index) {
    if (commands_sequences.size() >= MAX_COMMANDS_SEQUENCES) {
        return nullptr;
    }

    CommandsSequence *sequence = resource_manager.allocate<CommandsSequence>();
    sequence->size = COMMANDS_SEQUENCE_DATA + commands_sizes[static_cast<size_t>(Instruction::Empty)];
    sequence->length = DEFAULT_COMMANDS_SEQUENCE_LENGTH;

    Command command;
    command.duration = DEFAULT_COMMANDS_SEQUENCE_LENGTH;
    sequence->commands[0] = command;

    return insert_commands_sequence(sequence, index);
}

CommandsSequence *Song::insert_commands_sequence(CommandsSequence *sequence, const size_t index) {
    if (sequence == nullptr) {
        return nullptr;
    }

    if (index >= 0 && index < static_cast<int>(commands_sequences.size())) {
        commands_sequences.insert(commands_sequences.begin() + index, sequence);
        link_manager.realign_links(Target::COMMANDS_SEQUENCE, index, false);
        lock_registry.realign_locks(Target::COMMANDS_SEQUENCE, index, false);
        /*
        for (auto &order : orders) {
            for (size_t i = 0; i < order->order_length; i++) {
                if (order->sequences[i] >= index) {
                    order->sequences[i]++;
                }
            }
        }
        */
    } else {
        commands_sequences.push_back(sequence);
    }

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

CommandsChannel *Song::add_commands_channel(const size_t index) {
    if (commands_channels.size() >= MAX_COMMANDS_CHANNELS) {
        return nullptr;
    }

    CommandsChannel *channel = resource_manager.allocate<CommandsChannel>();

    return insert_commands_channel(channel, index);
}

CommandsChannel *Song::insert_commands_channel(CommandsChannel *channel, const size_t index) {
    if (channel == nullptr) {
        return nullptr;
    }

    if (index >= 0 && index < static_cast<int>(commands_channels.size())) {
        commands_channels.insert(commands_channels.begin() + index, channel);
        link_manager.realign_links(Target::COMMANDS_CHANNEL, index, false);
        lock_registry.realign_locks(Target::COMMANDS_CHANNEL, index, false);
    } else {
        commands_channels.push_back(channel);
    }

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
        resource_manager.deallocate(envelopes[index]);
        envelopes.erase(envelopes.begin() + index);
        link_manager.realign_links(Target::ENVELOPE, index, true);
        lock_registry.realign_locks(Target::ENVELOPE, index, true);
        for (auto &channel : channels) {
            if (channel->envelope_index >= index) {
                channel->envelope_index = std::max(0, channel->envelope_index - 1);
            }
        }
    }
}

void Song::remove_sequence(const size_t index) {
    if (index < sequences.size()) {
        resource_manager.deallocate(sequences[index]);
        sequences.erase(sequences.begin() + index);
        link_manager.realign_links(Target::SEQUENCE, index, true);
        lock_registry.realign_locks(Target::SEQUENCE, index, true);
        /* potential conflict with commands sequences
        for (auto &order : orders) {
            for (size_t i = 0; i < order->order_length; i++) {
                if (order->sequences[i] >= index) {
                    order->sequences[i] = std::max(0, order->sequences[i] - 1);
                }
            }
        }
        */
    }
}

void Song::remove_order(const size_t index) {
    if (index < orders.size()) {
        resource_manager.deallocate(orders[index]);
        orders.erase(orders.begin() + index);
        link_manager.realign_links(Target::ORDER, index, true);
        lock_registry.realign_locks(Target::ORDER, index, true);
        for (auto &channel : channels) {
            if (channel->order_index >= index) {
                channel->order_index = std::max(0, channel->order_index - 1);
            }
        }
    }
}

void Song::remove_wavetable(const size_t index) {
    if (index < wavetables.size()) {
        resource_manager.deallocate(wavetables[index]);
        wavetables.erase(wavetables.begin() + index);
        link_manager.realign_links(Target::WAVETABLE, index, true);
        lock_registry.realign_locks(Target::WAVETABLE, index, true);
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
        resource_manager.deallocate(oscillators[index]);
        oscillators.erase(oscillators.begin() + index);
        link_manager.realign_links(Target::OSCILLATOR, index, true);
        lock_registry.realign_locks(Target::OSCILLATOR, index, true);
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
        resource_manager.deallocate(channels[index]);
        channels.erase(channels.begin() + index);
        links[link_type].erase(links[link_type].begin() + index);
        num_channels = channels.size();

        lock_registry.realign_locks(Target::CHANNEL, index, true);
        link_manager.realign_links(Target::CHANNEL, index, true);
        link_manager.set_links();
    }
}

void Song::remove_dsp(const size_t index) {
    if (index < dsps.size()) {
        size_t link_type = static_cast<size_t>(ItemType::DSP);
        resource_manager.deallocate(dsps[index]);
        dsps.erase(dsps.begin() + index);
        links[link_type].erase(links[link_type].begin() + index);
        num_dsps = dsps.size();

        lock_registry.realign_locks(Target::DSP, index, true);
        link_manager.realign_links(Target::DIRECT_DSP, index, true);
        link_manager.realign_links(Target::DSP, index, true);
        link_manager.set_links();
    }
}

void Song::remove_commands_sequence(const size_t index) {
    if (index < commands_sequences.size()) {
        resource_manager.deallocate(commands_sequences[index]);
        commands_sequences.erase(commands_sequences.begin() + index);
        link_manager.realign_links(Target::COMMANDS_SEQUENCE, index, true);
        lock_registry.realign_locks(Target::COMMANDS_SEQUENCE, index, true);
        /*
        for (auto &order : orders) {
            for (size_t i = 0; i < order->order_length; i++) {
                if (order->sequences[i] >= index) {
                    order->sequences[i] = std::max(0, order->sequences[i] - 1);
                }
            }
        }
        */
    }
}

void Song::remove_commands_channel(const size_t index) {
    if (index < commands_channels.size()) {
        resource_manager.deallocate(commands_channels[index]);
        commands_channels.erase(commands_channels.begin() + index);
        num_commands_channels = commands_channels.size();

        lock_registry.realign_locks(Target::COMMANDS_CHANNEL, index, true);
        link_manager.realign_links(Target::COMMANDS_CHANNEL, index, true);
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

std::vector<std::string> Song::find_envelope_dependencies(const size_t envelope_index) {
    std::set<std::string> dependencies;
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->envelope_index == envelope_index) {
            dependencies.insert(channel_names[i]);
        }
    }

    return std::vector<std::string>(dependencies.begin(), dependencies.end());
}

std::vector<std::string> Song::find_sequence_dependencies(const size_t sequence_index) {
    std::set<std::string> dependencies;
    std::set<size_t> channel_orders = get_channel_orders();
    for (const size_t order_index : channel_orders) {
        const Order *order = orders[order_index];
        for (size_t i = 0; i < order->order_length; i++) {
            if (order->sequences[i] == sequence_index) {
                dependencies.insert(order_names[order_index]);
            }
        }
    }

    return std::vector<std::string>(dependencies.begin(), dependencies.end());
}

std::vector<std::string> Song::find_order_dependencies(const size_t order_index) {
    std::set<std::string> dependencies;
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->order_index == order_index) {
            dependencies.insert(channel_names[i]);
        }
    }

    return std::vector<std::string>(dependencies.begin(), dependencies.end());
}

std::vector<std::string> Song::find_wavetable_dependencies(const size_t wavetable_index) {
    std::set<std::string> dependencies;
    for (size_t i = 0; i < oscillators.size(); i++) {
        const Oscillator *oscillator = static_cast<const Oscillator *>(oscillators[i]);
        if (oscillator->generator_index == GENERATOR_WAVETABLE) {
            const OscillatorWavetable *wavetable = static_cast<const OscillatorWavetable *>(oscillators[i]);
            if (wavetable->wavetable_index == wavetable_index) {
                dependencies.insert(oscillator_names[i]);
            }
        }
    }

    return std::vector<std::string>(dependencies.begin(), dependencies.end());
}

std::vector<std::string> Song::find_oscillator_dependencies(const size_t oscillator_index) {
    std::set<std::string> dependencies;
    for (size_t i = 0; i < channels.size(); i++) {
        if (channels[i]->oscillator_index == oscillator_index) {
            dependencies.insert(channel_names[i]);
        }
    }

    return std::vector<std::string>(dependencies.begin(), dependencies.end());
}

std::vector<std::string> Song::find_commands_sequence_dependencies(const size_t sequence_index) {
    std::set<std::string> dependencies;
    std::set<size_t> channel_orders = get_commands_channel_orders();
    for (const size_t order_index : channel_orders) {
        const Order *order = orders[order_index];
        for (size_t i = 0; i < order->order_length; i++) {
            if (order->sequences[i] == sequence_index) {
                dependencies.insert(order_names[order_index]);
            }
        }
    }

    return std::vector<std::string>(dependencies.begin(), dependencies.end());
}

void Song::add_dsp_dependencies(
    std::set<std::string> &dependencies,
    std::vector<std::string> &names,
    const std::vector<Link> &links,
    const size_t dsp_index
) {
    for (size_t i = 0; i < links.size(); i++) {
        const Link &link = links[i];
        if (link.target == Target::DIRECT_DSP) {
            if (link.index == dsp_index) {
                dependencies.insert(names[i]);
            }
        } else if (link.target == Target::SPLITTER_DSP) {
            if (dsp_index >= link.index && dsp_index < link.index + MAX_OUTPUT_CHANNELS) {
                dependencies.insert(names[i]);
            }
        }
    }
}

std::vector<std::string> Song::find_dsp_dependencies(const size_t dsp_index) {
    std::set<std::string> dependencies;
    const auto &channel_links = links[static_cast<size_t>(ItemType::CHANNEL)];
    add_dsp_dependencies(dependencies, channel_names, channel_links, dsp_index);

    const auto &dsp_links = links[static_cast<size_t>(ItemType::DSP)];
    add_dsp_dependencies(dependencies, dsp_names, dsp_links, dsp_index);

    return std::vector<std::string>(dependencies.begin(), dependencies.end());
}

float Song::calculate_real_bpm() {
    return unit * static_cast<float>(sample_rate) / static_cast<float>(ticks_per_beat);
}

float Song::get_row_duration() {
    return static_cast<float>(ticks_per_beat) / static_cast<float>(sample_rate);
}

void Song::generate_header_vector(
    std::stringstream &asm_content,
    const std::string &name,
    const std::string &short_name,
    const size_t size,
    const char separator
) {
    asm_content << "\n\n"
                << "CDECL(" << name << "s):\n";
    for (size_t i = 0; i < size; i++) {
        asm_content << "\nglobal " << name << "s." << name << "_" << i << ":\n";
        asm_content << "." << name << "_" << i << ":\n";
        asm_content << "incbin \"song" << separator << get_element_path(short_name + "s", short_name, i, separator) << "\"\n";
    }
}

void Song::set_used_flags(std::stringstream &asm_content) {
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
        asm_content << "    \%define USED_COMMAND\n";
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
    asm_content << "    extern CDECL(num_channels)" << "\n";
    asm_content << "    extern CDECL(num_dsps)" << "\n";
    asm_content << "    extern CDECL(num_commands_channels)" << "\n";
    asm_content << "    extern CDECL(unit)" << "\n";
    asm_content << "\n";
    return asm_content.str();
}

void Song::generate_targets_asm(std::stringstream &asm_content) {
    asm_content << "\n\n"
                << "align 4\n"
                << "CDECL(targets):\n";
    const auto pointers = link_manager.get_pointers_map();
    for (const auto &pair : pointers) {
        const LinkKey key = pair.second;
        asm_content << "    dd ";
        asm_content << LinkManager::get_link_reference(key) << "\n";
    }
}

std::string Song::generate_data_asm_file(const char separator) {
    std::stringstream asm_content;
    asm_content << "SEGMENT_DATA\n";
    asm_content << "CDECL(bpm):\n";
    asm_content << "dw " << bpm << "\n";
    asm_content << "CDECL(unit):\n";
    asm_content << "dd "
                << std::fixed
                << std::setprecision(std::numeric_limits<float>::max_digits10)
                << static_cast<float>(unit) << "\n";
    asm_content << "CDECL(normalizer):\n";
    asm_content << "dd "
                << std::fixed
                << std::setprecision(std::numeric_limits<float>::max_digits10)
                << static_cast<float>(normalizer) << "\n\n";
    asm_content << "CDECL(num_channels):\n";
    asm_content << "db " << static_cast<int>(num_channels) << "\n";
    asm_content << "CDECL(num_dsps):\n";
    asm_content << "db " << static_cast<int>(num_dsps) << "\n";
    asm_content << "CDECL(num_commands_channels):\n";
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
    generate_targets_asm(asm_content);

    return asm_content.str();
}

nlohmann::json Song::create_header_json() const {
    nlohmann::json json;
    json["general"] = {
        {"bpm", bpm},
        {"unit", static_cast<double>(unit)},
        {"sample_rate", sample_rate},
        {"normalizer", static_cast<double>(normalizer)},
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

nlohmann::json Song::save_gui_state() {
    nlohmann::json json;
    json["editor"] = {
        {"current_octave", gui.get_current_octave()},
        {"jump_step", gui.get_jump_step()},
        {"page_size", gui.get_page_size()},
    };

    json["current"] = {
        {"channel_index", gui.get_current_channel_index()},
        {"dsp_index", gui.get_current_dsp_index()},
        {"commands_channel_index", gui.get_current_commands_channel_index()},
        {"oscillator_index", gui.get_current_oscillator_index()},
        {"envelope_index", gui.get_current_envelope_index()},
        {"sequence_index", gui.get_current_sequence_index()},
        {"order_index", gui.get_current_order_index()},
        {"wavetable_index", gui.get_current_wavetable_index()},
        {"commands_sequence_index", gui.get_current_commands_sequence_index()},
    };

    json["routing"] = {};
    const auto nodes_positions = gui.get_routing_nodes_positions();
    for (const auto &pair : nodes_positions) {
        const std::string &node_name = pair.first.to_string();
        const auto &position = pair.second;
        json["routing"][node_name] = {
            {"x", position.x},
            {"y", position.y}
        };
    }

    return json;
}

void Song::calculate_song_length() {
    max_rows = 0;
    for (const auto &channel : channels) {
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

size_t Song::calculate_dsps(const Effect effect) {
    size_t count = 0;
    const uint8_t effect_value = static_cast<uint8_t>(effect);
    for (const auto &dsp : dsps) {
        if (static_cast<DSP *>(dsp)->effect_index == effect_value) {
            count++;
        }
    }
    return count;
}

size_t Song::calculate_oscillators(const Generator generator) {
    size_t count = 0;
    const uint8_t generator_value = static_cast<uint8_t>(generator);
    for (const auto &oscillator : oscillators) {
        if (static_cast<Oscillator *>(oscillator)->generator_index == generator_value) {
            count++;
        }
    }
    return count;
}

size_t Song::calculate_commands(const Instruction instruction) {
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

void Song::compress_directory(const std::string &directory, const std::string &output_file) {
    miniz_cpp::zip_file zip;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string rel_path = std::filesystem::relative(entry.path(), directory).string();
            std::replace(rel_path.begin(), rel_path.end(), '\\', '/');
            zip.write(entry.path().string(), rel_path);
        }
    }

    zip.save(output_file);
}

void Song::decompress_archive(const std::string &output_file, const std::string &directory) {
    miniz_cpp::zip_file zip(output_file);

    for (const auto &info : zip.infolist()) {
        std::filesystem::path out_path = std::filesystem::path(directory) / info.filename;
        if (!info.filename.empty() && info.filename.back() == '/') {
            std::filesystem::create_directories(out_path);
        } else {
            std::filesystem::create_directories(out_path.parent_path());
            std::ofstream out_file(out_path, std::ios::binary);
            out_file << zip.read(info.filename);
        }
    }
}

void Song::compile_sources(const std::string &directory, const std::string &filename, const CompilationScheme scheme, const std::string &platform) {
    std::stringstream compile_command;
    const std::filesystem::path scripts_path = get_scripts_path();
    const std::filesystem::path compile_script = scripts_path / "compile.py";
    const std::filesystem::path executable = get_python_path();

    compile_command << executable.string() << " "
                    << "\"" << compile_script.string() << "\" "
                    << "\"" << get_base_path().string() << "\" "
                    << "\"" << platform << "\" "
                    << "\"" << directory << "\" "
                    << "\"" << filename << "\"";

    std::cout << "Compiling sources with command: " << compile_command.str() << std::endl;

    switch (scheme) {
    case CompilationScheme::Uncompressed: {
        compile_command << " --uncompressed";
        break;
    }
    case CompilationScheme::Debug: {
        compile_command << " --debug";
        break;
    }
    case CompilationScheme::Compressed:
    default:
        break;
    }

    const auto &[exit_code, output] = run_command(compile_command.str());
    if (exit_code != 0) {
        throw std::runtime_error("Failed to compile sources: " + output);
    }
}

std::string Song::get_element_path(const std::string &directory, const std::string &prefix, const size_t i, const char separator) {
    return directory + separator + prefix + "_" + std::to_string(i) + ".bin";
}

std::string Song::get_element_path(const std::filesystem::path &directory, const std::string &prefix, const size_t i) {
    return (directory / (prefix + "_" + std::to_string(i) + ".bin")).string();
}

void Song::serialize_dsp(std::ofstream &file, void *dsp) {
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

void *Song::deserialize_dsp(std::ifstream &file) {
    DSP *generic = resource_manager.allocate<DSP>();
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

void *Song::deserialize_oscillator(std::ifstream &file) {
    uint8_t size;
    uint8_t oscillator_type;
    read_data(file, &size, sizeof(size));
    read_data(file, &oscillator_type, sizeof(oscillator_type));

    switch (static_cast<Generator>(oscillator_type)) {
    case Generator::Square: {
        OscillatorSquare *oscillator = resource_manager.allocate<OscillatorSquare>();
        read_data(file, &oscillator->duty_cycle, sizeof(oscillator->duty_cycle));
        return reinterpret_cast<void *>(oscillator);
    }
    case Generator::Saw: {
        OscillatorSaw *oscillator = resource_manager.allocate<OscillatorSaw>();
        read_data(file, &oscillator->reverse, sizeof(oscillator->reverse));
        return reinterpret_cast<void *>(oscillator);
    }
    case Generator::Sine: {
        OscillatorSine *oscillator = resource_manager.allocate<OscillatorSine>();
        return reinterpret_cast<void *>(oscillator);
    }
    case Generator::Wavetable: {
        OscillatorWavetable *oscillator = resource_manager.allocate<OscillatorWavetable>();
        read_data(file, &oscillator->wavetable_index, sizeof(oscillator->wavetable_index));
        read_data(file, &oscillator->interpolation, sizeof(oscillator->interpolation));
        return reinterpret_cast<void *>(oscillator);
    }
    case Generator::Noise: {
        OscillatorNoise *oscillator = resource_manager.allocate<OscillatorNoise>();
        return reinterpret_cast<void *>(oscillator);
    }
    case Generator::Count:
    default: {
        throw std::runtime_error("Unknown oscillator type: " + std::to_string(oscillator_type));
    }
    }
}

void Song::export_header_asm_file(const std::filesystem::path &directory) const {
    std::string asm_content = generate_header_asm_file();
    std::ofstream asm_file(directory / "header.asm");
    asm_file << asm_content;
    asm_file.close();
}

void Song::export_data_asm_file(const std::filesystem::path &directory) {
    std::string asm_content = generate_data_asm_file();
    std::ofstream asm_file(directory / "data.asm");
    asm_file << asm_content;
    asm_file.close();
}

void Song::export_header(const std::filesystem::path &directory) const {
    const nlohmann::json header = create_header_json();
    std::ofstream header_file(directory / "header.json");
    header_file << header.dump(JSON_INDENTATION);
    header_file.close();
}

void Song::export_gui_state(const std::filesystem::path &directory) {
    nlohmann::json gui_state = Song::save_gui_state();
    std::ofstream gui_file(directory / "gui.json");
    gui_file << gui_state.dump(JSON_INDENTATION);
    gui_file.close();
}

void Song::export_lock_registry(const std::filesystem::path &directory) {
    const std::filesystem::path lock_registry_path = directory / "lock.json";
    nlohmann::json lock_registry_json = lock_registry.to_json();
    std::ofstream lock_file(lock_registry_path);
    lock_file << lock_registry_json.dump(JSON_INDENTATION);
    lock_file.close();
}

template <typename T>
void Song::export_series(const std::filesystem::path &directory, const std::string &prefix, const std::vector<T> &series, const std::vector<size_t> &sizes) {
    const std::filesystem::path series_dir = directory / (prefix + "s");
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < series.size(); i++) {
        const std::string filename = Song::get_element_path(series_dir, prefix, i);
        export_data(filename, series[i], sizes[i % sizes.size()]);
    }
}

void Song::export_channels(const std::filesystem::path &directory) {
    const std::filesystem::path series_dir = directory / "chans";
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < channels.size(); i++) {
        const Channel *channel = channels[i];
        const std::string filename = get_element_path(series_dir, "chan", i);
        std::ofstream file(filename, std::ios::binary);
        channel->serialize(file);
        file.close();
    }
}

void Song::export_dsps(const std::filesystem::path &directory) {
    const std::filesystem::path dsps_dir = directory / "dsps";
    std::filesystem::create_directories(dsps_dir);

    for (size_t i = 0; i < dsps.size(); i++) {
        const std::string filename = get_element_path(dsps_dir, "dsp", i);
        std::ofstream file(filename, std::ios::binary);
        void *dsp = dsps[i];
        Song::serialize_dsp(file, dsp);
        file.close();
    }
}

void Song::export_commands_sequences(const std::filesystem::path &directory) {
    const std::filesystem::path series_dir = directory / "c_seqs";
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < commands_sequences.size(); i++) {
        const CommandsSequence *sequence = commands_sequences[i];
        const std::string filename = Song::get_element_path(series_dir, "c_seq", i);
        std::ofstream file(filename, std::ios::binary);
        sequence->serialize(file);
        file.close();
    }
}

template <typename T>
void Song::export_arrays(const std::filesystem::path &directory, const std::string &prefix, const std::vector<T> &arrays) {
    const std::filesystem::path series_dir = directory / (prefix + "s");
    std::filesystem::create_directories(series_dir);
    for (size_t i = 0; i < arrays.size(); i++) {
        const std::string filename = get_element_path(series_dir, prefix, i);
        std::ofstream file(filename, std::ios::binary);
        const T element = arrays[i];
        element->serialize(file);
        file.close();
    }
}

void Song::export_links(const std::filesystem::path &filename) {
    std::ofstream file(filename, std::ios::binary);
    for (const ItemType &type : {ItemType::CHANNEL, ItemType::DSP, ItemType::COMMANDS}) {
        const size_t link_type = static_cast<size_t>(type);
        for (const auto &link : links[link_type]) {
            link.serialize(file);
        }
    }
    file.close();
}

void Song::import_envelopes(const std::filesystem::path &directory, const nlohmann::json &json) {
    const size_t envelope_count = json["data"]["envelopes"];
    for (size_t i = 0; i < envelope_count; i++) {
        const std::string filename = get_element_path(directory / "envels", "envel", i);
        Envelope *envelope = resource_manager.allocate<Envelope>();
        std::ifstream file(filename, std::ios::binary);
        read_data(file, envelope, sizeof(Envelope));
        envelopes.push_back(envelope);
        file.close();
    }
}

void Song::import_sequences(const std::filesystem::path &directory, const nlohmann::json &json) {
    const size_t sequence_count = json["data"]["sequences"];
    for (size_t i = 0; i < sequence_count; i++) {
        const std::string filename = get_element_path(directory / "seqs", "seq", i);
        std::ifstream file(filename, std::ios::binary);
        Sequence *sequence = Sequence::deserialize(file);
        sequences.push_back(sequence);
        file.close();
    }
}

void Song::import_orders(const std::filesystem::path &directory, const nlohmann::json &json) {
    const size_t order_count = json["data"]["orders"];
    for (size_t i = 0; i < order_count; i++) {
        const std::string filename = get_element_path(directory / "orders", "order", i);
        std::ifstream file(filename, std::ios::binary);
        Order *order = Order::deserialize(file);
        orders.push_back(order);
        file.close();
    }
}

void Song::import_wavetables(const std::filesystem::path &directory, const nlohmann::json &json) {
    const size_t wavetable_count = json["data"]["wavetables"];
    for (size_t i = 0; i < wavetable_count; i++) {
        const std::string filename = get_element_path(directory / "waves", "wave", i);
        std::ifstream file(filename, std::ios::binary);
        Wavetable *wavetable = Wavetable::deserialize(file);
        wavetables.push_back(wavetable);
        file.close();
    }
}

void Song::import_oscillators(const std::filesystem::path &directory, const nlohmann::json &json) {
    const size_t oscillator_count = json["data"]["oscillators"];
    for (size_t i = 0; i < oscillator_count; i++) {
        const std::string filename = get_element_path(directory / "oscs", "osc", i);
        std::ifstream file(filename, std::ios::binary);
        void *oscillator = deserialize_oscillator(file);
        oscillators.push_back(oscillator);
        file.close();
    }
}

void Song::import_dsps(const std::filesystem::path &directory, const nlohmann::json &json) {
    const size_t dsp_count = json["data"]["dsps"];
    for (size_t i = 0; i < dsp_count; i++) {
        const std::string filename = get_element_path(directory / "dsps", "dsp", i);
        std::ifstream file(filename, std::ios::binary);
        void *dsp = deserialize_dsp(file);
        dsps.push_back(dsp);
        file.close();
    }
}

void Song::import_channels(const std::filesystem::path &directory, const nlohmann::json &json) {
    const size_t channel_count = json["data"]["channels"];
    for (size_t i = 0; i < channel_count; i++) {
        const std::string filename = get_element_path(directory / "chans", "chan", i);
        std::ifstream file(filename, std::ios::binary);
        Channel *channel = Channel::deserialize(file);
        channels.push_back(channel);
        file.close();
    }
}

void Song::import_commands_sequences(const std::filesystem::path &directory, const nlohmann::json &json) {
    const size_t sequence_count = json["data"]["commands_sequences"];
    for (size_t i = 0; i < sequence_count; i++) {
        const std::string filename = get_element_path(directory / "c_seqs", "c_seq", i);
        std::ifstream file(filename, std::ios::binary);
        CommandsSequence *sequence = CommandsSequence::deserialize(file);
        commands_sequences.push_back(sequence);
        file.close();
    }
}

void Song::import_commands_channels(const std::filesystem::path &directory, const nlohmann::json &json) {
    const size_t channel_count = json["data"]["commands_channels"];
    for (size_t i = 0; i < channel_count; i++) {
        const std::string filename = get_element_path(directory / "c_chans", "c_chan", i);
        std::ifstream file(filename, std::ios::binary);
        CommandsChannel *channel = resource_manager.allocate<CommandsChannel>();
        read_data(file, channel, sizeof(CommandsChannel));
        commands_channels.push_back(channel);
        file.close();
    }
}

void Song::import_links(const std::filesystem::path &directory, const nlohmann::json &json) {
    const size_t channel_count = json["data"]["channels"];
    const size_t dsp_count = json["data"]["dsps"];
    const size_t commands_count = json["data"]["commands"];
    const std::string links_filename = (directory / "links.bin").string();
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
        const auto [sequence_index, channel_index] = LinkManager::unpack_command_id(link.id);
        commands_links[sequence_index][channel_index] = link;
    }

    link_manager.set_links();
    file.close();
}

void Song::import_gui_state(const std::filesystem::path &directory) {
    const std::string gui_filename = (directory / "gui.json").string();
    nlohmann::json json = read_json(gui_filename);
    const auto &json_editor = json["editor"];
    gui.set_current_octave(json_editor.value("current_octave", GUI_DEFAULT_CURRENT_OCTAVE));
    gui.set_jump_step(json_editor.value("jump_step", GUI_DEFAULT_JUMP_STEP));
    gui.set_page_size(json_editor.value("page_size", GUI_DEFAULT_PAGE_SIZE));

    const auto &json_current = json["current"];
    gui.set_current_channel_index(json_current.value("channel_index", 0));
    gui.set_current_dsp_index(json_current.value("dsp_index", 0));
    gui.set_current_commands_channel_index(json_current.value("commands_channel_index", 0));
    gui.set_current_oscillator_index(json_current.value("oscillator_index", 0));
    gui.set_current_envelope_index(json_current.value("envelope_index", 0));
    gui.set_current_sequence_index(json_current.value("sequence_index", 0));
    gui.set_current_order_index(json_current.value("order_index", 0));
    gui.set_current_wavetable_index(json_current.value("wavetable_index", 0));
    gui.set_current_commands_sequence_index(json_current.value("commands_sequence_index", 0));

    const auto &json_routing = json["routing"];
    std::vector<std::pair<NodeIdentifier, ImVec2>> nodes_positions;
    for (const auto &pair : json_routing.items()) {
        const std::string &node_name = pair.key();
        const auto &position = pair.value();
        const NodeIdentifier node_identifier(node_name);
        const ImVec2 node_position(position["x"], position["y"]);
        nodes_positions.emplace_back(node_identifier, node_position);
    }

    gui.update();
    gui.from();
    gui.set_routing_nodes_positions(nodes_positions);
}

void Song::import_lock_registry(const std::filesystem::path &directory) {
    const std::filesystem::path lock_registry_path = directory / "lock.json";
    if (!std::filesystem::exists(lock_registry_path)) {
        return;
    }

    nlohmann::json lock_registry_json = read_json(lock_registry_path.string());
    lock_registry.from_json(lock_registry_json);
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
    LinkManager::reset();
    update_sizes();
    buffers.clear();
    resource_manager.clear();
}

std::set<size_t> Song::get_channel_orders() {
    std::set<size_t> channel_orders;
    for (const Channel *channel : channels) {
        if (channel->order_index >= orders.size()) {
            continue;
        }

        channel_orders.insert(channel->order_index);
    }

    return channel_orders;
}

std::set<size_t> Song::get_commands_channel_orders() {
    std::set<size_t> commands_channel_orders;
    for (const CommandsChannel *channel : commands_channels) {
        if (channel->order_index >= orders.size()) {
            continue;
        }

        commands_channel_orders.insert(channel->order_index);
    }

    return commands_channel_orders;
}
