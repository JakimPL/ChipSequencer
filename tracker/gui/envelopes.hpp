#ifndef GUI_ENVELOPES_HPP
#define GUI_ENVELOPES_HPP

#include <algorithm>
#include <string>
#include "init.hpp"

class EnvelopeGUI {
  private:
    struct CurrentEnvelope {
        int base_volume;
        int sustain_level;
        int attack;
        int decay;
        int hold;
        int release;
    } current_envelope;

    int envelope_index = 0;
    std::vector<std::string> envelope_names;

    void from_envelope() {
        const Envelope *envelope = envelopes[envelope_index];
        current_envelope.base_volume = static_cast<int>(envelope->base_volume);
        current_envelope.sustain_level = static_cast<int>(envelope->sustain_level);
        current_envelope.attack = static_cast<int>(envelope->attack);
        current_envelope.decay = static_cast<int>(envelope->decay);
        current_envelope.hold = static_cast<int>(envelope->hold);
        current_envelope.release = static_cast<int>(envelope->release);
    }

    void to_envelope() const {
        Envelope *envelope = envelopes[envelope_index];
        envelope->base_volume = static_cast<uint16_t>(std::clamp(current_envelope.base_volume, 0, UINT16_MAX));
        envelope->sustain_level = static_cast<uint16_t>(std::clamp(current_envelope.sustain_level, 0, UINT16_MAX));
        envelope->attack = static_cast<uint16_t>(std::clamp(current_envelope.attack, 0, UINT16_MAX));
        envelope->decay = static_cast<uint16_t>(std::clamp(current_envelope.decay, 0, UINT16_MAX));
        envelope->hold = static_cast<uint16_t>(std::clamp(current_envelope.hold, 0, UINT16_MAX));
        envelope->release = static_cast<uint16_t>(std::clamp(current_envelope.release, 0, UINT16_MAX));
    }

    void update_envelopes() {
        envelope_names.clear();
        for (size_t i = 0; i < envelopes.size(); ++i) {
            envelope_names.emplace_back("Envelope " + std::to_string(i + 1));
        }
        if (envelope_index >= static_cast<int>(envelope_names.size())) {
            envelope_index = static_cast<int>(envelope_names.size()) - 1;
        }
        if (envelope_index < 0 && !envelope_names.empty()) {
            envelope_index = 0;
        }
    }

    void prepare_envelopes_combo() {
        std::vector<const char *> envelope_names_cstr;
        for (const auto &name : envelope_names) {
            envelope_names_cstr.push_back(name.c_str());
        }

        ImGui::Combo("##EnvelopeCombo", &envelope_index, envelope_names_cstr.data(), envelope_names_cstr.size());
        from_envelope();
    }

    void draw_levels() {
        ImGui::Text("Levels");
        ImGui::SliderInt("Base Volume", &current_envelope.base_volume, 0, UINT16_MAX, "");
        ImGui::SliderInt("Sustain Level", &current_envelope.sustain_level, 0, UINT16_MAX, "");
    }

    void add_slider(const char *label, int &reference, int min, int max) {
        std::string slider_id = std::string("##") + label + "Slider";
        std::string input_id = std::string("##") + label + "Input";
        ImGui::PushID(label);
        ImGui::SliderInt(slider_id.c_str(), &reference, min, max, "");
        ImGui::SameLine();
        ImGui::InputInt(input_id.c_str(), &reference, min, max);
        ImGui::PopID();
    }

    void draw_timers() {
        ImGui::Text("Timers");
        add_slider("Attack", current_envelope.attack, 0, 10000);
        add_slider("Decay", current_envelope.decay, 0, 10000);
        add_slider("Hold", current_envelope.hold, 0, 10000);
        add_slider("Release", current_envelope.release, 0, 10000);
    }

    void draw_envelope_graph() {
        ImGui::Separator();
        ImGui::Text("Envelope Graph");
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        float width = ImGui::GetContentRegionAvail().x;
        float height = 200.0f;
        ImVec2 size = ImVec2(width, height);
        ImGui::InvisibleButton("canvas", size);
        ImVec2 canvas_p0 = p;
        ImVec2 canvas_p1 = ImVec2(p.x + size.x, p.y + size.y);

        draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
        float attack_time = static_cast<float>(current_envelope.attack) / 10000.0f;
        float release_time = static_cast<float>(current_envelope.release) / 10000.0f;
        draw_list->AddLine(
            ImVec2(canvas_p0.x, canvas_p1.y),
            ImVec2(canvas_p0.x + size.x * attack_time, canvas_p0.y),
            IM_COL32(255, 0, 0, 255),
            2.0f);
        draw_list->AddLine(
            ImVec2(canvas_p0.x + size.x * attack_time, canvas_p0.y),
            ImVec2(canvas_p1.x, canvas_p1.y),
            IM_COL32(0, 255, 0, 255),
            2.0f);
    }

  public:
    EnvelopeGUI() {
        update_envelopes();
    }

    void draw() {
        ImGui::Begin("Envelope Editor");

        if (envelopes.empty()) {
            ImGui::Text("No envelopes available.");
            ImGui::End();
            return;
        }

        prepare_envelopes_combo();

        ImGui::Columns(2, "top_columns");
        draw_levels();
        ImGui::NextColumn();
        draw_timers();

        ImGui::Columns(1);
        draw_envelope_graph();
        ImGui::End();
        to_envelope();
    }
};

#endif // GUI_ENVELOPES_HPP
