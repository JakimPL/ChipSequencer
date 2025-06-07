#include <numeric>
#include <string>
#include <iostream>

#include "../../general.hpp"
#include "../../structures.hpp"
#include "../../maps/commands.hpp"
#include "../../maps/dsps.hpp"
#include "../../maps/oscillators.hpp"
#include "../../maps/sizes.hpp"
#include "../init.hpp"
#include "../names.hpp"
#include "summary.hpp"

GUISummaryPanel::GUISummaryPanel(const bool visible)
    : GUIPanel(visible) {
}

void GUISummaryPanel::draw() {
    ImGui::Begin("Summary");

    draw_summary();
    draw_optimizations();

    ImGui::End();
}

void GUISummaryPanel::draw_summary() {
    size_t player_size = draw_summary_components();
    size_t song_data_size = draw_summary_song_data();
    size_t total_size = player_size + song_data_size;

    if (ImGui::BeginTable("SummaryTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Item");
        ImGui::TableSetupColumn("Size (bytes)");
        ImGui::TableHeadersRow();

        // Player size
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Code size");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", player_size);

        // Song data size
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Song data size");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", song_data_size);

        // Total size
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Total size");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", total_size);

        ImGui::EndTable();
    }
}

size_t GUISummaryPanel::draw_summary_components() {
    size_t player_size = 0;
    if (ImGui::BeginTable("SummaryComponentsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Component");
        ImGui::TableSetupColumn("Size (bytes)");
        ImGui::TableHeadersRow();

        const size_t core_size = module_sizes.at("Core");
        player_size = core_size;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Core");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", core_size);

        if (!dsps.empty()) {
            const size_t dsp_module_size = module_sizes.at("DSPs");
            player_size += dsp_module_size;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("DSP module");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%zu", dsp_module_size);

            for (size_t i = 0; i < effect_names.size(); ++i) {
                const Effect effect = static_cast<Effect>(i);
                const std::string &name = effect_names[i];
                if (song.calculate_dsps(effect) > 0) {
                    const size_t component_size = component_sizes.at("DSPs").at(name);
                    player_size += component_size;

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", name.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%zu", component_size);
                }
            }
        }

        for (size_t i = 0; i < generator_names.size(); ++i) {
            const Generator generator = static_cast<Generator>(i);
            const std::string &name = generator_names[i];
            if (song.calculate_oscillators(generator) > 0) {
                const size_t component_size = component_sizes.at("Oscillators").at(name);
                player_size += component_size;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", name.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%zu", component_size);
            }
        }

        if (!commands_channels.empty()) {
            const size_t commands_module_size = module_sizes.at("Commands");
            player_size += commands_module_size;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Commands module");
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%zu", commands_module_size);

            for (size_t i = 0; i < instruction_names.size(); ++i) {
                const Instruction instruction = static_cast<Instruction>(i);
                const std::string &name = instruction_names.at(instruction);
                if (song.calculate_commands(instruction) > 0) {
                    const size_t component_size = component_sizes.at("Commands").at(name);
                    player_size += component_size;

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", name.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%zu", component_size);
                }
            }
        }

        // Total
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Code size");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", player_size);

        ImGui::EndTable();
    }

    return player_size;
}

