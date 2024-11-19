#include <string>
#include <vector>
#include "../init.hpp"

#define NOTE_REST -2
#define NOTE_OFF -1

const char *note_names[] = {
    "C-0",
    "C#0",
    "D-0",
    "D#0",
    "E-0",
    "F-0",
    "F#0",
    "G-0",
    "G#0",
    "A-0",
    "A#0",
    "B-0",
    "C-1",
    "C#1",
    "D-1",
    "D#1",
    "E-1",
    "F-1",
    "F#1",
    "G-1",
    "G#1",
    "A-1",
    "A#1",
    "B-1",
    "C-2",
    "C#2",
    "D-2",
    "D#2",
    "E-2",
    "F-2",
    "F#2",
    "G-2",
    "G#2",
    "A-2",
    "A#2",
    "B-2",
    "C-3",
    "C#3",
    "D-3",
    "D#3",
    "E-3",
    "F-3",
    "F#3",
    "G-3",
    "G#3",
    "A-3",
    "A#3",
    "B-3",
    "C-4",
    "C#4",
    "D-4",
    "D#4",
    "E-4",
    "F-4",
    "F#4",
    "G-4",
    "G#4",
    "A-4",
    "A#4",
    "B-4",
    "C-5",
    "C#5",
    "D-5",
    "D#5",
    "E-5",
    "F-5",
    "F#5",
    "G-5",
    "G#5",
    "A-5",
    "A#5",
    "B-5",
    "C-6",
    "C#6",
    "D-6",
    "D#6",
    "E-6",
    "F-6",
    "F#6",
    "G-6",
    "G#6",
    "A-6",
    "A#6",
    "B-6",
    "C-7",
    "C#7",
    "D-7",
    "D#7",
    "E-7",
    "F-7",
    "F#7",
    "G-7",
    "G#7",
    "A-7",
    "A#7",
    "B-7",
    "C-8",
    "C#8",
    "D-8",
    "D#8",
    "E-8",
    "F-8",
    "F#8",
    "G-8",
    "G#8",
    "A-8",
    "A#8",
    "B-8",
    "C-9",
    "C#9",
    "D-9",
    "D#9",
    "E-9",
    "F-9",
    "F#9",
    "G-9",
    "G#9",
    "A-9",
    "A#9",
    "B-9",
    "C-10"};

void prepare_combo(const std::vector<std::string> &names, std::string label, int &index) {
    std::vector<const char *> names_cstr;
    for (const auto &name : names) {
        names_cstr.push_back(name.c_str());
    }

    float comboWidth = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(comboWidth);
    ImGui::Combo(label.c_str(), &index, names_cstr.data(), names_cstr.size());
}

void update_items(std::vector<std::string> &names, size_t size, std::string label, int &index) {
    names.clear();
    for (size_t i = 0; i < size; ++i) {
        names.emplace_back(label + std::to_string(i + 1));
    }
    if (index >= static_cast<int>(names.size())) {
        index = static_cast<int>(names.size()) - 1;
    }
    if (index < 0 && !names.empty()) {
        index = 0;
    }
}

std::string get_note_name(int8_t midi_value) {
    if (midi_value == NOTE_REST) return "...";
    if (midi_value == NOTE_OFF) return "===";

    int max_notes = sizeof(note_names) / sizeof(note_names[0]);
    if (midi_value < max_notes) {
        return std::string(note_names[midi_value]);
    }
    return "???";
}

uint8_t get_midi_value(const std::string &note_name) {
    for (size_t i = 0; i < sizeof(note_names) / sizeof(note_names[0]); ++i) {
        if (note_name == note_names[i]) {
            return static_cast<int8_t>(i);
        }
    }

    return NOTE_REST;
}