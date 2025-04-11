#include "../constants.hpp"
#include "../maps/offsets.hpp"
#include "../structures.hpp"
#include "../utils/file.hpp"
#include "link.hpp"

#include <iostream>

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
    write_data(file, &type, sizeof(type));
    write_data(file, &id, sizeof(id));
    write_data(file, &target, sizeof(target));
    write_data(file, &index, sizeof(index));
    if (group >= 2) {
        uint16_t _offset = 0;
        try {
            _offset = x32_to_x16[group].at(offset);
        } catch (const std::out_of_range &exception) {
            std::cerr << "Error: " << exception.what() << std::endl;
        }
        write_data(file, &_offset, sizeof(_offset));
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
        try {
            offset = x16_to_x32[group].at(offset);
        } catch (const std::out_of_range &exception) {
            std::cerr << "Error: " << exception.what() << std::endl;
            offset = 0;
        }
    }
}
