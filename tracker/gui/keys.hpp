#pragma once

#include <utility>

#include "../song/links/key.hpp"
#include "../song/links/type.hpp"
#include "../song/links/target.hpp"

typedef std::pair<ItemType, int> InputKey;
typedef LinkKey OutputKey;

OutputKey from_input_key(const InputKey &source_key);
