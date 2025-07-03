#pragma once

#include <utility>

#include "../song/links/key.hpp"
#include "../song/links/type.hpp"
#include "../song/links/target.hpp"

using InputKey = std::pair<ItemType, int>;
using OutputKey = LinkKey;

OutputKey from_input_key(const InputKey &source_key);
