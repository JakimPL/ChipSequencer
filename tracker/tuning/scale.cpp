#include <algorithm>
#include <cmath>
#include <iostream>

#include "scale.hpp"

ScaleComposer::ScaleComposer()
    : symbols(std::begin(scale_symbols), std::end(scale_symbols)) {
}

void ScaleComposer::compose(const int new_edo) {
    edo = std::clamp(new_edo, MIN_EDO, MAX_EDO);
    scale.clear();

    get_note_centers_and_limits();

    double start = limits.back() - 1.0;
    for (size_t note = 0; note < centers.size(); ++note) {
        const double center_val = centers[note];
        const double end = limits[note];
        const double x = start * edo;
        const double y = end * edo;
        const int center_index = static_cast<int>(std::round(center_val * edo));
        const int lower = static_cast<int>(std::ceil(x));
        const int upper = static_cast<int>(std::floor(y));
        std::vector<int> indices;
        for (int i = lower; i <= upper; ++i) {
            indices.push_back(i);
        }

        int index_pos = 0;
        bool found = false;
        for (size_t j = 0; j < indices.size(); ++j) {
            if (indices[j] == center_index) {
                index_pos = static_cast<int>(j);
                found = true;
                break;
            }
        }

        if (!found) {
            index_pos = static_cast<int>(indices.size() / 2);
        }

        for (size_t j = 0; j < indices.size(); ++j) {
            const int pos = indices[j] % edo;
            std::string note_name = render(scale_names[note], static_cast<int>(j) - index_pos);

            // 12-EDO correction
            if (edo == 12 && note_name == "C♯") {
                note_name = "D♭";
            }

            scale.push_back(note_name);
            if (note_name == "A") {
                a_index = pos;
            }
        }

        start = end;
    }
}

int ScaleComposer::get_edo() const {
    return edo;
}

size_t ScaleComposer::get_a_index() const {
    return a_index;
}

std::vector<std::string> ScaleComposer::get_scale() const {
    return scale;
}

std::string ScaleComposer::render(const std::string &name, const int offset) const {
    const int key = (offset == 0 ? 0 : (offset > 0 ? 1 : -1));
    const std::pair<int, const char *> *found = nullptr;
    for (const auto &p : scale_symbols) {
        if (p.first == key) {
            found = &p;
            break;
        }
    }

    const std::string symbol = (found ? found->second : "");
    std::string repeated;
    int count = std::abs(offset);
    for (int i = 0; i < count; ++i) {
        repeated += symbol;
    }

    return name + repeated;
}

void ScaleComposer::get_note_centers_and_limits() {
    centers.clear();
    limits.clear();
    for (size_t i = 0; i + 1 < scale_intervals_count; ++i) {
        double x = std::log2(scale_intervals[i]);
        double y = std::log2(scale_intervals[i + 1]);
        centers.push_back(x);
        limits.push_back(0.5 * (x + y));
    }
}