size_t GUISummaryPanel::draw_summary_song_data() {
    size_t song_data_size = 0;
    if (ImGui::BeginTable("SummarySongDataTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Element");
        ImGui::TableSetupColumn("Count");
        ImGui::TableSetupColumn("Size (bytes)");
        ImGui::TableHeadersRow();

        // Channels
        const size_t channels_count = channels.size();
        size_t channels_size = channels_count * sizeof(Channel);
        song_data_size += channels_size;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Channels");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", channels_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", channels_size);

        // DSPs
        const size_t dsps_count = dsps.size();
        size_t dsps_size = 0;
        for (const auto *dsp : dsps) {
            const DSP *generic = static_cast<const DSP *>(dsp);
            const size_t effect = generic->effect_index;
            dsps_size += 1 + sizeof(uint32_t) + dsps_sizes.at(effect);
        }

        song_data_size += dsps_size;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("DSPs");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", dsps_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", dsps_size);

        // Envelopes
        const size_t envelopes_count = envelopes.size();
        const size_t envelopes_size = envelopes_count * sizeof(Envelope);
        song_data_size += envelopes_size;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Envelopes");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", envelopes_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", envelopes_size);

        // Sequences
        const size_t sequences_count = sequences.size();
        size_t sequences_size = 0;
        for (const Sequence *sequence : sequences) {
            const size_t sequence_size = sequence->size + SEQUENCE_NOTES;
            song_data_size += sequence_size;
            sequences_size += sequence_size;
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Sequences");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", sequences_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", sequences_size);

        // Orders
        size_t orders_size = 0;
        size_t orders_count = orders.size();
        for (const Order *order : orders) {
            const size_t order_size = order->order_length + ORDER_SEQUENCES;
            song_data_size += order_size;
            orders_size += order_size;
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Orders");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", orders_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", orders_size);

        // Oscillators
        const size_t oscillators_count = oscillators.size();
        size_t oscillators_size = 0;
        for (const void *oscillator : oscillators) {
            const Oscillator *generic = static_cast<const Oscillator *>(oscillator);
            oscillators_size += 1 + oscillators_sizes.at(generic->generator_index);
        }

        song_data_size += oscillators_size;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Oscillators");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", oscillators_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", oscillators_size);

        // Wavetables
        const size_t wavetables_count = wavetables.size();
        size_t wavetables_size = 0;
        for (const Wavetable *wavetable : wavetables) {
            wavetables_size += wavetable->wavetable_size + WAVETABLE_DATA;
        }

        song_data_size += wavetables_size;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Wavetables");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", wavetables_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", wavetables_size);

        // Commands channels
        const size_t commands_channels_count = commands_channels.size();
        const size_t commands_channels_size = commands_channels_count * sizeof(CommandsChannel);
        song_data_size += commands_channels_size;

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Commands channels");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", commands_channels_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", commands_channels_size);

        // Commands sequences
        size_t commands_sequences_count = commands_sequences.size();
        size_t commands_sequences_size = 0;
        for (const CommandsSequence *sequence : commands_sequences) {
            commands_sequences_size += COMMANDS_SEQUENCE_DATA;
            const size_t sequence_size = sequence->length;
            for (size_t i = 0; i < sequence_size; i++) {
                const Command &command = sequence->commands[i];
                commands_sequences_size += commands_sizes.at(command.instruction);
            }
        }

        song_data_size += commands_sequences_size;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Commands sequences");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", commands_sequences_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", commands_sequences_size);

        // Message size
        const size_t message_size = strlen(song.get_message().c_str()) + 1;
        song_data_size += message_size;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Song message");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", message_size);

        // Total
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Song data size");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text(" ");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", song_data_size);

        ImGui::EndTable();
    }

    return song_data_size;
}

void GUISummaryPanel::draw_optimizations() {
    ImGui::Separator();
    ImGui::Text("Optimizations:");

    if (dsps.empty()) {
        ImGui::BulletText("Disabled all DSPs");
    } else {
        for (size_t i = 0; i < effect_names.size(); ++i) {
            const Effect effect = static_cast<Effect>(i);
            const std::string &name = effect_names[i];
            if (song.calculate_dsps(effect) == 0) {
                ImGui::BulletText("Disabled effect: %s", name.c_str());
            }
        }
    }

    for (size_t i = 0; generator_names.size() > i; ++i) {
        const Generator generator = static_cast<Generator>(i);
        const std::string &name = generator_names[i];
        if (song.calculate_oscillators(generator) == 0) {
            ImGui::BulletText("Disabled generator: %s", generator_names[i].c_str());
        }
    }

    if (commands_channels.empty()) {
        ImGui::BulletText("Disabled all commands");
    } else {
        for (const auto &[instruction, name] : instruction_names) {
            if (instruction == Instruction::Empty) {
                continue;
            }

            if (song.calculate_commands(instruction) == 0) {
                ImGui::BulletText("Disabled command: %s", name);
            }
        }
    }
}
