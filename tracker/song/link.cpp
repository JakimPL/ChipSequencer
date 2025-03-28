#include "../constants.hpp"
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
    write_data(file, &type, 1);
    write_data(file, &id, 1);
    write_data(file, &target, 1);
    write_data(file, &index, 1);
    write_data(file, &offset, 2);
}
