#pragma once

#include <map>
#include <utility>

#include "../../patterns/selection.hpp"

template <typename T>
using PatternSelectionChange = std::map<PatternRow, std::pair<T, T>>;
