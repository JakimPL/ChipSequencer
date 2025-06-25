#pragma once

#include <map>
#include <utility>

#include "../../patterns/selection.hpp"

template <typename T>
struct PatternSelectionChange {
    std::map<PatternRow, std::pair<T, T>> changes;
};
