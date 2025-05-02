#include <numeric>
#include <string>

#include "../../general.hpp"
#include "../../structures.hpp"
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
        size_t channels_count = channels.size();
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
        size_t dsps_count = dsps.size();
        size_t dsps_size = 0;
        for (const auto *dsp : dsps) {
            const DSP *generic = static_cast<const DSP *>(dsp);
            const size_t effect = generic->effect_index;
            dsps_size += 1 + sizeof(uint32_t);
            switch (effect) {
            case EFFECT_GAINER: {
                dsps_size += SIZE_DSP_GAINER;
                break;
            }
            case EFFECT_DISTORTION: {
                dsps_size += SIZE_DSP_DISTORTION;
                break;
            }
            case EFFECT_FILTER: {
                dsps_size += SIZE_DSP_FILTER;
                break;
            }
            case EFFECT_DELAY: {
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
        size_t envelopes_count = envelopes.size();
        size_t envelopes_size = envelopes_count * sizeof(Envelope);
        total_size += envelopes_size;
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Envelopes");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%zu", envelopes_count);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%zu", envelopes_size);

        // Sequences
        size_t sequences_size = 0;
        size_t sequences_count = sequences.size();
        for (const Sequence *sequence : sequences) {
            const size_t sequence_size = sequence->data_size + 1;
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
        size_t oscillators_count = oscillators.size();
        size_t oscillators_size = 0;
        for (const void *oscillator : oscillators) {
            const Oscillator *generic = static_cast<const Oscillator *>(oscillator);
            oscillators_size += 1;
            switch (generic->generator_index) {
            case GENERATOR_SQUARE: {
                oscillators_size += SIZE_OSCILLATOR_SQUARE;
                break;
            }
            case GENERATOR_SAW: {
                oscillators_size += SIZE_OSCILLATOR_SAW;
                break;
            }
            case GENERATOR_SINE: {
                oscillators_size += SIZE_OSCILLATOR_SINE;
                break;
            }
            case GENERATOR_WAVETABLE: {
                oscillators_size += SIZE_OSCILLATOR_WAVETABLE;
                break;
            }
            case GENERATOR_NOISE: {
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
        size_t wavetables_count = wavetables.size();
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

        // Total Row
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
}
