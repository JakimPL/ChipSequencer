#include <numeric>
#include <string>

#include "../../general.hpp"
#include "../../structures.hpp"
#include "../../maps/commands.hpp"
#include "../init.hpp"
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
    if (ImGui::BeginTable("SummaryTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Element");
        ImGui::TableSetupColumn("Count");
        ImGui::TableSetupColumn("Size (bytes)");
        ImGui::TableHeadersRow();

        size_t total_size = 0;

        // Channels
        const size_t channels_count = channels.size();
        size_t channels_size = channels_count * sizeof(Channel);
        total_size += channels_size;
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
            dsps_size += 1 + sizeof(uint32_t);
            switch (static_cast<Effect>(effect)) {
            case Effect::Gainer: {
                dsps_size += SIZE_DSP_GAINER;
                break;
            }
            case Effect::Distortion: {
                dsps_size += SIZE_DSP_DISTORTION;
                break;
            }
            case Effect::Filter: {
                dsps_size += SIZE_DSP_FILTER;
                break;
            }
            case Effect::Delay: {
                dsps_size += SIZE_DSP_DELAY;
                break;
            }
            }
        }
        total_size += dsps_size;
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
        total_size += envelopes_size;
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
            const size_t sequence_size = sequence->size + 1;
            total_size += sequence_size;
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
            const size_t order_size = order->order_length + 1;
            total_size += order_size;
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
            oscillators_size += 1;
            switch (static_cast<Generator>(generic->generator_index)) {
            case Generator::Square: {
                oscillators_size += SIZE_OSCILLATOR_SQUARE;
                break;
            }
            case Generator::Saw: {
                oscillators_size += SIZE_OSCILLATOR_SAW;
                break;
            }
            case Generator::Sine: {
                oscillators_size += SIZE_OSCILLATOR_SINE;
                break;
            }
            case Generator::Wavetable: {
                oscillators_size += SIZE_OSCILLATOR_WAVETABLE;
                break;
            }
            case Generator::Noise: {
                oscillators_size += SIZE_OSCILLATOR_NOISE;
                break;
            }
            }
        }

        total_size += oscillators_size;
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
            wavetables_size += wavetable->wavetable_size + 1;
        }

        total_size += wavetables_size;
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
        total_size += commands_channels_size;

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
                commands_sequences_size += command_sizes.at(command.instruction);
            }
        }

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Commands sequences");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", commands_sequences_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", commands_sequences_size);

        // Total
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Total song size");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text(" ");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", total_size);

        ImGui::EndTable();
    }
}

void GUISummaryPanel::draw_optimizations() {
    ImGui::Separator();
    ImGui::Text("Optimizations:");

    if (dsps.empty()) {
        ImGui::BulletText("Disabled all DSPs");
    } else {
        if (song.calculate_dsps(EFFECT_GAINER) == 0) {
            ImGui::BulletText("Disabled gainer effect");
        }
        if (song.calculate_dsps(EFFECT_DISTORTION) == 0) {
            ImGui::BulletText("Disabled distortion effect");
        }
        if (song.calculate_dsps(EFFECT_FILTER) == 0) {
            ImGui::BulletText("Disabled filter effect");
        }
        if (song.calculate_dsps(EFFECT_DELAY) == 0) {
            ImGui::BulletText("Disabled delay effect");
        }
    }

    if (song.calculate_oscillators(GENERATOR_SQUARE) == 0) {
        ImGui::BulletText("Disabled square oscillator");
    }
    if (song.calculate_oscillators(GENERATOR_SINE) == 0) {
        ImGui::BulletText("Disabled sine oscillator");
    }
    if (song.calculate_oscillators(GENERATOR_SAW) == 0) {
        ImGui::BulletText("Disabled saw oscillator");
    }
    if (song.calculate_oscillators(GENERATOR_NOISE) == 0) {
        ImGui::BulletText("Disabled noise oscillator");
    }

    if (commands_channels.empty()) {
        ImGui::BulletText("Disabled all commands");
    } else {
        if (song.calculate_commands(INSTRUCTION_PORTAMENTO_UP) == 0) {
            ImGui::BulletText("Disabled portamento up command");
        }
        if (song.calculate_commands(INSTRUCTION_PORTAMENTO_DOWN) == 0) {
            ImGui::BulletText("Disabled portamento down command");
        }
        if (song.calculate_commands(INSTRUCTION_SET_MASTER_GAINER) == 0) {
            ImGui::BulletText("Disabled set master gainer command");
        }
        if (song.calculate_commands(INSTRUCTION_SET_BPM) == 0) {
            ImGui::BulletText("Disabled set BPM command");
        }
        if (song.calculate_commands(INSTRUCTION_SET_DIVISION) == 0) {
            ImGui::BulletText("Disabled set division command");
        }
        if (song.calculate_commands(INSTRUCTION_CHANGE_BYTE_VALUE) == 0) {
            ImGui::BulletText("Disabled change byte value command");
        }
        if (song.calculate_commands(INSTRUCTION_CHANGE_WORD_VALUE) == 0) {
            ImGui::BulletText("Disabled change word value command");
        }
        if (song.calculate_commands(INSTRUCTION_CHANGE_DWORD_VALUE) == 0) {
            ImGui::BulletText("Disabled change dword value command");
        }
        if (song.calculate_commands(INSTRUCTION_CHANGE_FLOAT_VALUE) == 0) {
            ImGui::BulletText("Disabled change float value command");
        }
    }
}
