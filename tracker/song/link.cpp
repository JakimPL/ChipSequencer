#include "../constants.hpp"
#include "../structures.hpp"
#include "link.hpp"

void Link::set_pointer() {
    switch (type) {
    case ItemType::CHANNEL:
        Channel *channel = static_cast<Channel *>(item);
        channel->output = pointer;
        break;
    case ItemType::DSP:
        void **dsp_output = reinterpret_cast<void **>(item + DSP_OUTPUT);
        *dsp_output = pointer;
        break;
    }
}