#include <algorithm>
#include <cmath>

#include "../utils/math.hpp"
#include "scale.hpp"

ptrdiff_t ScaleComposer::get_ordered_index(const std::string &note_name) {
    const auto *begin = std::begin(scale_names_ordered);
    const auto *end = std::end(scale_names_ordered);
    const auto *it = std::find(begin, end, note_name);
    if (it != end) {
        return std::distance(begin, it);
    }
    return -1;
}

void ScaleComposer::compose(const int new_edo) {
    edo = std::clamp(new_edo, MIN_EDO, MAX_EDO);
    a_index = -1;

    const int scale_names_count = static_cast<int>(std::size(scale_names));
    const int start = static_cast<int>(std::floor(-static_cast<double>(edo) / 2.0)) + 4;
    const int end = static_cast<int>(std::floor(static_cast<double>(edo) / 2.0)) + 4;

    std::vector<std::pair<uint16_t, std::string>> notes;
    notes.reserve(end - start);

    for (int i = start; i < end; ++i) {
        const int offset = std::floor(static_cast<float>(i) / scale_names_count);
        const int name_index = mod(i, scale_names_count);
        const std::string name = scale_names[name_index];
        const ptrdiff_t ordered_index = get_ordered_index(name);
        const uint16_t rank = static_cast<int>(ordered_index) * 5000 + offset;

        const std::string note_name = render(name, offset);
        const std::pair<int, std::string> note(rank, note_name);
        notes.emplace_back(note);
    }

    if (notes.size() != static_cast<size_t>(edo)) {
        throw std::runtime_error("Scale creation algorithm failed.");
    }

    std::sort(notes.begin(), notes.end(), [](const auto &a, const auto &b) {
        return a.first < b.first;
    });

    for (size_t i = 0; i < notes.size(); ++i) {
        const auto &note = notes[i];
        scale[i] = note.second;
        if (note.second == "A") {
            a_index = static_cast<int>(i);
        }
    }
}

int ScaleComposer::get_edo() const {
    return edo;
}

size_t ScaleComposer::get_a_index() const {
    return a_index;
}

std::array<std::string, MAX_EDO> ScaleComposer::get_scale() const {
    return scale;
}

std::string ScaleComposer::render(const std::string &name, int offset) const {
    // 12-edo correction
    if (edo == 12 && offset == -1) {
        if (name == "E") {
            return "D#";
        } else if (name == "B") {
            return "A#";
        }
    }

    std::string symbol;
    if (offset == 0) {
        symbol = scale_symbols[1];
    } else if (offset > 0) {
        symbol = scale_symbols[2];
    } else {
        symbol = scale_symbols[0];
    }

    std::string repeated;
    const int count = std::abs(offset);
    for (int i = 0; i < count; ++i) {
        repeated += symbol;
    }

    return name + repeated;
}
