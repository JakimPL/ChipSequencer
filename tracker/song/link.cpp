#include "../constants.hpp"
#include "../maps/offsets.hpp"
#include "../structures.hpp"
#include "../utils/file.hpp"
#include "link.hpp"

void Link::assign_output() {
    Channel *channel = reinterpret_cast<Channel *>(item);
    void **dsp_output = reinterpret_cast<void **>(item + DSP_OUTPUT);

    switch (type) {
    case ItemType::CHANNEL:
        channel->output = pointer;
        break;
    case ItemType::DSP:
        *dsp_output = pointer;
        break;
    }
}

void Link::serialize(std::ofstream &file) const {
    const size_t group = static_cast<size_t>(target);
    const uint8_t _offset = x32_to_x16[group].at(offset);
    write_data(file, &type, sizeof(type));
    write_data(file, &id, sizeof(id));
    write_data(file, &target, sizeof(target));
    write_data(file, &index, sizeof(index));
    if (group >= 2) {
        write_data(file, &_offset, sizeof(offset));
    } else {
        write_data(file, &offset, sizeof(offset));
    }
}

void Link::deserialize(std::ifstream &file) {
    read_data(file, &type, sizeof(type));
    read_data(file, &id, sizeof(id));
    read_data(file, &target, sizeof(target));
    read_data(file, &index, sizeof(index));
    read_data(file, &offset, sizeof(offset));
    const size_t group = static_cast<size_t>(target);
    if (group >= 2) {
        offset = x16_to_x32[group].at(offset);
    }
}
