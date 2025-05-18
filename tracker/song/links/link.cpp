#include "../../constants.hpp"
#include "../../maps/offsets.hpp"
#include "../../structures.hpp"
#include "../../utils/file.hpp"
#include "../output.hpp"
#include "link.hpp"

void Link::assign_output() {
    switch (type) {
    case ItemType::CHANNEL: {
        Channel *channel = reinterpret_cast<Channel *>(item);
        channel->target = table_id;
        break;
    }
    case ItemType::DSP: {
        DSP *dsp = reinterpret_cast<DSP *>(item);
        dsp->target = table_id;
        break;
    }
    case ItemType::COMMANDS: {
        break;
    }
    case ItemType::COUNT:
    default: {
        throw std::runtime_error("Invalid link type: " + std::to_string(static_cast<int>(type)));
    }
    }
}

void Link::serialize(std::ofstream &file) const {
    const size_t group = static_cast<size_t>(target);
    write_data(file, &type, sizeof(type));
    write_data(file, &id, sizeof(id));
    write_data(file, &target, sizeof(target));
    write_data(file, &index, sizeof(index));
    write_data(file, &offset, sizeof(offset));
}

void Link::deserialize(std::ifstream &file) {
    read_data(file, &type, sizeof(type));
    read_data(file, &id, sizeof(id));
    read_data(file, &target, sizeof(target));
    read_data(file, &index, sizeof(index));
    read_data(file, &offset, sizeof(offset));
}
